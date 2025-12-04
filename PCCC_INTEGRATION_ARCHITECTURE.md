# PCCC Protocol Integration Architecture

**Document Version:** 1.0  
**Target Project:** Modbus Parser (v1.0-stable)  
**Integration Phase:** Add PCCC as second protocol  
**Date:** December 2, 2025

---

## Executive Summary

This document defines how to integrate PCCC (Programmable Controller Communication Command) protocol parsing into the existing Modbus Parser codebase, following its established architectural patterns while preparing for the v2.0 plugin system migration.

### Integration Approach

**Strategy:** **Parallel Module Pattern**
- Create `pccc_parser.c/h` alongside `modbus_parser.c/h`
- Reuse `pcap_reader` infrastructure
- Share common patterns (callback-based, statistics tracking, report generation)
- Minimal changes to existing Modbus code
- Prepares for v2.0 protocol plugin architecture

---

## Table of Contents

1. [Architecture Alignment](#architecture-alignment)
2. [Module Design: pccc_parser](#module-design-pccc_parser)
3. [Data Structures](#data-structures)
4. [API Design](#api-design)
5. [Integration Points](#integration-points)
6. [Implementation Phases](#implementation-phases)
7. [Testing Strategy](#testing-strategy)
8. [Migration Path to v2.0](#migration-path-to-v20)

---

## Architecture Alignment

### Matching Existing Patterns

Your Modbus Parser follows these patterns that PCCC integration must preserve:

| Pattern | Modbus Implementation | PCCC Implementation |
|---------|----------------------|---------------------|
| **Entry Point** | `main.c` orchestrates | Same `main.c` extended with protocol selection |
| **Callback-Based** | `pcap_process_file()` → callback | Same callback, different payload handler |
| **Stateless Parsing** | `modbus_parse_frame()` | `pccc_parse_frame()` (same pattern) |
| **Statistics Tracking** | `attack_stats_t` | `pccc_analysis_stats_t` (parallel structure) |
| **Display Modes** | Table + Verbose | Table + Verbose (same flags) |
| **Report Generation** | Markdown file output | Markdown file output (same format) |
| **Memory Management** | Caller frees via `modbus_free_frame()` | Caller frees via `pccc_free_frame()` |

### Current System Flow (Modbus)

```
User Command
    │
    ▼
main.c: parse_arguments() → mode: MODBUS (default)
    │
    ▼
pcap_reader: pcap_process_file(port=502)
    │
    └─► For each packet:
         └─► process_modbus_payload()
              └─► modbus_parse_frame()
              └─► modbus_display_frame() or modbus_display_frame_table()
              └─► modbus_update_attack_stats()
    │
    ▼
modbus_finalize_attack_stats()
```

### Extended System Flow (Modbus + PCCC)

```
User Command
    │
    ▼
main.c: parse_arguments() → mode: MODBUS or PCCC (-p flag)
    │
    ├─► IF MODBUS:
    │    └─► pcap_reader: pcap_process_file(port=502)
    │         └─► process_modbus_payload() [unchanged]
    │
    └─► IF PCCC:
         └─► pcap_reader: pcap_process_file(port=44818)
              └─► process_pccc_payload() [NEW]
                   └─► pccc_parse_frame()
                   └─► pccc_display_frame() or pccc_display_frame_table()
                   └─► pccc_update_analysis_stats()
         │
         ▼
         pccc_finalize_analysis_stats()
```

---

## Module Design: pccc_parser

### File Structure

```
src/
├── main.c                    [MODIFIED - add protocol selection]
├── pcap_reader.c/h           [UNCHANGED]
├── modbus_parser.c/h         [UNCHANGED]
├── pccc_parser.c/h           [NEW]
├── colors.h                  [UNCHANGED]
└── CMakeLists.txt            [MODIFIED - add pccc_parser.c]
```

### pccc_parser.h (Public API)

```c
/**
 * pccc_parser.h - PCCC Protocol Parser
 * 
 * Parses PCCC (Programmable Controller Communication Command) frames
 * encapsulated within EtherNet/IP and CIP layers for legacy Rockwell
 * PLCs (MicroLogix, SLC 5/0x series).
 *
 * Follows Modbus Parser architectural patterns for consistency.
 */

#ifndef PCCC_PARSER_H
#define PCCC_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ============================================================================
// Data Structures
// ============================================================================

/**
 * struct eip_encap_header - EtherNet/IP Encapsulation Header
 * @command: EIP command code (0x006F = SendRRData for PCCC)
 * @length: Length of encapsulated data
 * @session_handle: Session identifier
 * @status: Status code
 * @sender_context: Sender context (8 bytes)
 * @options: Options flags
 */
typedef struct {
    uint16_t command;
    uint16_t length;
    uint32_t session_handle;
    uint32_t status;
    uint8_t sender_context[8];
    uint32_t options;
} eip_encap_header_t;

/**
 * struct cip_service_header - CIP Service Layer
 * @service: Service code (0x4B = Execute PCCC)
 * @req_path_size: Request path size in 16-bit words
 */
typedef struct {
    uint8_t service;
    uint8_t req_path_size;
} cip_service_header_t;

/**
 * struct pccc_requestor_id - PCCC Requestor Identification
 * @length: Length of this structure (typically 0x07)
 * @vendor_id: CIP Vendor ID (little-endian)
 * @serial_num: CIP Serial Number (little-endian)
 */
typedef struct {
    uint8_t length;
    uint16_t vendor_id;
    uint32_t serial_num;
} pccc_requestor_id_t;

/**
 * struct pccc_command - PCCC Command Structure
 * @cmd_code: Command code (0x0F = Protected Typed Logical Read/Write)
 * @status: Status code (0x00 in requests, various in responses)
 * @tns: Transaction Number (little-endian)
 * @function_code: Function code (0xA1, 0xA2, etc.)
 * @data: Function-specific data
 * @data_length: Length of data field
 */
typedef struct {
    uint8_t cmd_code;
    uint8_t status;
    uint16_t tns;           // Little-endian
    uint8_t function_code;
    uint8_t *data;          // Dynamically allocated
    uint16_t data_length;
} pccc_command_t;

/**
 * struct pccc_function_0xA2_data - Function 0xA2 Addressing
 * @byte_size: Number of bytes to read/write
 * @file_number: File number (e.g., 9 for N9)
 * @file_type: File type (0x89=Integer, 0x8A=Float)
 * @element_number: Element within file
 * @subelement_number: Sub-element (usually 0)
 */
typedef struct {
    uint8_t byte_size;
    uint8_t file_number;
    uint8_t file_type;
    uint8_t element_number;
    uint8_t subelement_number;
} pccc_function_0xA2_data_t;

/**
 * struct pccc_frame - Complete PCCC Frame
 * @eip: EtherNet/IP encapsulation header
 * @cip: CIP service header
 * @requestor_id: Requestor identification
 * @command: PCCC command structure
 * @is_request: True if request, false if response
 * @is_valid: True if frame parsed successfully
 */
typedef struct {
    eip_encap_header_t eip;
    cip_service_header_t cip;
    pccc_requestor_id_t requestor_id;
    pccc_command_t command;
    bool is_request;
    bool is_valid;
} pccc_frame_t;

/**
 * struct pccc_process_variable - Extracted Process Variable
 * @address: PV address string (e.g., "N9:1", "F8:0")
 * @file_type_char: Character representation ('N', 'F', etc.)
 * @file_number: File number
 * @element_number: Element number
 * @data_type: Data type name ("Integer", "Float", etc.)
 * @has_value: True if response contains value
 * @int_value: Integer value (if file_type = 0x89)
 * @float_value: Float value (if file_type = 0x8A)
 */
typedef struct {
    char address[32];
    char file_type_char;
    uint8_t file_number;
    uint8_t element_number;
    char data_type[16];
    bool has_value;
    union {
        int16_t int_value;
        float float_value;
    } value;
} pccc_process_variable_t;

/**
 * struct pccc_analysis_stats - PCCC Traffic Analysis Statistics
 * (Parallel structure to modbus attack_stats_t)
 */
typedef struct {
    // Frame counts
    uint32_t total_frames;
    uint32_t request_count;
    uint32_t response_count;
    uint32_t error_count;
    uint32_t parse_failures;
    
    // Process Variables
    uint32_t unique_pvs_seen;
    char observed_pvs[256][32];  // Track PV addresses
    uint32_t pv_read_count;
    uint32_t pv_write_count;
    
    // Command/Function tracking
    bool commands_seen[256];
    bool functions_seen[256];
    uint32_t unique_commands;
    uint32_t unique_functions;
    
    // Timing
    double first_packet_time;
    double last_packet_time;
    double total_duration;
    double avg_frame_rate;
    uint32_t rapid_burst_count;
    
    // Security indicators
    float error_rate;
    bool sequential_scanning_detected;
    
    // Reporting
    FILE *report_file;
    bool report_enabled;
    char input_filename[256];
} pccc_analysis_stats_t;

// ============================================================================
// Core Parsing Functions
// ============================================================================

/**
 * pccc_parse_frame() - Parse PCCC frame from raw payload
 * @payload: Raw packet payload (EIP encapsulation onward)
 * @payload_len: Length of payload
 * @frame: Output frame structure
 *
 * Parses layered structure: EIP → CIP → PCCC
 * Validates headers at each layer.
 * Allocates memory for frame->command.data.
 *
 * Return: true on success, false on parse error
 *
 * NOTE: Caller must call pccc_free_frame() to free allocated memory
 */
bool pccc_parse_frame(const uint8_t *payload, uint32_t payload_len,
                      pccc_frame_t *frame);

/**
 * pccc_free_frame() - Free dynamically allocated frame data
 * @frame: Frame to free
 *
 * Frees frame->command.data if allocated.
 * Safe to call multiple times.
 * Safe to call on failed parse (does nothing).
 */
void pccc_free_frame(pccc_frame_t *frame);

/**
 * pccc_extract_process_variable() - Extract PV from PCCC frame
 * @frame: Parsed PCCC frame
 * @pv: Output process variable structure
 *
 * Extracts process variable addressing information from Function 0xA2.
 * Constructs address string (e.g., "N9:1").
 * For responses, extracts data value.
 *
 * Return: true if PV extracted, false if not applicable
 */
bool pccc_extract_process_variable(const pccc_frame_t *frame,
                                    pccc_process_variable_t *pv);

// ============================================================================
// Display Functions
// ============================================================================

/**
 * pccc_display_frame() - Display frame in verbose mode
 * @frame: Frame to display
 * @src_ip: Source IP address
 * @src_port: Source port
 * @dst_ip: Destination IP address
 * @dst_port: Destination port
 * @packet_num: Packet sequence number
 * @timestamp: Packet timestamp
 *
 * Displays complete frame breakdown:
 * - Connection information
 * - EIP header fields
 * - CIP service information
 * - PCCC command details
 * - Process variable (if applicable)
 * - Data values (for responses)
 */
void pccc_display_frame(const pccc_frame_t *frame,
                        const char *src_ip, uint16_t src_port,
                        const char *dst_ip, uint16_t dst_port,
                        uint32_t packet_num, double timestamp);

/**
 * pccc_display_frame_table() - Display frame in compact table format
 * @frame: Frame to display
 * @src_ip: Source IP address
 * @src_port: Source port
 * @dst_ip: Destination IP address
 * @dst_port: Destination port
 * @packet_num: Packet sequence number
 * @timestamp: Packet timestamp
 * @print_header: True to print table header
 *
 * Single-line table format:
 * Packet | Timestamp | Source | Dest | TNS | Cmd | Func | PV | Value
 */
void pccc_display_frame_table(const pccc_frame_t *frame,
                               const char *src_ip, uint16_t src_port,
                               const char *dst_ip, uint16_t dst_port,
                               uint32_t packet_num, double timestamp,
                               bool print_header);

// ============================================================================
// Analysis Functions
// ============================================================================

/**
 * pccc_init_analysis_stats() - Initialize analysis statistics
 * @stats: Statistics structure to initialize
 * @enable_report: True to enable markdown report generation
 * @input_filename: PCAP filename (for report naming)
 *
 * Initializes all counters to zero.
 * Opens report file if enabled.
 * Writes report header.
 */
void pccc_init_analysis_stats(pccc_analysis_stats_t *stats,
                              bool enable_report,
                              const char *input_filename);

/**
 * pccc_update_analysis_stats() - Update statistics for a frame
 * @stats: Statistics structure
 * @frame: Parsed frame
 * @timestamp: Frame timestamp
 *
 * Updates per-frame statistics:
 * - Frame counts (requests/responses/errors)
 * - Process variable tracking
 * - Command/function usage
 * - Timing information
 * - Security indicators
 */
void pccc_update_analysis_stats(pccc_analysis_stats_t *stats,
                                const pccc_frame_t *frame,
                                double timestamp);

/**
 * pccc_finalize_analysis_stats() - Compute final statistics
 * @stats: Statistics structure
 *
 * Calculates final metrics:
 * - Error rates
 * - Frame rates
 * - Scanning detection
 * Writes report summary if enabled.
 * Closes report file.
 */
void pccc_finalize_analysis_stats(pccc_analysis_stats_t *stats);

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * pccc_get_command_name() - Get human-readable command name
 * @cmd_code: PCCC command code
 *
 * Return: Command name string (e.g., "Protected Typed Logical Read")
 */
const char* pccc_get_command_name(uint8_t cmd_code);

/**
 * pccc_get_function_name() - Get human-readable function name
 * @function_code: PCCC function code
 *
 * Return: Function name string (e.g., "Read with three address fields")
 */
const char* pccc_get_function_name(uint8_t function_code);

/**
 * pccc_get_file_type_char() - Get file type character
 * @file_type: File type byte (0x89, 0x8A, etc.)
 *
 * Return: Character representation ('N', 'F', etc.) or '?' if unknown
 */
char pccc_get_file_type_char(uint8_t file_type);

/**
 * pccc_get_file_type_name() - Get file type name
 * @file_type: File type byte
 *
 * Return: Type name string (e.g., "Integer", "Float")
 */
const char* pccc_get_file_type_name(uint8_t file_type);

#endif /* PCCC_PARSER_H */
```

---

## Data Structures

### Alignment with Modbus Structures

| Modbus Structure | PCCC Equivalent | Purpose |
|------------------|-----------------|---------|
| `modbus_mbap_header_t` | `eip_encap_header_t` | Outer protocol header |
| `modbus_tcp_frame_t` | `pccc_frame_t` | Complete frame |
| `attack_stats_t` | `pccc_analysis_stats_t` | Traffic analysis |
| N/A | `pccc_process_variable_t` | Extracted PV data |

### Memory Management Pattern

**Same as Modbus:**
```c
// Parse allocates memory
pccc_frame_t frame;
if (pccc_parse_frame(payload, length, &frame)) {
    // Use frame
    // ...
    // Free allocated memory
    pccc_free_frame(&frame);  // REQUIRED
}
```

---

## API Design

### Function Naming Convention

Following your existing pattern:

```c
// Module prefix: pccc_
// Pattern: pccc_verb_noun()

bool pccc_parse_frame(...)      // Parse frame
void pccc_free_frame(...)       // Free resources
void pccc_display_frame(...)    // Verbose display
void pccc_display_frame_table(...)  // Table display
void pccc_init_analysis_stats(...)  // Initialize stats
void pccc_update_analysis_stats(...) // Update stats
void pccc_finalize_analysis_stats(...) // Finalize stats
const char* pccc_get_command_name(...)  // Utility
```

Matches `modbus_` prefix pattern exactly.

---

## Integration Points

### 1. main.c Modifications

```c
// Add protocol selection
typedef enum {
    PROTOCOL_MODBUS,
    PROTOCOL_PCCC
} protocol_t;

typedef struct {
    protocol_t protocol;        // NEW
    display_mode_t mode;
    bool generate_report;
    const char *pcap_file;
} options_t;

// Extend CLI parsing
void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] <pcap_file>\n", program_name);
    printf("Options:\n");
    printf("  -v          Verbose output\n");
    printf("  -r          Generate markdown report\n");
    printf("  -p PROTOCOL Protocol to parse (modbus|pccc) [default: modbus]\n");  // NEW
    printf("  -h          Show this help\n");
}

// Protocol-specific callbacks
void process_modbus_payload(...) {
    // Existing Modbus handling [UNCHANGED]
}

void process_pccc_payload(                    // NEW
    const uint8_t *payload, uint32_t length,
    const char *src_ip, uint16_t src_port,
    const char *dst_ip, uint16_t dst_port,
    double timestamp, void *user_data) {
    
    process_context_pccc_t *ctx = (process_context_pccc_t*)user_data;
    pccc_frame_t frame;
    
    if (!pccc_parse_frame(payload, length, &frame)) {
        ctx->stats.parse_failures++;
        return;
    }
    
    // Display
    if (ctx->mode == DISPLAY_VERBOSE) {
        pccc_display_frame(&frame, src_ip, src_port, dst_ip, dst_port,
                          ctx->frame_count, timestamp);
    } else {
        bool print_hdr = (ctx->frame_count == 0);
        pccc_display_frame_table(&frame, src_ip, src_port, dst_ip, dst_port,
                                ctx->frame_count, timestamp, print_hdr);
    }
    
    // Update statistics
    pccc_update_analysis_stats(&ctx->stats, &frame, timestamp);
    ctx->frame_count++;
    
    pccc_free_frame(&frame);
}

// Main function dispatch
int main(int argc, char *argv[]) {
    options_t opts = {0};
    opts.protocol = PROTOCOL_MODBUS;  // Default
    
    // Parse arguments (add -p option)
    // ...
    
    if (opts.protocol == PROTOCOL_MODBUS) {
        // Existing Modbus path [UNCHANGED]
        process_context_t ctx = {0};
        // ...
        pcap_process_file(opts.pcap_file, process_modbus_payload, &ctx);
        // ...
    } else if (opts.protocol == PROTOCOL_PCCC) {
        // New PCCC path
        process_context_pccc_t ctx = {0};
        ctx.mode = opts.mode;
        pccc_init_analysis_stats(&ctx.stats, opts.generate_report, opts.pcap_file);
        
        pcap_process_file(opts.pcap_file, process_pccc_payload, &ctx);
        
        pccc_finalize_analysis_stats(&ctx.stats);
        printf("\nProcessed %u PCCC frames\n", ctx.frame_count);
    }
    
    return 0;
}
```

### 2. pcap_reader.c Extension

**Option A: Port Parameter (Recommended)**

```c
// Extend pcap_process_file signature (BREAKING CHANGE for v2.0)
bool pcap_process_file(const char *filename,
                       uint16_t target_port,     // NEW: 502 or 44818
                       pcap_callback_t callback,
                       void *user_data);

// Usage:
pcap_process_file("capture.pcap", 502, process_modbus_payload, &ctx);     // Modbus
pcap_process_file("capture.pcap", 44818, process_pccc_payload, &ctx);     // PCCC
```

**Option B: Backward Compatible**

Keep existing function, add new variant:

```c
// Existing (unchanged)
bool pcap_process_file(const char *filename,
                       pcap_callback_t callback,
                       void *user_data);  // Defaults to port 502

// New function
bool pcap_process_file_port(const char *filename,
                            uint16_t target_port,
                            pcap_callback_t callback,
                            void *user_data);
```

**Recommendation:** Option B for v1.x, migrate to Option A in v2.0

### 3. CMakeLists.txt Update

```cmake
add_executable(modbus-parser
    src/main.c
    src/pcap_reader.c
    src/modbus_parser.c
    src/pccc_parser.c        # ADD THIS LINE
)
```

---

## Implementation Phases

### Phase 1: Core Parsing (Week 1)

**Deliverables:**
- [x] `pccc_parser.h` - Complete API definition
- [ ] `pccc_parser.c` - Core parsing functions:
  - `pccc_parse_frame()`
  - `pccc_free_frame()`
  - `pccc_extract_process_variable()`
- [ ] Unit test with sample PCAP
- [ ] Validate against Wireshark dissector

**Success Criteria:**
- Parse EtherNet/IP encapsulation header
- Parse CIP service layer
- Parse PCCC command structure
- Extract Function 0xA2 addressing
- Match Wireshark output byte-for-byte

### Phase 2: Display & Analysis (Week 1)

**Deliverables:**
- [ ] Display functions:
  - `pccc_display_frame()` (verbose)
  - `pccc_display_frame_table()` (table)
- [ ] Statistics functions:
  - `pccc_init_analysis_stats()`
  - `pccc_update_analysis_stats()`
  - `pccc_finalize_analysis_stats()`
- [ ] Utility functions for name lookups

**Success Criteria:**
- Verbose output shows all frame details
- Table format matches Modbus style
- Statistics track PVs, commands, timing

### Phase 3: Integration (Week 2)

**Deliverables:**
- [ ] Modify `main.c` for protocol selection
- [ ] Add `-p` CLI option
- [ ] Extend `pcap_reader` for port 44818
- [ ] Update `CMakeLists.txt`
- [ ] Test end-to-end with PCCC.pcap

**Success Criteria:**
- `./modbus-parser -p pccc -v PCCC.pcap` works
- `./modbus-parser -p pccc -r PCCC.pcap` generates report
- Existing Modbus functionality unchanged

### Phase 4: Documentation & Testing (Week 2)

**Deliverables:**
- [ ] Update README.md with PCCC support
- [ ] Update ARCHITECTURE.md with PCCC module
- [ ] Update API.md with PCCC API
- [ ] Create test PCAP suite
- [ ] Performance testing

---

## Testing Strategy

### Unit Testing (Manual - v1.x)

**Test Cases:**

1. **Valid PCCC Frame Parsing**
   - Input: Known-good PCCC frame bytes
   - Expected: Successful parse, correct field values

2. **Malformed Frame Handling**
   - Wrong EIP command → Parse failure
   - Truncated frame → Parse failure
   - Invalid CIP service → Parse failure

3. **Process Variable Extraction**
   - Function 0xA2 request → Extract "N9:1"
   - Function 0xA2 response → Extract "N9:1" + value

4. **Memory Management**
   - Parse + Free → No leaks (valgrind)
   - Multiple frames → Correct allocation/free

### Integration Testing

**Test PCAPs:**
- `PCCC.pcap` - Real traffic (from project)
- Create synthetic PCAPs:
  - Single PV read
  - Multiple PV reads
  - Error responses
  - Mixed requests/responses

**Test Commands:**
```bash
# Table format
./modbus-parser -p pccc PCCC.pcap

# Verbose format
./modbus-parser -p pccc -v PCCC.pcap

# Report generation
./modbus-parser -p pccc -r PCCC.pcap
cat PCCC_analysis.md

# Still works for Modbus
./modbus-parser modbus_capture.pcap
```

### Comparison Testing

**Wireshark Validation:**
```bash
# For each frame in PCCC.pcap:
# 1. Parser output
# 2. Wireshark display
# 3. Compare field-by-field
```

---

## Migration Path to v2.0

### Current (v1.x): Parallel Modules

```
modbus-parser/
├── modbus_parser.c/h    (Protocol 1)
└── pccc_parser.c/h      (Protocol 2)
     └── Shared patterns but independent
```

### Future (v2.0): Plugin Architecture

```
libprotocolparse/
├── framework/
│   ├── plugin_interface.h
│   ├── logging.c/h
│   └── analysis_framework.c/h
└── plugins/
    ├── modbus/
    │   └── modbus_plugin.c   (Refactored from modbus_parser.c)
    └── pccc/
        └── pccc_plugin.c     (Refactored from pccc_parser.c)
```

### Refactoring Strategy

**What stays the same:**
- Core parsing logic (frame structures, algorithms)
- Display formatting
- Statistics tracking

**What changes:**
- Wrapper: Plugin interface implementation
- Registration: Dynamic plugin loading
- Logging: Kernel-style framework
- Configuration: Per-plugin config files

**Benefit of parallel modules now:**
- Proves multi-protocol pattern works
- Identifies common abstractions
- Code can be refactored (not rewritten) for v2.0

---

## PCCC Protocol Layer Details

### Complete Packet Structure

```
┌─────────────────────────────────────────────────┐
│ Ethernet Frame                                   │
├─────────────────────────────────────────────────┤
│ IP Header                                        │
├─────────────────────────────────────────────────┤
│ TCP Header (port 44818)                          │
├─────────────────────────────────────────────────┤
│ EtherNet/IP Encapsulation Header (24 bytes)     │
│   Command: 0x006F (SendRRData)                   │
│   Length: Following bytes                        │
│   Session Handle: 0xXXXXXXXX                     │
│   Status: 0x00000000                             │
│   Sender Context: 8 bytes                        │
│   Options: 0x00000000                            │
├─────────────────────────────────────────────────┤
│ CIP Service Header (2+ bytes)                    │
│   Service: 0x4B (Execute PCCC)                   │
│   Request Path Size: N words                     │
│   Request Path: (N*2 bytes)                      │
├─────────────────────────────────────────────────┤
│ PCCC Requestor ID (7 bytes)                      │
│   Length: 0x07                                   │
│   Vendor ID: 0xXXXX (LE)                         │
│   Serial Num: 0xXXXXXXXX (LE)                    │
├─────────────────────────────────────────────────┤
│ PCCC Command (REQUEST)                           │
│   +0: Cmd Code: 0x0F                             │
│   +1: Status: 0x00                               │
│   +2: TNS: 0xXXXX (LE)                           │
│   +4: Function: 0xA2                             │
│   +5: Byte Size: 0x02                            │
│   +6: File Num: 0x09 (N9)                        │
│   +7: File Type: 0x89 (Integer)                  │
│   +8: Element: 0x01                              │
│   +9: Sub-element: 0x00                          │
└─────────────────────────────────────────────────┘

Response structure similar, but:
│   +4: Data payload (if status == 0x00)           │
```

### Byte Offsets Reference

**EtherNet/IP Encapsulation (24 bytes):**
- Offset 0-1: Command (big-endian uint16) - Must be 0x006F
- Offset 2-3: Length (big-endian uint16)
- Offset 4-7: Session Handle (little-endian uint32)
- Offset 8-11: Status (little-endian uint32)
- Offset 12-19: Sender Context (8 bytes)
- Offset 20-23: Options (little-endian uint32)

**CIP Service (Variable):**
- Offset 24: Service Code - Must be 0x4B
- Offset 25: Request Path Size (in 16-bit words)
- Offset 26+: Request Path (size * 2 bytes)

**PCCC Requestor ID (7 bytes):**
- Offset N+0: Length (0x07)
- Offset N+1-2: Vendor ID (little-endian uint16)
- Offset N+3-6: Serial Number (little-endian uint32)

**PCCC Command (Request - Function 0xA2):**
- Offset M+0: Command Code (0x0F)
- Offset M+1: Status (0x00)
- Offset M+2-3: TNS (little-endian uint16)
- Offset M+4: Function Code (0xA2)
- Offset M+5: Byte Size
- Offset M+6: File Number
- Offset M+7: File Type
- Offset M+8: Element Number
- Offset M+9: Sub-element Number

**PCCC Response:**
- Offset M+0: Response Code (echo of command)
- Offset M+1: Status Code (0x00=success)
- Offset M+2-3: TNS (little-endian uint16)
- Offset M+4+: Data (if status == 0x00)

### File Type Codes (CORRECTED)

```c
#define FILE_TYPE_OUTPUT       0x80  // O - Output
#define FILE_TYPE_INPUT        0x81  // I - Input
#define FILE_TYPE_STATUS       0x82  // S - Status
#define FILE_TYPE_BINARY       0x83  // B - Binary
#define FILE_TYPE_TIMER        0x84  // T - Timer
#define FILE_TYPE_COUNTER      0x85  // C - Counter
#define FILE_TYPE_CONTROL      0x86  // R - Control
#define FILE_TYPE_INTEGER      0x89  // N - Integer *** PRIMARY ***
#define FILE_TYPE_FLOAT        0x8A  // F - Float    *** PRIMARY ***
#define FILE_TYPE_ASCII        0x8D  // A - ASCII
#define FILE_TYPE_BCD          0x8F  // D - BCD
```

**CRITICAL NOTE:** The 0xA2 value is the **FUNCTION CODE**, NOT the file type!
- Function 0xA2 = "Protected Typed Logical Read with Three Address Fields"
- File Type 0x8A = Float (F files)

### Command Codes

```c
#define PCCC_CMD_0F  0x0F  // Protected Typed Logical Read/Write
// Other commands exist but 0x0F is primary focus
```

### Function Codes (for Command 0x0F)

```c
#define PCCC_FNC_0F_A1  0xA1  // Two address fields
#define PCCC_FNC_0F_A2  0xA2  // Three address fields *** PRIMARY ***
#define PCCC_FNC_0F_A9  0xA9  // Write with two address fields
#define PCCC_FNC_0F_AA  0xAA  // Write with three address fields
// Many other function codes exist
```

---

## Summary

### Integration Checklist

- [ ] Create `pccc_parser.h` with API matching Modbus patterns
- [ ] Implement `pccc_parser.c` with core parsing
- [ ] Add protocol selection to `main.c`
- [ ] Extend `pcap_reader` for port 44818
- [ ] Update build system
- [ ] Test with PCCC.pcap
- [ ] Update documentation (README, ARCHITECTURE, API)
- [ ] Validate against Wireshark
- [ ] Performance testing
- [ ] Release as v1.1 or v1.5

### Timeline Estimate

- **Phase 1 (Parsing):** 3-4 days
- **Phase 2 (Display/Stats):** 2-3 days
- **Phase 3 (Integration):** 1-2 days
- **Phase 4 (Testing/Docs):** 2-3 days

**Total:** 8-12 days of focused development

### Success Metrics

- ✅ PCCC frames parse correctly (validated vs Wireshark)
- ✅ Process variables extracted (N9, N11, F8 confirmed)
- ✅ Display modes work (table + verbose)
- ✅ Reports generate correctly
- ✅ Existing Modbus functionality unaffected
- ✅ No memory leaks (valgrind clean)
- ✅ Documentation complete

---

## References

### Source Documents

- **Wireshark PCCC Dissector:** `wireshark-packet-cip.c` and `wireshark-packet-cip.h`
- **Rockwell Specification:** `1770-rm516_-en-p.pdf` (DF1/PCCC Protocol)
- **Project Summary:** `Project_Summary_-_Gemini_-_Nov25.md`
- **Modbus Parser Docs:** README.md, ARCHITECTURE.md, API.md

### Key Insights from Wireshark Code

1. PCCC is wrapped in CIP Service 0x4B (Execute PCCC)
2. Requestor ID structure precedes PCCC command
3. Request path size is in 16-bit words (must multiply by 2 for bytes)
4. File type 0x89 = Integer, 0x8A = Float (NOT 0xA2!)
5. Transaction Number (TNS) is little-endian uint16
6. Element number is single byte for Function 0xA2

---

## Next Steps

1. **Review & Approve:** Review this architecture document for accuracy and completeness
2. **Phase 1 Implementation:** Begin with `pccc_parser.c` core parsing functions
3. **Validation:** Test parsing logic against PCCC.pcap with Wireshark comparison
4. **Iterate:** Refine based on real-world testing

---

**Document Status:** Ready for Review  
**Author:** Claude AI (Anthropic) in collaboration with Marty  
**Date:** December 2, 2025  
**Version:** 1.0
