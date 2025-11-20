#ifndef MODBUS_PARSER_H
#define MODBUS_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Display modes
typedef enum {
    DISPLAY_TABLE,      // Compact table format
    DISPLAY_VERBOSE     // Detailed breakdown
} display_mode_t;

// MBAP Header structure (7 bytes)
typedef struct {
    uint16_t transaction_id;
    uint16_t protocol_id;
    uint16_t length;
    uint8_t unit_id;
} modbus_mbap_header_t;

// Modbus TCP Frame structure
typedef struct {
    modbus_mbap_header_t mbap;
    uint8_t function_code;
    uint8_t *data;
    uint16_t data_length;
} modbus_tcp_frame_t;

// Modbus exception codes
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

// Attack detection statistics
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



// Function code names
const char* modbus_get_function_name(uint8_t function_code);

// Parse Modbus TCP frame from raw bytes
bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len, modbus_tcp_frame_t *frame);

// Display parsed frame
void modbus_display_frame(const modbus_tcp_frame_t *frame);

// Free frame resources
void modbus_free_frame(modbus_tcp_frame_t *frame);

void modbus_display_frame_table(const modbus_tcp_frame_t *frame,
                                const char *src_ip, uint16_t src_port,
                                const char *dst_ip, uint16_t dst_port,
                                uint32_t packet_number,
                                double timestamp,
                                bool is_first);

// Get exception code name
const char* modbus_get_exception_name(uint8_t exception_code);

// Update attack statistics
void modbus_update_attack_stats(attack_stats_t *stats, const modbus_tcp_frame_t *frame, double timestamp);

// Display attack detection summary
void modbus_display_attack_summary(const attack_stats_t *stats);

// Report generation functions
bool modbus_open_report(attack_stats_t *stats, const char *pcap_filename);
void modbus_write_report_header(attack_stats_t *stats, const char *pcap_filename);
void modbus_write_report_frame(attack_stats_t *stats, const modbus_tcp_frame_t *frame,
                                const char *src_ip, uint16_t src_port,
                                const char *dst_ip, uint16_t dst_port,
                                uint32_t packet_number,
                                double timestamp);
void modbus_write_report_summary(attack_stats_t *stats, const uint32_t *function_counts);
void modbus_close_report(attack_stats_t *stats);

#endif // MODBUS_PARSER_H
