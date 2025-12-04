# Modbus Parser API Reference

**Version:** 1.0-stable  
**Status:** Current Implementation  
**Last Updated:** December 2, 2025

---

## Table of Contents

1. [Overview](#overview)
2. [PCAP Reader API](#pcap-reader-api)
3. [Modbus Parser API](#modbus-parser-api)
4. [Data Structures](#data-structures)
5. [Enumerations](#enumerations)
6. [Usage Examples](#usage-examples)
7. [Error Handling](#error-handling)
8. [Thread Safety](#thread-safety)

---

## Overview

### Purpose

This document describes the public API of the Modbus Parser v1.0. All functions, structures, and constants defined in public headers (`pcap_reader.h`, `modbus_parser.h`) are documented here.

### API Stability

**v1.0-stable:**
- This API is considered stable for the v1.x series
- Breaking changes will only occur in v2.0+
- Deprecations will be announced one minor version in advance

**Deprecation Policy:**
- v1.1: Announce deprecations, provide alternatives
- v1.2+: Keep deprecated functions with warnings
- v2.0: Remove deprecated functions

### Header Files

```c
#include "pcap_reader.h"    // PCAP file processing
#include "modbus_parser.h"  // Modbus TCP frame parsing
```

---

## PCAP Reader API

### Functions

#### pcap_process_file()

```c
bool pcap_process_file(
    const char *filename,
    pcap_callback_t callback,
    void *user_data
);
```

**Description:**  
Opens a PCAP file, iterates through all packets, filters for Modbus TCP traffic (port 502), and invokes the callback for each matching frame.

**Parameters:**
- `filename`: Path to PCAP file (relative or absolute)
- `callback`: Function to invoke for each Modbus TCP payload
- `user_data`: Opaque pointer passed to callback (can be NULL)

**Returns:**
- `true`: Successfully processed file (even if 0 Modbus frames found)
- `false`: Failed to open file or other error

**Behavior:**
1. Opens PCAP file using libpcap
2. Reads packets sequentially
3. For each packet:
   - Validates Ethernet/IP/TCP layers
   - Checks if TCP destination or source port is 502
   - Extracts TCP payload
   - Invokes callback with payload and metadata
4. Closes file when complete

**Error Handling:**
- Prints error to stderr on file open failure
- Prints warnings for malformed packets (continues processing)
- Skips non-TCP packets silently
- Skips non-port-502 packets silently

**Example:**
```c
void my_callback(const uint8_t *payload, uint32_t length, 
                 const char *src_ip, uint16_t src_port,
                 const char *dst_ip, uint16_t dst_port,
                 double timestamp, void *user_data) {
    // Process payload
}

if (!pcap_process_file("capture.pcap", my_callback, NULL)) {
    fprintf(stderr, "Failed to process PCAP file\n");
    return 1;
}
```

**See Also:** `pcap_callback_t`

---

### Callback Types

#### pcap_callback_t

```c
typedef void (*pcap_callback_t)(
    const uint8_t *payload,
    uint32_t length,
    const char *src_ip,
    uint16_t src_port,
    const char *dst_ip,
    uint16_t dst_port,
    double timestamp,
    void *user_data
);
```

**Description:**  
Callback function type invoked for each Modbus TCP payload found in PCAP.

**Parameters:**
- `payload`: Pointer to Modbus TCP frame data (MBAP + PDU)
- `length`: Length of payload in bytes
- `src_ip`: Source IP address (null-terminated string, e.g., "192.168.1.1")
- `src_port`: Source TCP port
- `dst_ip`: Destination IP address (null-terminated string)
- `dst_port`: Destination TCP port (typically 502)
- `timestamp`: Packet timestamp (seconds since epoch, with microsecond precision)
- `user_data`: Opaque pointer from `pcap_process_file()` call

**Lifetime:**
- `payload`: Valid only during callback invocation
- IP address strings: Valid only during callback invocation
- Copy data if needed beyond callback scope

**Usage Notes:**
- Callback should not free `payload` (managed by PCAP reader)
- Callback should not free IP address strings
- Callback can modify `user_data` (e.g., accumulate statistics)
- Return value: void (errors should be handled internally or via `user_data`)

**Example:**
```c
typedef struct {
    uint32_t frame_count;
    uint32_t parse_errors;
} stats_t;

void process_frame(const uint8_t *payload, uint32_t length,
                   const char *src_ip, uint16_t src_port,
                   const char *dst_ip, uint16_t dst_port,
                   double timestamp, void *user_data) {
    stats_t *stats = (stats_t*)user_data;
    modbus_tcp_frame_t frame;
    
    if (modbus_parse_frame(payload, length, &frame)) {
        stats->frame_count++;
        // ... process frame ...
        modbus_free_frame(&frame);
    } else {
        stats->parse_errors++;
    }
}
```

---

## Modbus Parser API

### Core Parsing Functions

#### modbus_parse_frame()

```c
bool modbus_parse_frame(
    const uint8_t *payload,
    uint32_t payload_len,
    modbus_tcp_frame_t *frame
);
```

**Description:**  
Parses a Modbus TCP frame from raw bytes into structured format. Validates MBAP header and allocates memory for data field.

**Parameters:**
- `payload`: Raw Modbus TCP frame data (MBAP header + PDU)
- `payload_len`: Length of payload in bytes (minimum 8)
- `frame`: Output structure to populate (must not be NULL)

**Returns:**
- `true`: Frame parsed successfully, `frame` populated
- `false`: Parse error (invalid header, truncated frame, etc.)

**Behavior:**
1. Validates `payload_len >= 8` (MBAP + function code minimum)
2. Parses MBAP header (7 bytes):
   - Transaction ID (big-endian uint16)
   - Protocol ID (big-endian uint16, must be 0x0000)
   - Length (big-endian uint16)
   - Unit ID (uint8)
3. Validates length field matches payload
4. Extracts function code (byte 7)
5. Allocates and copies data field (bytes 8+)
6. Populates `frame` structure

**Memory Management:**
- Allocates `frame->data` via `malloc()` (if data_length > 0)
- Caller MUST call `modbus_free_frame()` to free allocated memory
- Do not call `free()` directly on `frame->data`

**Error Conditions:**
- `payload` is NULL → false
- `payload_len < 8` → false
- Protocol ID != 0x0000 → false
- Length field invalid → false
- Memory allocation failure → false

**Example:**
```c
modbus_tcp_frame_t frame;

if (modbus_parse_frame(payload, length, &frame)) {
    printf("Transaction ID: 0x%04X\n", frame.mbap.transaction_id);
    printf("Function Code: 0x%02X\n", frame.function_code);
    
    // ... use frame ...
    
    modbus_free_frame(&frame);  // REQUIRED
} else {
    fprintf(stderr, "Parse error\n");
}
```

**See Also:** `modbus_free_frame()`, `modbus_tcp_frame_t`

---

#### modbus_free_frame()

```c
void modbus_free_frame(modbus_tcp_frame_t *frame);
```

**Description:**  
Frees memory allocated by `modbus_parse_frame()`. Must be called for every successfully parsed frame.

**Parameters:**
- `frame`: Frame structure to free (must have been populated by `modbus_parse_frame()`)

**Behavior:**
- Frees `frame->data` if non-NULL
- Sets `frame->data` to NULL
- Sets `frame->data_length` to 0
- Does not free `frame` itself (typically stack-allocated)

**Usage Notes:**
- Safe to call multiple times (checks for NULL)
- Safe to call on failed parse (data will be NULL)
- Do not use `frame->data` after calling this function

**Example:**
```c
modbus_tcp_frame_t frame;
if (modbus_parse_frame(payload, length, &frame)) {
    // ... use frame ...
    modbus_free_frame(&frame);  // Required
}
// frame.data is now NULL, safe to go out of scope
```

---

### Display Functions

#### modbus_display_frame()

```c
void modbus_display_frame(const modbus_tcp_frame_t *frame);
```

**Description:**  
Displays detailed verbose breakdown of frame to stdout. Shows all header fields, function code name, and decoded data fields.

**Parameters:**
- `frame`: Parsed frame to display

**Output Format:**
```
MBAP Header:
  Transaction ID: 0x00D1
  Protocol ID: 0x0000
  Length: 6 bytes
  Unit ID: 0x01
Function: 0x03 (Read Holding Registers)
Data: Start Address: 0x0001, Quantity: 1
```

**Function-Specific Decoding:**
- **0x03/0x04** (Read Registers): Start address + quantity
- **0x06** (Write Single): Address + value
- **0x10** (Write Multiple): Start address + quantity + byte count
- **0x80+** (Exception): Exception code
- **Others**: Hex dump of data

**Usage Notes:**
- Intended for verbose/debugging output
- Use `modbus_display_frame_table()` for compact output
- Outputs to stdout (redirect if needed)

**Example:**
```c
modbus_tcp_frame_t frame;
if (modbus_parse_frame(payload, length, &frame)) {
    modbus_display_frame(&frame);  // Verbose output
    modbus_free_frame(&frame);
}
```

**See Also:** `modbus_display_frame_table()`

---

#### modbus_display_frame_table()

```c
void modbus_display_frame_table(
    const modbus_tcp_frame_t *frame,
    const char *src_ip,
    uint16_t src_port,
    const char *dst_ip,
    uint16_t dst_port,
    uint32_t packet_num,
    double timestamp,
    bool print_header
);
```

**Description:**  
Displays frame in compact single-line table format with color coding. Suitable for processing many frames.

**Parameters:**
- `frame`: Parsed frame to display
- `src_ip`: Source IP address
- `src_port`: Source port
- `dst_ip`: Destination IP address
- `dst_port`: Destination port
- `packet_num`: Sequence number for display (1-based)
- `timestamp`: Packet timestamp (formatted as HH:MM:SS.SSSSSS)
- `print_header`: If true, prints table header first

**Output Format:**
```
Packet | Timestamp | Source | Destination | Trans ID | Unit | Function | Details
-------|-----------|--------|-------------|----------|------|----------|--------
1      | 21:45:53  | 192... | 192...      | 0x00D1   | 0x01 | Read ... | Addr=1, Qty=1
```

**Color Coding:**
- Cyan: IP addresses
- Yellow: Transaction ID
- Green: Unit ID
- Magenta: Function code
- Blue: Data details

**Usage Notes:**
- Call with `print_header=true` for first frame only
- Subsequent frames: `print_header=false`
- Colors use ANSI codes (may not work in all terminals)
- Compact format suitable for log files (redirect)

**Example:**
```c
bool first = true;
// ... in callback loop ...
modbus_display_frame_table(&frame, src_ip, src_port, dst_ip, dst_port,
                          packet_num, timestamp, first);
first = false;
```

---

### Utility Functions

#### modbus_get_function_name()

```c
const char* modbus_get_function_name(uint8_t function_code);
```

**Description:**  
Returns human-readable name for function code.

**Parameters:**
- `function_code`: Modbus function code (0x00-0xFF)

**Returns:**
- Function name string (e.g., "Read Holding Registers")
- "Exception Response: <name>" for codes 0x80+
- "Unknown Function" for undefined codes
- String is static, do not free

**Supported Function Codes:**
```c
0x01 - "Read Coils"
0x02 - "Read Discrete Inputs"
0x03 - "Read Holding Registers"
0x04 - "Read Input Registers"
0x05 - "Write Single Coil"
0x06 - "Write Single Register"
0x0F - "Write Multiple Coils"
0x10 - "Write Multiple Registers"
0x14 - "Read File Record"
0x15 - "Write File Record"
0x16 - "Mask Write Register"
0x17 - "Read/Write Multiple Registers"
0x18 - "Read FIFO Queue"
0x2B - "Encapsulated Interface Transport"
0x80+ - Exception responses
```

**Example:**
```c
printf("Function: %s\n", modbus_get_function_name(0x03));
// Output: Function: Read Holding Registers

printf("Function: %s\n", modbus_get_function_name(0x83));
// Output: Function: Exception Response: Read Holding Registers
```

---

### Security Analysis Functions

#### modbus_init_attack_stats()

```c
void modbus_init_attack_stats(
    attack_stats_t *stats,
    bool enable_report,
    const char *input_filename
);
```

**Description:**  
Initializes security analysis statistics structure. Optionally opens report file.

**Parameters:**
- `stats`: Statistics structure to initialize
- `enable_report`: If true, create and open report file
- `input_filename`: Base filename for report (e.g., "capture.pcap" → "capture_analysis.md")

**Behavior:**
- Zeroes all statistics counters
- If `enable_report`:
  - Creates report filename by appending "_analysis.md"
  - Opens file for writing
  - Writes markdown header
  - Writes traffic summary table header

**Example:**
```c
attack_stats_t stats;
modbus_init_attack_stats(&stats, true, "capture.pcap");
// Report file "capture_analysis.md" created
```

**See Also:** `modbus_finalize_attack_stats()`

---

#### modbus_update_attack_stats()

```c
void modbus_update_attack_stats(
    attack_stats_t *stats,
    const modbus_tcp_frame_t *frame,
    double timestamp
);
```

**Description:**  
Updates security statistics based on observed frame. Call once per frame.

**Parameters:**
- `stats`: Statistics structure to update
- `frame`: Parsed frame to analyze
- `timestamp`: Frame timestamp (for timing analysis)

**Behavior:**
- Increments total frame counter
- Detects exception responses (function code 0x80+)
- Tracks unique function codes
- Detects sequential scanning patterns
- Calculates timing metrics
- Identifies rapid bursts

**Statistics Updated:**
- `total_frames`: Total frames seen
- `exception_count`: Exception responses
- `unique_functions_seen`: Count of unique function codes
- `function_codes_seen[]`: Bitmap of observed codes
- `rapid_burst_count`: Frames < 0.1s apart
- Timing data: first/last packet times, durations

**Example:**
```c
// In callback loop
modbus_update_attack_stats(&stats, &frame, timestamp);
```

**See Also:** `modbus_finalize_attack_stats()`

---

#### modbus_finalize_attack_stats()

```c
void modbus_finalize_attack_stats(attack_stats_t *stats);
```

**Description:**  
Calculates final statistics and generates security analysis report. Call after processing all frames.

**Parameters:**
- `stats`: Statistics structure to finalize

**Behavior:**
1. Calculates derived metrics:
   - Exception rate (percentage)
   - Average frame rate
   - Total duration
2. Detects threat patterns:
   - High exception rate (>20%)
   - Sequential scanning
   - Rapid bursts
3. If report enabled:
   - Writes security analysis section
   - Writes threat indicators
   - Writes timing analysis
   - Writes function code summary
   - Closes report file

**Report Sections:**
- Exception Rate Analysis
- Threat Indicators
- Timing Analysis
- Function Code Summary
- Function Code Coverage

**Example:**
```c
attack_stats_t stats;
modbus_init_attack_stats(&stats, true, "capture.pcap");

// ... process frames, calling modbus_update_attack_stats() ...

modbus_finalize_attack_stats(&stats);
// Report complete, file closed
```

---

#### modbus_write_report_frame()

```c
void modbus_write_report_frame(
    attack_stats_t *stats,
    const modbus_tcp_frame_t *frame,
    const char *src_ip,
    uint16_t src_port,
    const char *dst_ip,
    uint16_t dst_port,
    uint32_t packet_num,
    double timestamp
);
```

**Description:**  
Writes single frame entry to report file (traffic summary table). Call once per frame if reporting enabled.

**Parameters:**
- `stats`: Statistics structure (must have report enabled)
- `frame`: Parsed frame to report
- `src_ip`, `src_port`: Source address
- `dst_ip`, `dst_port`: Destination address
- `packet_num`: Frame sequence number
- `timestamp`: Frame timestamp

**Behavior:**
- Writes markdown table row
- Formats timestamp as HH:MM:SS.SSSSSS
- Includes function code and details
- No-op if report not enabled

**Example:**
```c
modbus_write_report_frame(&stats, &frame, src_ip, src_port, 
                         dst_ip, dst_port, packet_num, timestamp);
```

---

## Data Structures

### modbus_mbap_header_t

```c
typedef struct {
    uint16_t transaction_id;  // Transaction identifier
    uint16_t protocol_id;     // Protocol identifier (0x0000)
    uint16_t length;          // Bytes following (unit_id + PDU)
    uint8_t unit_id;          // Unit/slave identifier
} modbus_mbap_header_t;
```

**Description:**  
Modbus Application Protocol (MBAP) header structure. Fixed 7 bytes at start of every Modbus TCP frame.

**Fields:**
- `transaction_id`: Request/response correlation (0x0000-0xFFFF)
- `protocol_id`: Always 0x0000 for Modbus TCP
- `length`: Number of bytes following this field (typically 2-255)
- `unit_id`: Target device address (0x00-0xFF, 0xFF = broadcast)

**Byte Layout:**
```
Offset | Size | Field
-------|------|-------------------
0      | 2    | transaction_id
2      | 2    | protocol_id
4      | 2    | length
6      | 1    | unit_id
```

**Endianness:** All multi-byte fields are big-endian (network byte order).

---

### modbus_tcp_frame_t

```c
typedef struct {
    modbus_mbap_header_t mbap;  // MBAP header
    uint8_t function_code;      // Modbus function code
    uint8_t *data;              // Function-specific data (allocated)
    uint16_t data_length;       // Length of data field
} modbus_tcp_frame_t;
```

**Description:**  
Complete parsed Modbus TCP frame structure.

**Fields:**
- `mbap`: MBAP header (see `modbus_mbap_header_t`)
- `function_code`: Modbus function (0x01-0xFF)
  - 0x01-0x7F: Standard functions
  - 0x80-0xFF: Exception responses (0x80 + function code)
- `data`: Dynamically allocated function-specific data
  - NULL if data_length is 0
  - Must be freed via `modbus_free_frame()`
- `data_length`: Size of data field in bytes

**Memory Management:**
- Structure itself: typically stack-allocated
- `data` field: heap-allocated by `modbus_parse_frame()`
- Lifetime: managed by caller via `modbus_free_frame()`

**Usage Pattern:**
```c
modbus_tcp_frame_t frame;              // Stack-allocated
modbus_parse_frame(payload, len, &frame);  // Allocates frame.data
// ... use frame ...
modbus_free_frame(&frame);             // Frees frame.data
// frame goes out of scope
```

---

### attack_stats_t

```c
typedef struct {
    // Frame counters
    uint32_t total_frames;
    uint32_t exception_count;
    uint32_t unique_functions_seen;
    uint32_t sequential_probes;
    
    // Function tracking
    bool function_codes_seen[256];
    uint8_t last_function_code;
    bool sequential_pattern_detected;
    float exception_rate;
    
    // Report generation
    FILE *report_file;
    bool report_enabled;
    
    // Timing analysis
    double first_packet_time;
    double last_packet_time;
    double total_duration;
    double avg_frame_rate;
    uint32_t rapid_burst_count;
} attack_stats_t;
```

**Description:**  
Security analysis statistics accumulator.

**Frame Counters:**
- `total_frames`: Total frames processed
- `exception_count`: Exception responses seen
- `unique_functions_seen`: Count of unique function codes
- `sequential_probes`: Consecutive sequential function codes

**Function Tracking:**
- `function_codes_seen[256]`: Bitmap of observed function codes
- `last_function_code`: Previous function code (for pattern detection)
- `sequential_pattern_detected`: True if scanning pattern found
- `exception_rate`: Calculated percentage (0.0-100.0)

**Report Generation:**
- `report_file`: Open file handle (NULL if disabled)
- `report_enabled`: Report flag

**Timing Analysis:**
- `first_packet_time`: Timestamp of first frame (seconds)
- `last_packet_time`: Timestamp of last frame
- `total_duration`: Time span of capture
- `avg_frame_rate`: Frames per second
- `rapid_burst_count`: Frames < 0.1s apart

**Initialization:**
```c
attack_stats_t stats;
modbus_init_attack_stats(&stats, enable_report, filename);
```

---

## Enumerations

### display_mode_t

```c
typedef enum {
    DISPLAY_TABLE,    // Compact table format
    DISPLAY_VERBOSE   // Detailed breakdown
} display_mode_t;
```

**Description:**  
Display mode selection for frame output.

**Values:**
- `DISPLAY_TABLE`: Single-line table format (default)
- `DISPLAY_VERBOSE`: Multi-line detailed format

**Usage:**
```c
display_mode_t mode = DISPLAY_TABLE;  // or DISPLAY_VERBOSE

// Pass to display functions
modbus_display_frame_table(...);  // For table mode
modbus_display_frame(...);        // For verbose mode
```

---

### modbus_exception_code_t

```c
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
```

**Description:**  
Standard Modbus exception codes returned in exception responses.

**Usage:**
```c
if (frame.function_code >= 0x80) {
    uint8_t exception_code = frame.data[0];
    switch (exception_code) {
    case MODBUS_EXCEPTION_ILLEGAL_FUNCTION:
        printf("Function not supported\n");
        break;
    case MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS:
        printf("Invalid register address\n");
        break;
    // ... etc
    }
}
```

---

## Usage Examples

### Example 1: Basic Frame Parsing

```c
#include "pcap_reader.h"
#include "modbus_parser.h"

void process_payload(const uint8_t *payload, uint32_t length,
                     const char *src_ip, uint16_t src_port,
                     const char *dst_ip, uint16_t dst_port,
                     double timestamp, void *user_data) {
    modbus_tcp_frame_t frame;
    
    if (modbus_parse_frame(payload, length, &frame)) {
        printf("Function: %s\n", 
               modbus_get_function_name(frame.function_code));
        modbus_free_frame(&frame);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pcap_file>\n", argv[0]);
        return 1;
    }
    
    if (!pcap_process_file(argv[1], process_payload, NULL)) {
        fprintf(stderr, "Failed to process PCAP\n");
        return 1;
    }
    
    return 0;
}
```

---

### Example 2: Security Analysis

```c
#include "pcap_reader.h"
#include "modbus_parser.h"

typedef struct {
    attack_stats_t stats;
    uint32_t frame_count;
} context_t;

void analyze_frame(const uint8_t *payload, uint32_t length,
                   const char *src_ip, uint16_t src_port,
                   const char *dst_ip, uint16_t dst_port,
                   double timestamp, void *user_data) {
    context_t *ctx = (context_t*)user_data;
    modbus_tcp_frame_t frame;
    
    if (modbus_parse_frame(payload, length, &frame)) {
        ctx->frame_count++;
        modbus_update_attack_stats(&ctx->stats, &frame, timestamp);
        modbus_write_report_frame(&ctx->stats, &frame, src_ip, src_port,
                                 dst_ip, dst_port, ctx->frame_count, 
                                 timestamp);
        modbus_free_frame(&frame);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pcap_file>\n", argv[0]);
        return 1;
    }
    
    context_t ctx = {0};
    modbus_init_attack_stats(&ctx.stats, true, argv[1]);
    
    if (!pcap_process_file(argv[1], analyze_frame, &ctx)) {
        fprintf(stderr, "Failed to process PCAP\n");
        return 1;
    }
    
    modbus_finalize_attack_stats(&ctx.stats);
    
    printf("Processed %u frames\n", ctx.frame_count);
    printf("Exception rate: %.1f%%\n", ctx.stats.exception_rate);
    
    return 0;
}
```

---

### Example 3: Dual Display Mode

```c
#include "pcap_reader.h"
#include "modbus_parser.h"

typedef struct {
    display_mode_t mode;
    bool first_frame;
} context_t;

void display_frame(const uint8_t *payload, uint32_t length,
                   const char *src_ip, uint16_t src_port,
                   const char *dst_ip, uint16_t dst_port,
                   double timestamp, void *user_data) {
    context_t *ctx = (context_t*)user_data;
    modbus_tcp_frame_t frame;
    static uint32_t packet_num = 0;
    
    if (modbus_parse_frame(payload, length, &frame)) {
        packet_num++;
        
        if (ctx->mode == DISPLAY_VERBOSE) {
            printf("\n--- Frame %u ---\n", packet_num);
            modbus_display_frame(&frame);
        } else {
            modbus_display_frame_table(&frame, src_ip, src_port,
                                      dst_ip, dst_port, packet_num,
                                      timestamp, ctx->first_frame);
            ctx->first_frame = false;
        }
        
        modbus_free_frame(&frame);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [-v|-t] <pcap_file>\n", argv[0]);
        return 1;
    }
    
    context_t ctx = {
        .mode = (strcmp(argv[1], "-v") == 0) ? DISPLAY_VERBOSE : DISPLAY_TABLE,
        .first_frame = true
    };
    
    if (!pcap_process_file(argv[2], display_frame, &ctx)) {
        fprintf(stderr, "Failed to process PCAP\n");
        return 1;
    }
    
    return 0;
}
```

---

## Error Handling

### Return Values

**Boolean Functions:**
- `true` (1): Success
- `false` (0): Failure

**Pointer Functions:**
- Valid pointer: Success
- `NULL`: Failure
- Static strings: Never NULL (may be "Unknown")

### Error Checking Pattern

```c
// Check boolean return
if (!modbus_parse_frame(payload, length, &frame)) {
    fprintf(stderr, "Parse error\n");
    // Handle error
    return;
}

// Check PCAP processing
if (!pcap_process_file(filename, callback, data)) {
    fprintf(stderr, "PCAP error: %s\n", filename);
    return 1;
}

// Check pointer validity (payload from callback)
if (payload == NULL || length == 0) {
    return;  // Skip invalid payload
}
```

### Memory Errors

**Allocation Failures:**
- `modbus_parse_frame()` returns false if malloc() fails
- Check return value before using frame
- No memory leaks on allocation failure

**Invalid Free:**
- Only call `modbus_free_frame()` on successfully parsed frames
- Safe to call multiple times (checks NULL)
- Do not call `free()` directly on `frame->data`

---

## Thread Safety

### Current Implementation (v1.0)

**Thread Safety:** **NOT thread-safe**

**Reasons:**
- PCAP reader uses static buffers
- Global state in some functions
- No mutex protection

**Implications:**
- Do not call API functions from multiple threads
- Do not process multiple PCAP files concurrently
- Do not share `attack_stats_t` across threads

### Recommendations

**Single-Threaded Usage:**
- Process one PCAP file at a time
- Use single callback execution path
- Sequential frame processing

**Multi-Threaded Usage (v2.0):**
- Not supported in v1.0
- v2.0 will provide thread-safe API
- Consider mutex protection if modifying code

---

## Deprecation Notices

### v1.0

No deprecated functions.

### Future Deprecations (v2.0)

The following functions will be deprecated in v2.0:

- `modbus_parse_frame()` → Use plugin interface
- `modbus_display_frame()` → Use plugin interface
- Direct access to `modbus_tcp_frame_t` → Use generic `struct frame`

Migration guide will be provided with v2.0.

---

## Related Documentation

- [README.md](README.md) - Project overview and quick start
- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
- [MIGRATION.md](MIGRATION.md) - v2.0 migration plan
- [C_CODING_STANDARD.md](C_CODING_STANDARD.md) - Code conventions

---

**Document Version:** 1.0  
**API Version:** 1.0-stable  
**Last Updated:** December 2, 2025
