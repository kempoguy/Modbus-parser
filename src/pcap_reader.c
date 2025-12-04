/*
 * pcap_reader.c - PCAP file processing and Modbus TCP traffic extraction
 *
 * Implements packet capture file parsing using libpcap. Handles layer
 * extraction (Ethernet/IP/TCP), port filtering (502), and payload
 * extraction for Modbus TCP frames.
 *
 * Key features:
 * - Robust PCAP parsing via libpcap
 * - Automatic layer skipping (Ethernet, IP options, TCP options)
 * - Port 502 filtering (source or destination)
 * - IP address formatting
 * - Timestamp conversion
 * - Cross-platform support (Windows/Linux/macOS)
 *
 * Copyright (C) 2025 Marty
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "pcap_reader.h"
#include <stdio.h>
#include <sys/time.h>
#include <pcap.h>

#ifdef _WIN32
    #include <winsock.h> // Windows: For network byte order functions
#else
    #include <arpa/inet.h> // *nix: Same
#endif


/* Network protocol header sizes and constants */

/* Ethernet header size */
#define ETHERNET_HEADER_SIZE 14

/* IP header minimum size */
#define IP_HEADER_MIN_SIZE 20

/* TCP header minimum size */
#define TCP_HEADER_MIN_SIZE 20

/* Modbus TCP port */
#define MODBUS_TCP_PORT 502


/**
 * struct ip_header_t - Simplified IPv4 header structure
 *
 * @version_ihl: Version (4 bits) IHL/Header Length (4 bits)
 * @tos: Type of Service
 * @total_length: Total packet length including header
 * @identification: Fragment identification
 * @flags_fragment: Flags (3 bits) + Fragment offset (13 bits)
 * @ttl: Time to Live
 * @protocol: Protocol (6 = TCP, 17 = UDP)
 * @checksum: Header checksum
 * @source_ip: Source IP Address (network byte order)
 * @dest_ip: Destination IP Address (network byte order)
 * 
 * Minimum IPv4 header fields needed for Modbus TCP extraction.
 * Does not include IP options (if present, calculated via IHL).
 *
 * Wire format: 20 bytes minimum (no options)
 */

typedef struct {
    uint8_t version_ihl;        // Version (4 bits) + IHL (4 bits)
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source_ip;
    uint32_t dest_ip;
} ip_header_t;


/**
 * struct tcp_header_t - TCP header structure (simplified)
 * @source_port: Source port number (network byte order)
 * @dest_port: Destination port number (networ kbyte order)
 * @seq_number: Sequence number
 * @ack_number: Acknowledgement number
 * @data_offset_reserved: Data offset (4 bits) + reserved (4 bits)
 * @flags: TCP flags (SYN, ACK, FIN, etc.)
 * @window: Window size
 * @checksum: Header + data checksum
 * @urgent_pointer: Urgent pointer
 *
 * Minimum TCP header fields needed for port extraction and payload location.
 * Does not include TCP options (if present, calculated via data offset).
 *
 * Wire format: 20 bytes minimum (no options)
 */

typedef struct {
    uint16_t source_port;
    uint16_t dest_port;
    uint32_t seq_number;
    uint32_t ack_number;
    uint8_t data_offset_reserved;  // Data offset (4 bits) + reserved (4 bits)
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;
} tcp_header_t;


/**
 * pcap_process_file() - Oricess PCAP file and extract MOdbus TCP payloads
 * @filename: Path to PCAP file (relative or absolute)
 * @callback: Function to invoke for each Modbus TCP frame
 * @user_data: Opaque pointer passed to callback
 *
 * Opens PCAP file via libpcap, iterates through all packets, and invokes
 * callback for each TCP packet on port 502 (Modbus TCP).
 *
 * Processing flow:
 * 1. Open PCAP file (pcap_open_offline)
 * 2. For each packet:
 *    a. Validate minimum size (Ethernet + IP + TCP headers)
 *    b. Skip Ethernet header (14 bytes)
 *    c. Parse IP header, extract IHL for variable length
 *    d. Check protocol == 6 (TCP)
 *    e. Parse TCP header, extract data offset for variable length
 *    f. Check port == 502 (source or destination)
 *    g. Calculate payload offset and extract data
 *    h. Format IP addresses as strings
 *    i. Convert timestamp to double (seconds.microseconds)
 *    j. Invoke callback with payload and metadata
 * 3. Close PCAP file
 *
 * Non-TCP packets and non-port-502 traffic are silently skipped.
 * Empty payloads (e.g., TCP ACK without data) are skipped.
 * Malformed packets are skipped with no error.
 *
 * Return: true if file processed successfully (even if 0 Modbus frames)
 *         false if file open failed or read error occurred
 */

bool pcap_process_file(const char *filename, modbus_payload_callback_t callback, void *user_data) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct pcap_pkthdr *header;
    const uint8_t *packet;
    int result;
    uint32_t packet_count = 0;
    uint32_t modbus_count = 0;

    // Open PCAP file
    handle = pcap_open_offline(filename, errbuf);
    if (handle == NULL) {
        printf("Error opening PCAP file: %s\n", errbuf);
        return false;
    }

    printf("Processing PCAP file: %s\n", filename);
    printf("Looking for Modbus TCP traffic (port %d)...\n\n", MODBUS_TCP_PORT);

    // Read packets
    while ((result = pcap_next_ex(handle, &header, &packet)) >= 0) {
        if (result == 0) {
            continue;  // Timeout
        }

        packet_count++;

        // Validate minimum packet size (Ethernet + IP + TCP headers)
        if (header->caplen < ETHERNET_HEADER_SIZE + IP_HEADER_MIN_SIZE + TCP_HEADER_MIN_SIZE) {
            continue;
        }

        // Skip Ethernet header
        const uint8_t *ip_packet = packet + ETHERNET_HEADER_SIZE;

        // Parse IP header
        ip_header_t *ip_hdr = (ip_header_t *)ip_packet;
        uint8_t ip_header_length = (ip_hdr->version_ihl & 0x0F) * 4;

        // Check if TCP protocol (6)
        if (ip_hdr->protocol != 6) {
            continue;
        }

        // Parse TCP header
        const uint8_t *tcp_packet = ip_packet + ip_header_length;
        tcp_header_t *tcp_hdr = (tcp_header_t *)tcp_packet;
        uint8_t tcp_header_length = ((tcp_hdr->data_offset_reserved >> 4) & 0x0F) * 4;

        // Convert port from network byte order
        uint16_t src_port = ntohs(tcp_hdr->source_port);
        uint16_t dst_port = ntohs(tcp_hdr->dest_port);

        // Check if Modbus TCP port (502)
        if (src_port != MODBUS_TCP_PORT && dst_port != MODBUS_TCP_PORT) {
            continue;
        }

        // Calculate payload offset and length
        uint32_t headers_size = ETHERNET_HEADER_SIZE + ip_header_length + tcp_header_length;
        if (header->caplen <= headers_size) {
            continue;  // No payload
        }

        const uint8_t *payload = packet + headers_size;
        uint32_t payload_length = header->caplen - headers_size;

        // Skip if payload is empty
        if (payload_length == 0) {
            continue;
        }

        modbus_count++;

        // Format IP addresses
        char src_ip_str[16], dst_ip_str[16];
        snprintf(src_ip_str, sizeof(src_ip_str), "%u.%u.%u.%u",
                    (ntohl(ip_hdr->source_ip) >> 24) & 0xFF,
                    (ntohl(ip_hdr->source_ip) >> 16) & 0xFF,
                    (ntohl(ip_hdr->source_ip) >> 8) & 0xFF,
                    ntohl(ip_hdr->source_ip) & 0xFF);

        snprintf(dst_ip_str, sizeof(dst_ip_str), "%u.%u.%u.%u",
                    (ntohl(ip_hdr->dest_ip) >> 24) & 0xFF,
                    (ntohl(ip_hdr->dest_ip) >> 16) & 0xFF,
                    (ntohl(ip_hdr->dest_ip) >> 8) & 0xFF,
                    ntohl(ip_hdr->dest_ip) & 0xFF);
        
        // Convert timestamp to double (seconds.milliseconds)
        double packet_time = (double)header->ts.tv_sec + (double)header->ts.tv_usec / 1000000.0;

        // Call callback with Modbus TCP payload and connection info
        callback(payload, payload_length, src_ip_str, src_port, dst_ip_str, dst_port, packet_time, user_data);
    }

    if (result == -1) {
        printf("Error reading packet: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return false;
    }

    pcap_close(handle);
    return true;
}
