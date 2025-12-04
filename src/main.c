/*
 * main.c - Modbus TCP Parser - Main entry point and orchestration
 *
 * Command-line application for parsing Modbus TCP traffic from PCAP files.
 * Provides frame-by-frame analysis, security threat detection, and
 * comprehensive markdown reporting.
 *
 * Features:
 * - Dual display modes (table/verbose)
 * - Security analysis (scanning, timing, exceptions)
 * - Markdown report generation
 * - Color-coded terminal output
 *
 * Usage: modbus-parser [options] <pcap_file>
 *
 * Copyright (C) 2025 Marty
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pcap_reader.h"
#include "modbus_parser.h"
#include "colors.h"


/**
 * struct process_context - Processing context passed to frame callback
 * @mode: Display format (table or verbose)
 * @frame_count: Total frames successfully parsed
 * @functon_counts: Per-function-code usage counters
 * @attack_stats: Security analysis accumulator
 *
 * Aggregates state across all processed frames including display mode,
 * frame counters, function code statistics, and security analysis.
 */

typedef struct {
    display_mode_t mode;
    uint32_t frame_count;
    uint32_t function_counts[256]; // Count occurences of each function code.
    attack_stats_t attack_stats; // Attack detection statistics.
} process_context_t;



/**
 * process_modbus_payload() - Callback function to process each Modbus TCP 
 * payload
 * 
 * @payload: Raw Modbus TCP frame data (MBAP + PDU)
 * @length: Payload length in bytes
 * @src_ip: Source IP address
 * @src_port: Source TCP port
 * @dst_ip: Destination IP address
 * @dst_port: Destination TCP port
 * @timestamp: Frame timestamp (seconds since epoch)
 * @user_data: Pointer to process_context_t structure
 *
 * Invoked by pcap_reader for each Modbus TCP frame found in PCAP.
 * 
 * Processing flow:
 * 1. Parse frame via modbus_parse_frame()
 * 2. Display frame (table or verbose mode)
 * 3. Write to report file (if enabled)
 * 4. Update statistics (frame count, function codes, security)
 * 5. Free frame resources
 *
 * Parse failures are silently skipped (verbose mode shows error).
 */

void process_modbus_payload(const uint8_t *payload, uint32_t length,
                            const char *src_ip, uint16_t src_port,
                            const char *dst_ip, uint16_t dst_port,
                            double timestamp,
                            void *user_data) {
    process_context_t *ctx = (process_context_t*)user_data;
    modbus_tcp_frame_t frame;
    
    // Parse the Modbus TCP frame
    if (modbus_parse_frame(payload, length, &frame)) {
        if (ctx->mode == DISPLAY_VERBOSE) {
            // Verbose mode: full detailed breakdown
            printf("\n--- Frame %u ---\n", ctx->frame_count + 1);
            printf("Connection: %s:%u -> %s:%u\n", src_ip, src_port, dst_ip, dst_port);
            modbus_display_frame(&frame);
        } else {
            // Table mode: compact single-line display
            bool is_first = (ctx->frame_count == 0);
            modbus_display_frame_table(&frame, src_ip, src_port, dst_ip, dst_port, ctx->frame_count + 1, timestamp, is_first);
        }
        
        // Write to report if enabled
        modbus_write_report_frame(&ctx->attack_stats, &frame, src_ip, src_port, 
                                  dst_ip, dst_port, ctx->frame_count + 1, timestamp);

        ctx->frame_count++;
        // Track function code usage
        ctx->function_counts[frame.function_code]++;
        // Update attack detection statistics
        modbus_update_attack_stats(&ctx->attack_stats, &frame, timestamp);
        modbus_free_frame(&frame);
    } else {
        if (ctx->mode == DISPLAY_VERBOSE) {
            printf("Failed to parse Modbus frame\n\n");
        }
    }
}


/**
 * print_usage() - Display command-line usage information
 * @program_name: Name of the executable (argv[0])
 *
 * Outputs help text including:
 * - Usage syntax
 * - Available options (-v, -r, -h)
 * - Usage examples
 *
 * Called when -h/--help specified or on argument errors.
 */

void print_usage(const char *program_name) {
    printf("Usage: %s [options] <pcap_file>\n", program_name);
    printf("\nOptions:\n");
    printf("  -v, --verbose    Display detailed breakdown of each frame\n");
    printf("  -r, --report     Generate markdown analysis report\n");
    printf("  -h, --help       Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s capture.pcap              # Table format (default)\n", program_name);
    printf("  %s -v capture.pcap           # Verbose format\n", program_name);
    printf("  %s -r capture.pcap           # Generate report\n", program_name);
    printf("  %s -v -r capture.pcap        # Verbose + report\n", program_name);
}


/**
 * print_color_legend() - Display color coding reference
 *
 * Outputs legend explaining ANSI color usage in table mode:
 * - Cyan: IP addresses
 * - Yellow: Transaction IDs
 * - Green: Unit IDs
 * - Magenta: Function codes
 * - Blue: Data fields
 *
 * Displayed once at start in table mode to help users interpret output.
 */

void print_color_legend() {
    printf("\n%sColor Legend:%s\n", COLOR_WHITE, COLOR_RESET);
    printf("  %sCyan: IP Addresses%s | ", COLOR_CYAN, COLOR_RESET);
    printf("%sYellow: Trans ID%s | ", COLOR_YELLOW, COLOR_RESET);
    printf("%sGreen: Unit ID%s | ", COLOR_GREEN, COLOR_RESET);
    printf("%sMagenta: Function%s | ", COLOR_MAGENTA, COLOR_RESET);
    printf("%sBlue: Data%s\n", COLOR_BLUE, COLOR_RESET);
}


/**
 * main() - Applicaiton entry point
 * @argc: Argument count
 * @argv: Argument vector
 *
 * Command-line parser and PCAP processing orchestrator.
 *
 * Flow:
 * 1. Parse command-line arguments (-v, -r, filename)
 * 2. Initialize processing context
 * 3. Open report file (if -r specified)
 * 4. Process PCAP via pcap_process_file()
 * 5. Display function code summary
 * 6. Display security analysis
 * 7. Finalize and close report
 *
 * Return: 0 on success, 1 on error
 */

int main(int argc, char *argv[]) {
    display_mode_t mode = DISPLAY_TABLE;  // Default to table format
    bool generate_report = false;
    const char *filename = NULL;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            mode = DISPLAY_VERBOSE;
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--report") == 0) {
            generate_report = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            printf("Unknown option: %s\n\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else {
            filename = argv[i];
        }
    }

    if (filename == NULL) {
        printf("Error: No PCAP file specified\n\n");
        print_usage(argv[0]);
        return 1;
    }

    printf("Modbus TCP Parser\n");
    printf("=================\n\n");
    printf("Processing PCAP file: %s\n", filename);
    printf("Mode: %s\n", mode == DISPLAY_VERBOSE ? "Verbose" : "Table");

    if (mode == DISPLAY_TABLE) {
        print_color_legend();
    }

    // Create processing context
    process_context_t ctx = {
        .mode = mode,
        .frame_count = 0,
        .function_counts = {0},
        .attack_stats = {0} // Initialise all counts to zero}
    };
    
        // Open report file if requested
        if (generate_report) {
        if (!modbus_open_report(&ctx.attack_stats, filename)) {
            printf("Warning: Report generation disabled due to file error\n");
            generate_report = false;
        } else {
            modbus_write_report_header(&ctx.attack_stats, filename);
        }
    }


    // Process PCAP file
    if (!pcap_process_file(filename, process_modbus_payload, &ctx)) {
        printf("Failed to process PCAP file\n");
        return 1;
    }

    printf("\n%sTotal Modbus frames processed: %u%s\n", COLOR_WHITE, ctx.frame_count, COLOR_RESET);

    // Display function code summary
    printf("\n%sFunction Code Summary:%s\n", COLOR_WHITE, COLOR_RESET);
    printf("%s%-40s %s%s\n", COLOR_WHITE, "Function", "Count", COLOR_RESET);
    printf("%s--------------------------------------------------------%s\n", COLOR_GRAY, COLOR_RESET);
    
    for (int i = 0; i < 256; i++) {
        if (ctx.function_counts[i] > 0) {
            printf("%s%-40s %s%u%s\n",
            COLOR_MAGENTA, modbus_get_function_name(i),
            COLOR_CYAN, ctx.function_counts[i], COLOR_RESET);
        }
    }

    // Display attack detection summary
    modbus_display_attack_summary(&ctx.attack_stats);

        // Finalize report if enabled
    if (generate_report) {
        modbus_write_report_summary(&ctx.attack_stats, ctx.function_counts);
        modbus_close_report(&ctx.attack_stats);
        printf("\nReport generation complete.\n");
    }
    
    return 0;
}
