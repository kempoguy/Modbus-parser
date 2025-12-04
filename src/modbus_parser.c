/*
 * modbus_parser.c - Modbus TCP protocol parser implementation
 *
 * Implements complete Modbus TCP frame parsing, validation, display,
 * and security analysis. Handles all standard function codes, exception
 * responses, and generates comprehensive security reports.
 *
 * Key components:
 * - MBAP header parsing and validation
 * - Function code decoding (0x01-0x2B plus exceptions)
 * - Dual display modes (table/verbose)
 * - Security analysis (exception rate, scanning, timing)
 * - Markdown report generation
 *
 * Security analysis detects:
 * - High exception rates (>20%)
 * - Sequential function code scanning
 * - Rapid burst patterns (<0.1s intervals)
 * - Wide function code coverage
 * Copyright (C) 2025 Marty
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "modbus_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "colors.h"

#ifdef _WIN32
    #include <winsock2.h>  //Windows: For ntohs (network to host short)
#else
    #include <arpa/inet.h>  // *nix: For ntohs (network to host short)
#endif


/*
 * Minimum valid Modbus TCP frame size in bytes
 * MBAP header (7 bytes) + Function code (1 byte) = 8 bytes minimum
 * Data field is optional and varies by function code
 */

#define MIN_MODBUS_FRAME_SIZE 8


/*
 * modbus_get_function_name() - Get human-readable function code name
 * @function_code: Modbus function code (0x00-0xFF)
 *
 * Maps function codes to descriptive names. Handles standard functions
 * (0x01-0x2B) including obsolete codes, and detects exception responses
 * (0x80+ bit set).
 *
 * Return: Static string with function name (never NULL)
 *         "Exception Response" for codes with high bit set (0x80+)
 *         "Unknown/Reserved Function" for undefined codes
 *         String must not be freed by caller
 */

const char* modbus_get_function_name(uint8_t function_code) {
    // Check for exception response (high bit set)
    if (function_code & 0x80) {
        return "Exception Response";
    }
    
    switch(function_code) {
        // Bit/Coil Access
        case 0x01: return "Read Coils";
        case 0x02: return "Read Discrete Inputs";
        case 0x05: return "Write Single Coil";
        case 0x0F: return "Write Multiple Coils";
        
        // Register Access
        case 0x03: return "Read Holding Registers";
        case 0x04: return "Read Input Registers";
        case 0x06: return "Write Single Register";
        case 0x10: return "Write Multiple Registers";
        case 0x16: return "Mask Write Register";
        case 0x17: return "Read/Write Multiple Registers";
        case 0x18: return "Read FIFO Queue";
        
        // File Record Access
        case 0x14: return "Read File Record";
        case 0x15: return "Write File Record";
        
        // Diagnostics
        case 0x07: return "Read Exception Status";
        case 0x08: return "Diagnostic";
        case 0x0B: return "Get Comm Event Counter";
        case 0x0C: return "Get Comm Event Log";
        case 0x11: return "Report Server ID";
        case 0x2B: return "Encapsulated Interface Transport";
        
        // Program/Configuration Functions
        case 0x09: return "Program Controller (Obsolete)";
        case 0x0A: return "Poll Controller (Obsolete)";
        case 0x0D: return "Program 484 (Obsolete)";
        case 0x0E: return "Poll 484 (Obsolete)";
        
        // Other Functions
        case 0x12: return "Read General Reference (Obsolete)";
        case 0x13: return "Write General Reference (Obsolete)";
        
        default:
            return "Unknown/Reserved Function";
    }
}


/**
 * modbus_get_exception_name() - Get human-readable exception code name
 * @exception_code: Exception code from exception response (0x01-0x0B)
 *
 * Maps exception codes to descriptive error messages.
 * Used when function_code >= 0x80 indicates an exception response.
 *
 * Return: Static string with exception description (never NULL)
 *         "Unknown Exception" for undefined codes
 *         String must not be freed by caller
 */

const char* modbus_get_exception_name(uint8_t exception_code) {
    switch(exception_code) {
        case MODBUS_EXCEPTION_ILLEGAL_FUNCTION:
            return "Illegal Function";
        case MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS:
            return "Illegal Data Address";
        case MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE:
            return "Illegal Data Value";
        case MODBUS_EXCEPTION_SERVER_DEVICE_FAILURE:
            return "Server Device Failure";
        case MODBUS_EXCEPTION_ACKNOWLEDGE:
            return "Acknowledge";
        case MODBUS_EXCEPTION_SERVER_DEVICE_BUSY:
            return "Server Device Busy";
        case MODBUS_EXCEPTION_MEMORY_PARITY_ERROR:
            return "Memory Parity Error";
        case MODBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE:
            return "Gateway Path Unavailable";
        case MODBUS_EXCEPTION_GATEWAY_TARGET_NO_RESPONSE:
            return "Gateway Target No Response";
        default:
            return "Unknown Exception";
    }
}


/**
 * modbus_parse_frame() - Parse Modbus TCP frame from raw bytes
 * @payload: Raw frame data (MBAP header + PDU)
 * @payload_len: Length of payload in bytes
 * @frame: Output structure to populate
 *
 * Parses and validates Modbus TCP frame structure:
 * 1. Validates minimum size (8 bytes)
 * 2. Extracts MBAP header fields (big-endian)
 * 3. Validates protocol ID (must be 0x0000)
 * 4. Validates length field consistency
 * 5. Extracts function code
 * 6. Allocates and copies data field (if present)
 *
 * Memory management:
 * - Allocates frame->data via malloc() if data_length > 0
 * - Caller MUST call modbus_free_frame() to release memory
 * - On failure, no memory is allocated
 *
 * Return: true if frame parsed successfully (frame populated)
 *         false if validation failed or allocation error
 */

bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len, modbus_tcp_frame_t *frame) {
    // Validate minimum size
    if (payload_len < MIN_MODBUS_FRAME_SIZE) {
        return false;
    }

    // Parse MBAP Header (7 bytes)
    // Modbus TCP uses big-endian (network byte order)
    frame->mbap.transaction_id = ntohs(*(uint16_t*)&payload[0]);
    frame->mbap.protocol_id = ntohs(*(uint16_t*)&payload[2]);
    frame->mbap.length = ntohs(*(uint16_t*)&payload[4]);
    frame->mbap.unit_id = payload[6];

    // Validate Protocol ID (must be 0x0000 for Modbus)
    if (frame->mbap.protocol_id != 0x0000) {
        printf("Error: Invalid Protocol ID (0x%04X, expected 0x0000)\n", 
               frame->mbap.protocol_id);
        return false;
    }

    // Parse PDU
    frame->function_code = payload[7];
    
    // Calculate data length (MBAP length includes unit_id + PDU)
    // PDU = function_code (1 byte) + data
    frame->data_length = frame->mbap.length - 2;  // Subtract unit_id and function_code
    
    // Allocate and copy data if present
    if (frame->data_length > 0) {
        frame->data = malloc(frame->data_length);
        if (frame->data == NULL) {
            printf("Error: Memory allocation failed\n");
            return false;
        }
        memcpy(frame->data, &payload[8], frame->data_length);
    } else {
        frame->data = NULL;
    }

    return true;
}


/**
 * modbus_display_frame_table() - Display frame in compact table format
 * @frame: Parsed frame to display
 * @src_ip: Source IP address string
 * @src_port: Source TCP port
 * @dst_ip: Destination IP address string
 * @dst_port: Destination TCP port
 * @packet_number: Sequence number for display
 * @timestamp: Packet timestamp (seconds since epoch)
 * @is_first: If true, print table header first
 *
 * Outputs single-line table row with ANSI color coding:
 * - Cyan: IP addresses
 * - Yellow: Transaction ID
 * - Green: Unit ID
 * - Magenta: Function code
 * - Blue: Data details
 *
 * Call with is_first=true only for the first frame to print header.
 * Formats timestamp as HH:MM:SS.microseconds.
 * Decodes function-specific data fields (addresses, quantities, etc.).
 */

void modbus_display_frame_table(const modbus_tcp_frame_t *frame,
                                const char *src_ip, uint16_t src_port,
                                const char *dst_ip, uint16_t dst_port,
                                uint32_t packet_number,
                                double timestamp,
                                bool is_first) {
    if (is_first) {
        printf("\n%s%-8s %-15s %-22s %-22s %-10s %-6s %-30s %-80s%s\n", COLOR_WHITE,
                "Packet", "Timestamp", "Source IP:Port", "Dest IP:Port", "Trans ID", "Unit", "Function", "Details", COLOR_RESET);
        printf("%s----------------------------------------------------------------------"
               "----------------------------------------------------------------------------%s\n",
               COLOR_GRAY, COLOR_RESET);
             
    }

    // Format source and destination
    char src[24], dst[24];
    snprintf(src, sizeof(src), "%s:%u", src_ip, src_port);
    snprintf(dst, sizeof(dst), "%s:%u", dst_ip, dst_port);

    // Get function name
    const char *func_name = modbus_get_function_name(frame->function_code);

    // Parse address/quantity or exception details
    char addr_qty[100] = "-";
    
    // Check if this is an exception response
    if (frame->function_code & 0x80) {
        if (frame->data_length >= 1) {
            uint8_t exception_code = frame->data[0];
            const char *exception_name = modbus_get_exception_name(exception_code);

            // Get the original function code (strip exception bit)
            uint8_t original_function = frame->function_code & 0x7F;
            const char *original_function_name = modbus_get_function_name(original_function);

            snprintf(addr_qty, sizeof(addr_qty), "Exception: %s (FC=0x%02X: %s)", exception_name, original_function, original_function_name);
        } else {
            snprintf(addr_qty, sizeof(addr_qty), "Exception (no data)");
        }
    } else if (frame->data_length >= 4) {
        // Parse address and quantity for common read/write functions
        if (frame->function_code >= 0x01 && frame->function_code <= 0x06) {
            uint16_t address = ntohs(*(uint16_t*)&frame->data[0]);
            uint16_t quantity = ntohs(*(uint16_t*)&frame->data[2]);
            snprintf(addr_qty, sizeof(addr_qty), "%u/%u", address, quantity);
        } else if (frame->function_code == 0x0F || frame->function_code == 0x10) {
            uint16_t address = ntohs(*(uint16_t*)&frame->data[0]);
            uint16_t quantity = ntohs(*(uint16_t*)&frame->data[2]);
            snprintf(addr_qty, sizeof(addr_qty), "%u/%u", address, quantity);
        }
    }

    // Format timestamp as HH:MM:SS.microseconds
    time_t sec = (time_t)timestamp;
    struct tm *tm_info = localtime(&sec);
    int microsec = (int)((timestamp - (double)sec) * 1000000);
    char time_str[20];
    snprintf(time_str, sizeof(time_str), "%02d:%-2d:%02d.%06d",
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, microsec);

    // Print colored row
    printf("%s%-8u%s%s%-16s %s%s%-22s%-22s%s%s0x%04X    %s%s0x%02X  %s%s%-30s%s%s%-80s%s\n", 
           COLOR_WHITE, packet_number, COLOR_RESET,
           COLOR_GRAY, time_str, COLOR_RESET,
           COLOR_CYAN, src, dst, COLOR_RESET,
           COLOR_YELLOW, frame->mbap.transaction_id, COLOR_RESET, 
           COLOR_GREEN, frame->mbap.unit_id, COLOR_RESET,
           COLOR_MAGENTA, func_name, COLOR_RESET,
           COLOR_BLUE, addr_qty, COLOR_RESET);
}


/**
 * modbus_display_frame() - Display detailed verbose frame breakdown
 * @frame: Parsed frame to display
 *
 * Outputs multi-line detailed frame analysis to stdout:
 * - MBAP header fields (transaction ID, protocol ID, length, unit ID)
 * - Function code with descriptive name
 * - Function-specific data decoding
 *
 * Function-specific decoding includes:
 * - 0x03/0x04 (Read Registers): Start address + quantity
 * - 0x06 (Write Single Register): Address + value
 * - 0x10 (Write Multiple Registers): Start address + quantity + byte count
 * - 0x80+ (Exception): Exception code + description
 * - Others: Hex dump of data
 *
 * Suitable for debugging and protocol analysis.
 * Use modbus_display_frame_table() for bulk processing.
 */

void modbus_display_frame(const modbus_tcp_frame_t *frame) {
    printf("\n=== Modbus TCP Frame ===\n");
    
    // MBAP Header
    printf("\nMBAP Header:\n");
    printf("  Transaction ID:  0x%04X (%u)\n", 
           frame->mbap.transaction_id, frame->mbap.transaction_id);
    printf("  Protocol ID:     0x%04X (Modbus)\n", frame->mbap.protocol_id);
    printf("  Length:          %u bytes\n", frame->mbap.length);
    printf("  Unit ID:         0x%02X\n", frame->mbap.unit_id);
    
    // PDU
    printf("\nProtocol Data Unit (PDU):\n");
    printf("  Function Code:   0x%02X (%s)\n", 
           frame->function_code, modbus_get_function_name(frame->function_code));
    
    // Data
    if (frame->data_length > 0) {
        printf("  Data Length:     %u bytes\n", frame->data_length);
        printf("  Data (hex):      ");
        for (uint16_t i = 0; i < frame->data_length; i++) {
            printf("%02X ", frame->data[i]);
            if ((i + 1) % 16 == 0 && i < frame->data_length - 1) {
                printf("\n                   ");
            }
        }
        printf("\n");
    } else {
        printf("  Data:            (none)\n");
    }
    
    printf("\n========================\n");
}


/**
 * modbus_free_frame() - Free memory allocated by modbus_parse_frame()
 * @frame: Frame structure to clean up
 *
 * Releases dynamically allocated data field and zeros pointers.
 * Safe to call multiple times or on failed parse (checks for NULL).
 * Does not free the frame structure itself (typically stack-allocated).
 *
 * Must be called for every successfully parsed frame to prevent memory leaks.
 */

void modbus_free_frame(modbus_tcp_frame_t *frame) {
    if (frame->data != NULL) {
        free(frame->data);
        frame->data = NULL;
    }
}


/**
 * modbus_uppdate_attack_stats() - Update security statistics for frame
 * @stats: Statistics structure to update
 * @frame: Parsed frame to analyze
 * @timestamp: Frame timestamp for timing analysis
 *
 * Accumulates per-frame security metrics:
 * - Total frame count
 * - Exception response count (function_code >= 0x80)
 * - Unique function codes seen
 * - Sequential pattern detection (3+ consecutive codes)
 * - Timing analysis (first/last packet, burst detection)
 *
 * Burst detection: Frames < 0.1 seconds apart are flagged as rapid bursts
 * which may indicate automated scanning or flood attempts.
 *
 * Sequential pattern: Detects 3+ consecutive function codes
 * (e.g., 0x01, 0x02, 0x03) suggesting systematic enumeration.
 *
 * Call once per frame during processing.
 * Initialize stats to zero before first call.
 * Call modbus_display_attack_summary() or modbus_write_report_summary()
 * after all frames processed.
 */

void modbus_update_attack_stats(attack_stats_t *stats, const modbus_tcp_frame_t *frame, double timestamp) {
    // Initialise timing on first frame
    if (stats->total_frames == 0) {
        stats->first_packet_time = timestamp;
        stats->last_packet_time = timestamp;
    } else {
        // Check for rapid burst (within 0.1 seconds of previous)
        double time_diff = timestamp - stats->last_packet_time;
        if (time_diff < 0.1 && time_diff > 0) {
            stats->rapid_burst_count++;
        }
        stats->last_packet_time = timestamp;
    }
    
    stats->total_frames++;
    
    // Track exception responses
    if (frame->function_code & 0x80) {
        stats->exception_count++;
    }
    
    // Track unique function codes
    uint8_t base_code = frame->function_code & 0x7F;  // Strip exception bit
    if (!stats->function_codes_seen[base_code]) {
        stats->function_codes_seen[base_code] = true;
        stats->unique_functions_seen++;
    }
    
    // Detect sequential probing (function codes incrementing)
    if (stats->total_frames > 1) {
        if (base_code == stats->last_function_code + 1 || 
            base_code == stats->last_function_code + 2) {  // Allow skip of 1
            stats->sequential_probes++;
            if (stats->sequential_probes >= 5) {
                stats->sequential_pattern_detected = true;
            }
        }
    }
    
    stats->last_function_code = base_code;
    
    // Calculate exception rate
    if (stats->total_frames > 0) {
        stats->exception_rate = (float)stats->exception_count / (float)stats->total_frames * 100.0f;
    }

    // Calculate timing metrics
    if (stats->total_frames > 1) {
        stats->total_duration = stats->last_packet_time - stats->first_packet_time;
        if (stats->total_duration > 0) {
            stats->avg_frame_rate = (double)stats->total_frames / stats->total_duration;
        }
    }
}


/**
 * modbus_display_attack_summary() - Display security analysis to console
 * @stats: Finalized statistics structure
 *
 * Outputs security analysis summary to stdout with ANSI color coding:
 * - Exception rate analysis
 * - Threat indicators (scanning, unusual patterns)
 * - Timing analysis (burst detection)
 * - Function code coverage
 *
 * Threat thresholds:
 * - Exception rate >20%: Potential scanning or misconfiguration
 * - Sequential pattern detected: Systematic enumeration
 * - High burst count: Automated tools or flood attempts
 *
 * Call after processing all frames and calculating final metrics.
 */

void modbus_display_attack_summary(const attack_stats_t *stats) {
    printf("\n%s=== Security Analysis ===%s\n", COLOR_WHITE, COLOR_RESET);
    
    // Exception rate analysis
    printf("\n%sException Rate Analysis:%s\n", COLOR_WHITE, COLOR_RESET);
    printf("  Total Frames:        %u\n", stats->total_frames);
    printf("  Exception Responses: %s%u%s (%.1f%%)\n", 
           stats->exception_rate > 50.0f ? COLOR_YELLOW : COLOR_GREEN,
           stats->exception_count, COLOR_RESET, stats->exception_rate);
    
    // Threat indicators
    printf("\n%sThreat Indicators:%s\n", COLOR_WHITE, COLOR_RESET);
    
    bool threat_detected = false;
    
    // High exception rate
    if (stats->exception_rate > 70.0f) {
        printf("  %s[!] HIGH EXCEPTION RATE%s - %.1f%% exceptions (likely scanning)\n",
               COLOR_YELLOW, COLOR_RESET, stats->exception_rate);
        threat_detected = true;
    }
    
    // Sequential probing
    if (stats->sequential_pattern_detected) {
        printf("  %s[!] SEQUENTIAL PROBING%s - Function codes tested in sequence\n",
               COLOR_YELLOW, COLOR_RESET);
        threat_detected = true;
    }
    
    // Wide function code coverage
    if (stats->unique_functions_seen > 10) {
        printf("  %s[!] BROAD ENUMERATION%s - %u different function codes tested\n",
               COLOR_YELLOW, COLOR_RESET, stats->unique_functions_seen);
        threat_detected = true;
    }
    
    if (!threat_detected) {
        printf("  %s[✓] No obvious scanning patterns detected%s\n", 
               COLOR_GREEN, COLOR_RESET);
    }

    // Timing Analysis
    printf("\n%sTiming Analysis:%s\n", COLOR_WHITE, COLOR_RESET);
    printf("  Total Duration:      %.2f seconds\n", stats->total_duration);
    printf("  Average Frame Rate:  %.2f frames/second\n", stats->avg_frame_rate);
    printf("  Rapid Bursts:        %u frames (< 0.1s apart)\n", stats->rapid_burst_count);
    
    // High rate scanning indicator
    if (stats->avg_frame_rate > 10.0) {
        printf("  %s[!] HIGH FRAME RATE%s - %.1f fps (automated scannign likely)\n",
                COLOR_YELLOW, COLOR_RESET, stats->avg_frame_rate);
    }

    // Function coverage details
    printf("\n%sFunction Code Coverage:%s\n", COLOR_WHITE, COLOR_RESET);
    printf("  Unique functions tested: %u\n", stats->unique_functions_seen);
    
    // List probed functions
    printf("  Codes observed: ");
    uint32_t count = 0;
    for (int i = 0; i < 256; i++) {
        if (stats->function_codes_seen[i]) {
            if (count > 0) printf(", ");
            printf("0x%02X", i);
            count++;
            if (count % 10 == 0 && count < stats->unique_functions_seen) {
                printf("\n                  ");
            }
        }
    }
    printf("\n");
}


/**
 * modbus_open_report() - Open markdown report file for writing
 * @stats: Statistics structure to initialize
 * @pcap_filename: Base filename (e.g., "capture.pcap")
 *
 * Creates report file with naming convention:
 * <pcap_filename>_analysis.md
 *
 * Initializes report_file handle in stats structure.
 * Sets report_enabled flag on success.
 *
 * Return: true if file opened successfully
 *         false if file creation failed
 *
 * On failure, prints error message and disables reporting.
 */

bool modbus_open_report(attack_stats_t *stats, const char *pcap_filename) {

    // Build report filename from PCAP filename
    char report_filename[512];
    
    // Find the last directory separator and last dot
    const char *last_slash = strrchr(pcap_filename, '/');
    const char *last_backslash = strrchr(pcap_filename, '\\');
    const char *filename_start = pcap_filename;
    
    // Use whichever separator was found last
    if (last_slash && last_backslash) {
        filename_start = (last_slash > last_backslash) ? last_slash + 1 : last_backslash + 1;
    } else if (last_slash) {
        filename_start = last_slash + 1;
    } else if (last_backslash) {
        filename_start = last_backslash + 1;
    }
    
    // Copy the directory path
    size_t dir_len = filename_start - pcap_filename;
    if (dir_len > 0) {
        strncpy(report_filename, pcap_filename, dir_len);
        report_filename[dir_len] = '\0';
    } else {
        report_filename[0] = '\0';
    }
    
    // Find the extension
    const char *dot = strrchr(filename_start, '.');
    size_t base_len = dot ? (size_t)(dot - filename_start) : strlen(filename_start);
    
    // Append base filename and new extension
    strncat(report_filename, filename_start, base_len);
    strcat(report_filename, "_analysis.md");
    
    // Open file for writing
    stats->report_file = fopen(report_filename, "w");
    if (!stats->report_file) {
        printf("Error: Could not create report file: %s\n", report_filename);
        return false;
    }
    
    printf("Writing report to: %s\n", report_filename);
    stats->report_enabled = true;
    return true;
}


/**
 * modbus_write_report_header() - Write markdown report header
 * @stats: Statistics structure with open report file
 * @pcap_filename: Original PCAP filename for metadata
 *
 * Writes:
 * - Report title ("Modbus TCP Security Analysis Report")
 * - Generation timestamp
 * - Source file reference
 * - Traffic summary table header
 *
 * Call once after modbus_open_report() and before writing frames.
 * No-op if report not enabled.
 */

void modbus_write_report_header(attack_stats_t *stats, const char *pcap_filename) {
    if (!stats->report_enabled || !stats->report_file) return;
    
    FILE *f = stats->report_file;
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(f, "# Modbus TCP Security Analysis Report\n\n");
    fprintf(f, "**Generated:** %s  \n", timestamp);
    fprintf(f, "**Source File:** `%s`  \n\n", pcap_filename);
    fprintf(f, "---\n\n");
    fprintf(f, "## Traffic Summary\n\n");
    fprintf(f, "| Packet | Timestamp | Source | Destination | Trans ID | Unit | Function | Details |\n");
    fprintf(f, "|--------|-----------|--------|-------------|----------|------|----------|---------|\n");
}


/**
 * modbus_write_report_frame() - Write single frame entry to report
 * @stats: Statistics structure with open report file
 * @frame: Parsed frame to report
 * @src_ip: Source IP address
 * @src_port: Source TCP port
 * @dst_ip: Destination IP address
 * @dst_port: Destination TCP port
 * @packet_number: Frame sequence number
 * @timestamp: Frame timestamp (seconds since epoch)
 *
 * Writes markdown table row containing:
 * - Packet number
 * - Formatted timestamp (HH:MM:SS.microseconds)
 * - Source and destination addresses
 * - Transaction ID, Unit ID
 * - Function code with name
 * - Function-specific details
 *
 * Call once per frame during PCAP processing.
 * No-op if report not enabled.
 */

void modbus_write_report_frame(attack_stats_t *stats, const modbus_tcp_frame_t *frame,
                                const char *src_ip, uint16_t src_port,
                                const char *dst_ip, uint16_t dst_port,
                                uint32_t packet_number,
                                double timestamp) {
    if (!stats->report_enabled || !stats->report_file) return;
    
    FILE *f = stats->report_file;

    // Format timestamp
    time_t sec = (time_t)timestamp;
    struct tm *tm_info = localtime(&sec);
    int microsec = (int)((timestamp - (double)sec) * 1000000);
    char time_str[20];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d.%06d",
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, microsec);

    // Format source and destination
    char src[32], dst[32];
    snprintf(src, sizeof(src), "%s:%u", src_ip, src_port);
    snprintf(dst, sizeof(dst), "%s:%u", dst_ip, dst_port);
    
    // Get function name
    const char *func_name = modbus_get_function_name(frame->function_code);
    
    // Build details string
    char details[128] = "-";
    
    if (frame->function_code & 0x80) {
        // Exception response
        if (frame->data_length >= 1) {
            uint8_t exception_code = frame->data[0];
            const char *exception_name = modbus_get_exception_name(exception_code);
            uint8_t original_function = frame->function_code & 0x7F;
            const char *original_function_name = modbus_get_function_name(original_function);
            
            snprintf(details, sizeof(details), "Exception: %s (FC=0x%02X: %s)", 
                     exception_name, original_function, original_function_name);
        }
    } else if (frame->data_length >= 4) {
        // Normal request with address/quantity
        if (frame->function_code >= 0x01 && frame->function_code <= 0x06) {
            uint16_t address = ntohs(*(uint16_t*)&frame->data[0]);
            uint16_t quantity = ntohs(*(uint16_t*)&frame->data[2]);
            snprintf(details, sizeof(details), "Addr=%u, Qty=%u", address, quantity);
        } else if (frame->function_code == 0x0F || frame->function_code == 0x10) {
            uint16_t address = ntohs(*(uint16_t*)&frame->data[0]);
            uint16_t quantity = ntohs(*(uint16_t*)&frame->data[2]);
            snprintf(details, sizeof(details), "Addr=%u, Qty=%u", address, quantity);
        }
    }
    
    fprintf(f, "| %u | %s | %s | %s | 0x%04X | 0x%02X | %s | %s |\n",
            packet_number, time_str, src, dst, frame->mbap.transaction_id, 
            frame->mbap.unit_id, func_name, details);
}


/**
 * modbus_write_report_summary() - Write security analysis section
 * @stats: Finalized statistics structure
 * @function_counts: Array of 256 per-function-code counters
 *
 * Writes markdown sections:
 * - Security Analysis header
 * - Exception rate analysis
 * - Threat indicators (scanning, bursts)
 * - Timing analysis (duration, frame rate, bursts)
 * - Function code summary table
 * - Function code coverage
 *
 * Calculates and displays:
 * - Exception rate percentage
 * - Average frame rate (frames/second)
 * - Total capture duration
 * - Threat assessments based on thresholds
 *
 * Call once after all frames processed.
 * No-op if report not enabled.
 */

void modbus_write_report_summary(attack_stats_t *stats, const uint32_t *function_counts) {
    if (!stats->report_enabled || !stats->report_file) return;
    
    FILE *f = stats->report_file;
    
    fprintf(f, "\n---\n\n");
    fprintf(f, "## Security Analysis\n\n");
   
    // Exception rate analysis
    fprintf(f, "### Exception Rate Analysis\n\n");
    fprintf(f, "- **Total Frames:** %u\n", stats->total_frames);
    fprintf(f, "- **Exception Responses:** %u (%.1f%%)\n", 
            stats->exception_count, stats->exception_rate);
    
    // Threat indicators
    fprintf(f, "\n### Threat Indicators\n\n");
    
    bool threat_detected = false;
    
    if (stats->exception_rate > 70.0f) {
        fprintf(f, "- ⚠️ **HIGH EXCEPTION RATE** - %.1f%% exceptions (likely scanning)\n",
                stats->exception_rate);
        threat_detected = true;
    }
    
    if (stats->sequential_pattern_detected) {
        fprintf(f, "- ⚠️ **SEQUENTIAL PROBING** - Function codes tested in sequence\n");
        threat_detected = true;
    }
    
    if (stats->unique_functions_seen > 10) {
        fprintf(f, "- ⚠️ **BROAD ENUMERATION** - %u different function codes tested\n",
                stats->unique_functions_seen);
        threat_detected = true;
    }
    
    if (!threat_detected) {
        fprintf(f, "- ✅ No obvious scanning patterns detected\n");
    }

    // Timing analysis
    fprintf(f, "\n### Timing Analysis\n\n");
    fprintf(f, "- **Total Duration:** %.2f seconds\n", stats->total_duration);
    fprintf(f, "- **Average Frame Rate:** %.2f frames/second\n", stats->avg_frame_rate);
    fprintf(f, "- **Rapid Bursts:** %u frames (< 0.1s apart)\n\n", stats->rapid_burst_count);
    
    if (stats->avg_frame_rate > 10.0) {
        fprintf(f, "- ⚠️ **HIGH FRAME RATE** - %.1f fps (automated scanning likely)\n", 
                stats->avg_frame_rate);
    }

    // Function code summary
    fprintf(f, "\n### Function Code Summary\n\n");
    fprintf(f, "**Unique functions tested:** %u\n\n", stats->unique_functions_seen);
    
    fprintf(f, "| Function Code | Function Name | Count |\n");
    fprintf(f, "|---------------|---------------|-------|\n");
    
    for (int i = 0; i < 256; i++) {
        if (function_counts[i] > 0) {
            fprintf(f, "| 0x%02X | %s | %u |\n", 
                    i, modbus_get_function_name(i), function_counts[i]);
        }
    }
    
    // Function coverage
    fprintf(f, "\n### Function Code Coverage\n\n");
    fprintf(f, "Codes observed: ");
    
    uint32_t count = 0;
    for (int i = 0; i < 256; i++) {
        if (stats->function_codes_seen[i]) {
            if (count > 0) fprintf(f, ", ");
            fprintf(f, "`0x%02X`", i);
            count++;
        }
    }
    fprintf(f, "\n");
}


/**
 * modbus_write_report_summary() - Close report file and clean up
 * @stats: Statistics structure with open report file
 *
 * Flushes and closes report file handle.
 * Sets report_file to NULL.
 * No-op if report not enabled.
 *
 * Call after modbus_write_report_summary() to finalize report.
 */

void modbus_close_report(attack_stats_t *stats) {
    if (!stats->report_enabled || !stats->report_file) return;
    
    fprintf(stats->report_file, "\n---\n\n");
    fprintf(stats->report_file, "*Report generated by Modbus TCP Parser*\n");
    
    fclose(stats->report_file);
    stats->report_file = NULL;
    stats->report_enabled = false;
}



