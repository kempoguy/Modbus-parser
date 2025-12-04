/*
 * modbus_parser.h - Modbus TCP protocol parsing and security analysis
 *
 * This module provides comprehensive Modbus TCP frame parsing, display
 * formatting, and security analysis capabilities. Handles MBAP header
 * validation, function code decoding, and threat detection.
 *
 * Key features:
 * - MBAP header parsing with validation
 * - All standard function codes (0x01-0x2B)
 * - Exception response handling
 * - Security analysis (scanning, timing, exceptions)
 * - Dual display modes (table/verbose)
 * - Markdown report generation
 * 
 * Copyright (C) 2025 Marty
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifndef MODBUS_PARSER_H
#define MODBUS_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/**
 * enum display_mode_t - Display mode selection for frame output
 *
 * Controls the verbosity and format of frame display output.
 */

typedef enum {
    DISPLAY_TABLE,      // Compact table format
    DISPLAY_VERBOSE     // Detailed breakdown
} display_mode_t;



/**
 * struct modbus_mbap_header - Modbus Application Protocol header structure
 *
 * @transaction_id: Request/response correlation ID (0x0000-0xFFFF)
 * @protocol_id: Protocol identifier (always 0x0000 for Modbus TCP)
 * @length: Number of bytes following (unit_id + PDU length)
 * @unit_id: Unit/slave identifier (0x00-0xFF, 0xFF=broadcast)
 * 
 * Fixed 7-byte header at the start of every Modbus TCP frame.
 * All multi-byte fields are big-endian (network byte order).
 *
 * Wire format:
 * Offset 0-1: Transaction ID (uint16_t, big-endian)
 * Offset 2-3: Protocol ID (uint16_t, big-endian, must be 0x0000)
 * Offset 4-5: Length (uint16_t, big-endian, bytes following this field)
 * Offset 6:   Unit ID (uint8_t, target device address)
 */

typedef struct {
    uint16_t transaction_id;
    uint16_t protocol_id;
    uint16_t length;
    uint8_t unit_id;
} modbus_mbap_header_t;



/**
 * modbus_tcp_frame_t() - Complete parsed Modbus TCP frame structure
 * @modbus_mbap_header_t: MBAP header (7 bytes)
 * @function_code: Modbus function (0x01-0x7F) or exception (0x80+)
 * @data: Function-specific data (heap allocated, can be NULL)
 * @data_length: Length of data field in bytes
 *
 * Contains the MBAP header and Protocol Data Unit (PDU).
 * The data field is dynamically allocated and must be freed
 * via modbus_free_frame() after use.
 *
 * Memory management:
 * - Structure itself: typically stack-allocated
 * - data field: heap-allocated by modbus_parse_frame()
 * - Caller must call modbus_free_frame() to release data
 */

typedef struct {
    modbus_mbap_header_t mbap;
    uint8_t function_code;
    uint8_t *data;
    uint16_t data_length;
} modbus_tcp_frame_t;


/**
 * modbus_exception_code_t - Standard Modbus exception codes
 *
 * Exception codes returned in exception response frames (function code 0x80+).
 * The exception code is the first byte of the data field in an exception
 * response.
 *
 * See Modbus specification section 7 for detailed descriptions.
 */

typedef enum {
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_EXCEPTION_SERVER_DEVICE_FAILURE = 0x04,
    MODBUS_EXCEPTION_ACKNOWLEDGE = 0x05,
    MODBUS_EXCEPTION_SERVER_DEVICE_BUSY = 0x06,
    MODBUS_EXCEPTION_MEMORY_PARITY_ERROR = 0x08,
    MODBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MODBUS_EXCEPTION_GATEWAY_TARGET_NO_RESPONSE = 0x0B
} modbus_exception_code_t;


/**
 * attack_stats_t() - Security analysis statistics accumulator
 * @total_frames: Total frames processed
 * @exception_count: Exception responses seen
 * @unique_functions_seen: Count of unique function codes
 * @sequential_probes: Consecutive sequential function codes
 * @function_codes_seen: Bitmap of observed function codes
 * @last_function_code: Previous function code (for pattern detection)
 * @sequential_pattern_detected: True if scannign pattern identified
 * @exception_rate: Calculated percentage (0.0-100.0)
 * @report_file: Open report file handle (NULL if disabled)
 * @report_enabled: Report generation flag
 * @first_packet_time: Timestamp of first frame (seconds)
 * @last_packet_time: Timestamp of last frame
 * @total_duration: Time span of capture
 * @avg_frame_rate: Frames per second
 * @rapid_burst_count: Frames < 0.1s apart (possible flood)
 *
 * Tracks frame counters, function code usage, timing patterns, and
 * threat indicators across all processed frames. Used to detect
 * reconnaissance, scanning, and anomalous behavior.
 *
 * Initialize with modbus_init_attack_stats(), update per-frame with
 * modbus_update_attack_stats(), and finalize with modbus_finalize_attack_stats
 * ().
 */

typedef struct {
    uint32_t total_frames;
    uint32_t exception_count;
    uint32_t unique_functions_seen;
    uint32_t sequential_probes;
    bool function_codes_seen[256];
    uint8_t last_function_code;
    bool sequential_pattern_detected;
    float exception_rate;
        
    // Report generation fields
    FILE *report_file;
    bool report_enabled;

    // Timing analysis
    double first_packet_time;
    double last_packet_time;
    double total_duration;
    double avg_frame_rate;
    uint32_t rapid_burst_count;  // Frames within 0.1s of previous
} attack_stats_t;


/**
 * modbus_get_function_name() - Get human-readable name for function
 * @function_code: Modbus function code (0x00-0xff)
 *
 * Returns descriptive name for standard Modbus function codes.
 * Handles both normal functions (0x01-0x7F) and exception responses (0x80+).
 *
 * Supported function codes:
 * 0x01-0x06, 0x0F-0x10: Standard read/write functions
 * 0x14-0x18: File record and FIFO operations
 * 0x2B: Encapsulated Interface Transport
 * 0x80+: Exception responses (returns "Exception Response: <function>")
 *
 * Return: Static string with function name (never NULL)
 *         "Unknown Function" for undefined codes
 *         String must not be freed by caller
 */

const char* modbus_get_function_name(uint8_t function_code);


// Parse Modbus TCP frame from raw bytes
/**
 * modbus_parse_frame() - Parse Modbus TCP frame from raw bytes
 * @payload: Raw frame data from network (MBAP header + PDU)
 * @payload_len: Length of payload in bytes (minimum 8)
 * @frame: Output structure to populate (must not be NULL)
 *
 * Parses MBAP header and PDU from raw bytes into structured format.
 * Validates protocol ID (must be 0x0000) and length field.
 * Allocates memory for frame->data which must be freed via modbus_free_frame().
 *
 * Parsing steps:
 * 1. Validate payload_len >= 8 (MBAP + function code minimum)
 * 2. Extract and validate MBAP header (7 bytes, big-endian)
 * 3. Extract function code (byte 7)
 * 4. Allocate and copy data field (bytes 8+, if any)
 *
 * Return: true on successful parse (frame populated, data allocated)
 *         false on error (invalid header, truncated frame, alloc failure)
 *
 * @note Caller MUST call modbus_free_frame() to release allocated memory
 * @note Do not call free() directly on frame->data
 */

bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len, modbus_tcp_frame_t *frame);


/**
 * modbud_display_frame() - Display detailed verbose frame breakdown
 * @frame: Parsed frame to display
 *
 * Outputs multi-line detailed breakdown to stdout including:
 * - All MBAP header fields
 * - Function code with name
 * - Function-specific data decoding
 *
 * Function-specific decoding:
 * - 0x03/0x04 (Read Registers): Start address + quantity
 * - 0x06 (Write Single): Address + value  
 * - 0x10 (Write Multiple): Start address + quantity + byte count
 * - 0x80+ (Exception): Exception code
 * - Others: Hex dump of data
 *
 * Output format suitable for debugging and learning protocol details.
 * Use modbus_display_frame_table() for compact bulk analysis.
 */

void modbus_display_frame(const modbus_tcp_frame_t *frame);


/**
 * modbus_free_frame() - Frere memory allocated by modbus_parse_frame()
 * @frame: Frame structure to free (must have been populated by 
 *         modbus_parse_frame())
 *
 * Frees the dynamically allocated data field and zeros pointers.
 * Must be called for every successfully parsed frame.
 * Does not free the frame structure itself (typically stack-allocated).
 *
 * Safe to call:
 * - Multiple times (checks for NULL)
 * - On failed parse (data will be NULL)
 *
 * @warning Do not use frame->data after calling this function
 */

void modbus_free_frame(modbus_tcp_frame_t *frame);


/**
 * modbus_display_frame_table() - Display frame in compact table format
 * @frame: Parsed frame to display
 * @src_ip: Source IP address
 * @src_port: Source TCP port
 * @dst_ip: Destination IP address
 * @dst_port: Destination TCP port
 * @packet_number: Sequence number for display (1-based)
 * @timestamp: Packet timestamp (formatted as HH:MM:SS.microseconds)
 * @is_first: If true, prints table header first
 *
 * Outputs single-line table row with color coding:
 * - Cyan: IP addresses
 * - Yellow: Transaction ID
 * - Green: Unit ID
 * - Magenta: Function code
 * - Blue: Data details
 *
 * Call with is_first=true for the first frame only to print header.
 * Subsequent frames should pass is_first=false.
 *
 * Suitable for processing many frames where compact output is needed.
 * Colors use ANSI codes (may not work in all terminals).
 */

void modbus_display_frame_table(const modbus_tcp_frame_t *frame,
                                const char *src_ip, uint16_t src_port,
                                const char *dst_ip, uint16_t dst_port,
                                uint32_t packet_number,
                                double timestamp,
                                bool is_first);


/**
 * modbus_get_exception_name() - Get human-readable exception code name 
 * @exception_code: Modbus exception code (0x01-0x0B)
 * @param2: Description of second parameter
 *
 * Returns descriptive name for standard Modbus exception codes.
 * Used when function_code >= 0x80 indicates an exception response.
 *
 * Return: Static string with exception name (never NULL)
 *         "Unknown Exception" for undefined codes
 *         String must not be freed by caller
 */

const char* modbus_get_exception_name(uint8_t exception_code);


/**
 * modbus_update_sttack_stats() - Update security statistics for a frame
 * @stats: Statistics structure to update
 * @frame: Parsed frame to analyse
 * @timestamp: Frame timestamp for timing analysis
 *
 * Updates per-frame security statistics including:
 * - Frame counters (total, exceptions)
 * - Function code tracking (unique codes, sequential patterns)
 * - Timing analysis (burst detection)
 * - Threat indicators (scanning patterns)
 *
 * Detection logic:
 * - Exception responses: function_code >= 0x80
 * - Sequential scanning: 3+ consecutive sequential function codes
 * - Rapid bursts: frames < 0.1 seconds apart
 *
 * Call once per frame during PCAP processing.
 * Must call modbus_init_attack_stats() before first use.
 * Call modbus_finalize_attack_stats() after all frames processed.
 */

void modbus_update_attack_stats(attack_stats_t *stats, const modbus_tcp_frame_t *frame, double timestamp);


/**
 * modbus_display_attack_summary() - Display security analysis summary
 * @stats: Finalised statistics structure
 * @param2: Description of second parameter
 *
 * Outputs human-readable security analysis to stdout including:
 * - Exception rate (percentage)
 * - Threat indicators (scanning detected, unusual patterns)
 * - Timing anomalies (rapid bursts)
 * - Function code coverage
 *
 * Should be called after modbus_finalize_attack_stats().
 * Output is suitable for terminal display (uses ANSI colors).
 */

void modbus_display_attack_summary(const attack_stats_t *stats);


/**
 * modbus_open_report() - Open markdown report file for writing
 * @stats: Statistics structure to intialise for reporting
 * @pcap_filename: Base filename for report (e.g., "capture,pcap" -> 
 * "capture_analysis.md")
 *
 * Creates and opens markdown report file with naming convention:
 * <input_filename>_analysis.md
 *
 * Return: true on success, false if file creation failed
 */

bool modbus_open_report(attack_stats_t *stats, const char *pcap_filename);


/**
 * modbus_write_report_header() - Write markdown report header and traffic
 * table header
 * @stats: Statistics structure with open report file
 * @pcap_filename: Original PCAP filename for report metadata
 *
 * Writes:
 * - Report title and metadata (date, source file)
 * - Traffic summary table header
 *
 * Call once after modbus_open_report() before writing frames.
 */

void modbus_write_report_header(attack_stats_t *stats, const char *pcap_filename);


/**
 * modbus_write_report_frame() - Write single frame entry to report
 * @stats: Statistics structure with open report file
 * @frame: Parsed frame to report
 * @src_ip: Source IP address
 * @src_port: Source TCP port
 * @dst_ip: Destination IP address
 * @dst_port: Destination TCP port
 * @packet_number: Frame sequence number
 * @timestamp: Frame timestamp
 *
 * Writes markdown table row for the frame including:
 * - Packet number
 * - Formatted timestamp
 * - Source/destination addresses
 * - Transaction ID, Unit ID
 * - Function code with name
 * - Function-specific details
 *
 * Call once per frame during PCAP processing (if reporting enabled).
 * No-op if report not enabled in stats.
 */

void modbus_write_report_frame(attack_stats_t *stats, const modbus_tcp_frame_t *frame,
                                const char *src_ip, uint16_t src_port,
                                const char *dst_ip, uint16_t dst_port,
                                uint32_t packet_number,
                                double timestamp);


/**
 * modbus_write_report_summary() - Write security analysis section to report
 * @stats: Finalised statistics structure
 * @function_counts: Array of 256 counters (perfunction code usage)
 *
 * Writes markdown sections:
 * - Exception rate analysis
 * - Threat indicators
 * - Timing analysis
 * - Function code summary table
 * - Function code coverage
 *
 * Call once after all frames processed and stats finalized.
 */

void modbus_write_report_summary(attack_stats_t *stats, const uint32_t *function_counts);


/**
 * modbus_close_report() - CLose report file and clean up
 * @stats: Statistics structure with open report
 *
 * Flushes and closes the report file.
 * Sets report_file to NULL.
 * No-op if report not enabled.
 */

void modbus_close_report(attack_stats_t *stats);

#endif /* MODBUS_PARSER_H */



