/*
 * pcap_reader.h - PCAP file processing for Modbus TCP traffic extraction
 *
 * This module provides functionality to read PCAP files, filter for Modbus TCP 
 * traffic (port 502), and invoke callbacks with extracted payload data.
 * 
 * Uses libpcap for robust PCAP parsing and handles Ethernet/IP/TCP layer
 * extraction automatically
 *
 * Copyright (C) 2025 Marty
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @version 1.0
 */

#ifndef PCAP_READER_H
#define PCAP_READER_H

#include <stdint.h>
#include <stdbool.h>


/**
 * modbus_payload_callback_t() - Callback function type for processing Modbus TCP payloads
 * 
 * @payload: Pointer to Modbus TCP frame data (MBAP + PDU)
 * @length: Length of payload in bytes (minimum 8 for valid frame)
 * @src_ip: Source IP address as null-terminated string (e.g., "192.168.1.1")
 * @src_port: Source TCP port number
 * @dst_ip: Destination IP address as null-terminated string
 * @dst_port: Destination TCP port (typically 502 for Modbus)
 * @timestamp: Packet timestamp in seconds since epoch (microsecond precision)
 * @user_data: Opaque pointer passed from pcap_process_file() call
 *
 * @note All pointer parameters (payload, IP strings) are only valid during the
 *       callback invocation. Copy data if needed beyond callback scope.
 * @note Callback should not free the payload or IP strings - managed by caller.
 *
 * Return: void
 */

typedef void (*modbus_payload_callback_t)(const uint8_t *payload, uint32_t length, 
                                            const char *src_ip, uint16_t src_port,
                                            const char *dst_ip, uint16_t dst_port,
                                            double timestamp,
                                            void *user_data);



// Process PCAP file and extract Modbus TCP payloads

/**
 * pcap_process_file() - Process PCAP file and extract Modbus TCp payloads
 * @filename: Path to PCAP file (relative or absolute)
 * @callback: Function to invoke for each Modbus TCP payload found
 * @user_data: Opaque pointer passed to callback (can be NULL)
 *
 * Opens the specified PCAP file, iterates through all packets, filters for
 * TCP traffic on port 502 (Modbus TCP), and invokes the callback for each
 * matching frame with the extracted payload and metadata.
 *
 * The function handles:
 * - PCAP file format parsing (via libpcap)
 * - Ethernet/IP/TCP layer extraction
 * - Port 502 filtering (source or destination)
 * - Payload extraction and callback invocation
 *
 * Non-TCP packets and non-port-502 traffic are silently skipped.
 * Malformed packets generate warnings but processing continues.
 *
 * Return: true if file was successfully processed (even if 0 Modbus frames 
 * found) false if file could not be opened or fatal error occurred
 */

bool pcap_process_file(const char *filename, modbus_payload_callback_t callback, void *user_data);

#endif /* PCAP_READER_H */
