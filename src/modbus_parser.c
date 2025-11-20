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

// Minimum Modbus TCP frame size: MBAP (7) + Function Code (1)
#define MIN_MODBUS_FRAME_SIZE 8

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


bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len, modbus_tcp_frame_t *frame) {
    // Validate minimum size
    if (payload_len < MIN_MODBUS_FRAME_SIZE) {
       // printf("Error: Payload too short (%u bytes, minimum %d)\n", 
       //        payload_len, MIN_MODBUS_FRAME_SIZE);
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

void modbus_free_frame(modbus_tcp_frame_t *frame) {
    if (frame->data != NULL) {
        free(frame->data);
        frame->data = NULL;
    }
}


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


bool modbus_open_report(attack_stats_t *stats, const char *pcap_filename) {
    printf("DEBUG: modbus_open_report() called with: %s\n", pcap_filename);

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
    printf("DEBUG: Attempting to create: %s\n", report_filename);
    stats->report_file = fopen(report_filename, "w");
    if (!stats->report_file) {
        printf("Error: Could not create report file: %s\n", report_filename);
        return false;
    }
    
    printf("Writing report to: %s\n", report_filename);
    stats->report_enabled = true;
    return true;
}

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

void modbus_close_report(attack_stats_t *stats) {
    if (!stats->report_enabled || !stats->report_file) return;
    
    fprintf(stats->report_file, "\n---\n\n");
    fprintf(stats->report_file, "*Report generated by Modbus TCP Parser*\n");
    
    fclose(stats->report_file);
    stats->report_file = NULL;
    stats->report_enabled = false;
}



