# Modbus Parser Architecture

**Version:** 1.0-stable  
**Document Status:** Current Implementation  
**Last Updated:** December 2, 2025

---

## Table of Contents

1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Module Design](#module-design)
4. [Data Structures](#data-structures)
5. [Algorithms](#algorithms)
6. [Security Analysis](#security-analysis)
7. [Design Decisions](#design-decisions)
8. [Performance Characteristics](#performance-characteristics)
9. [Future Architecture (v2.0)](#future-architecture-v20)

---

## Overview

### Purpose

The Modbus Parser is a monolithic C application that parses Modbus TCP protocol frames from PCAP files, performs security analysis, and generates reports. This document describes the v1.0 architecture as implemented.

### Design Philosophy

**v1.0 Approach:**
- Simplicity over extensibility
- Monolithic over modular
- Direct implementation over abstraction
- Working code over perfect code

**Rationale:** Get a functional tool quickly to validate the concept before investing in framework architecture.

### Architectural Style

**Current:** Procedural C with minimal abstraction  
**Future (v2.0):** Plugin-based with kernel-style patterns

---

## System Architecture

### High-Level View

```
┌─────────────────────────────────────────────────────┐
│                    main.c                           │
│  ┌───────────────────────────────────────────────┐  │
│  │  CLI Argument Parser                          │  │
│  └───────────────┬───────────────────────────────┘  │
│                  │                                   │
│  ┌───────────────▼───────────────────────────────┐  │
│  │  Process Context Initialization               │  │
│  │  - Display mode selection                     │  │
│  │  - Report generation flag                     │  │
│  │  - Statistics structures                      │  │
│  └───────────────┬───────────────────────────────┘  │
│                  │                                   │
│  ┌───────────────▼───────────────────────────────┐  │
│  │  PCAP Processing Loop                         │  │
│  │  (via pcap_reader callback)                   │  │
│  └───────────────┬───────────────────────────────┘  │
│                  │                                   │
│  ┌───────────────▼───────────────────────────────┐  │
│  │  Per-Frame Processing                         │  │
│  │  - Parse Modbus frame                         │  │
│  │  - Display (table or verbose)                 │  │
│  │  - Update statistics                          │  │
│  │  - Write to report                            │  │
│  └───────────────┬───────────────────────────────┘  │
│                  │                                   │
│  ┌───────────────▼───────────────────────────────┐  │
│  │  Final Analysis & Reporting                   │  │
│  │  - Exception rate calculation                 │  │
│  │  - Timing analysis                            │  │
│  │  - Function code summary                      │  │
│  │  - Threat detection                           │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
         │                           │
         │                           │
         ▼                           ▼
┌──────────────────┐        ┌──────────────────┐
│  pcap_reader     │        │  modbus_parser   │
│                  │        │                  │
│  - libpcap I/O   │        │  - Frame parsing │
│  - TCP filtering │        │  - Validation    │
│  - Port 502      │        │  - Display       │
│  - Callback      │        │  - Statistics    │
└──────────────────┘        └──────────────────┘
```

### Component Interaction

```
User Command
    │
    ▼
main.c: parse_arguments()
    │
    ▼
pcap_reader: pcap_process_file()
    │
    └─► For each packet:
         │
         ├─► TCP port 502 filter
         │
         ├─► Extract payload
         │
         └─► Callback: process_modbus_payload()
              │
              ▼
         modbus_parser: modbus_parse_frame()
              │
              ├─► Validate MBAP header
              ├─► Decode function code
              └─► Parse data fields
              │
              ▼
         main.c: process_modbus_payload()
              │
              ├─► Display frame (table or verbose)
              ├─► Update attack statistics
              ├─► Track function codes
              └─► Write to report file
              │
              ▼
         Continue until EOF
    │
    ▼
main.c: final_analysis()
    │
    ├─► Calculate exception rates
    ├─► Detect scanning patterns
    ├─► Analyze timing
    └─► Generate final report
```

---

## Module Design

### main.c (Entry Point & Orchestration)

**Responsibilities:**
- Command-line argument parsing
- Process context management
- Frame callback handling
- Statistics aggregation
- Final analysis & reporting

**Key Functions:**

```c
int main(int argc, char *argv[])
```
- Entry point
- Parses CLI arguments (-v, -r, filename)
- Initializes process context
- Invokes PCAP processor
- Performs final analysis

```c
void process_modbus_payload(
    const uint8_t *payload, uint32_t length,
    const char *src_ip, uint16_t src_port,
    const char *dst_ip, uint16_t dst_port,
    double timestamp, void *user_data)
```
- Callback for each Modbus TCP payload
- Parses frame via modbus_parser
- Updates statistics
- Handles display (table or verbose)
- Writes to report file

```c
void print_usage(const char *program_name)
```
- Displays help information
- Shows command-line options
- Provides usage examples

**Data Structures:**

```c
typedef struct {
    display_mode_t mode;           // DISPLAY_TABLE or DISPLAY_VERBOSE
    uint32_t frame_count;          // Total frames processed
    uint32_t function_counts[256]; // Per-function-code counters
    attack_stats_t attack_stats;   // Security analysis state
} process_context_t;
```

**Design Rationale:**
- Single file for orchestration keeps flow visible
- Context structure passes state to callback
- Simple procedural flow easy to understand

---

### pcap_reader.c/h (Packet Capture Processing)

**Responsibilities:**
- Open and read PCAP files via libpcap
- Parse packet headers (Ethernet, IP, TCP)
- Filter for TCP port 502 (Modbus)
- Extract TCP payload
- Invoke user callback with payload data

**Key Functions:**

```c
bool pcap_process_file(
    const char *filename,
    pcap_callback_t callback,
    void *user_data)
```
- Opens PCAP file using libpcap
- Iterates through packets
- For each packet:
  - Validates Ethernet/IP/TCP layers
  - Checks for port 502
  - Extracts payload
  - Calls user callback with metadata

**Callback Type:**

```c
typedef void (*pcap_callback_t)(
    const uint8_t *payload,
    uint32_t length,
    const char *src_ip,
    uint16_t src_port,
    const char *dst_ip,
    uint16_t dst_port,
    double timestamp,
    void *user_data);
```

**Layer Parsing:**

```
Packet Structure:
┌────────────────┐
│ Ethernet (14B) │ ─► Skip (handled by libpcap)
├────────────────┤
│ IP Header      │ ─► Parse for addresses
├────────────────┤
│ TCP Header     │ ─► Parse for ports, validate 502
├────────────────┤
│ Modbus Payload │ ─► Extract and pass to callback
└────────────────┘
```

**Error Handling:**
- Returns false on PCAP open failure
- Prints warnings for malformed packets
- Skips non-TCP and non-port-502 packets
- Continues processing on parse errors

**Design Rationale:**
- Abstracts libpcap complexity from main logic
- Callback pattern allows flexible payload processing
- Focused responsibility: packet → payload extraction

---

### modbus_parser.c/h (Protocol Parsing)

**Responsibilities:**
- Parse Modbus TCP frame structure
- Validate MBAP header
- Decode function codes
- Interpret data fields based on function
- Display formatted output
- Track security statistics
- Generate report content

**Key Functions:**

```c
bool modbus_parse_frame(
    const uint8_t *payload,
    uint32_t payload_len,
    modbus_tcp_frame_t *frame)
```
- Parses MBAP header (7 bytes)
- Validates protocol ID (must be 0x0000)
- Extracts function code
- Allocates and copies data field
- Returns true on success

**MBAP Header Parsing:**

```
MBAP Structure (7 bytes):
┌──────────────────┬──────────────────┐
│ Transaction ID   │ 2 bytes          │ Big-endian uint16
├──────────────────┼──────────────────┤
│ Protocol ID      │ 2 bytes          │ Must be 0x0000
├──────────────────┼──────────────────┤
│ Length           │ 2 bytes          │ Bytes following
├──────────────────┼──────────────────┤
│ Unit ID          │ 1 byte           │ Slave address
└──────────────────┴──────────────────┘
```

```c
void modbus_display_frame(const modbus_tcp_frame_t *frame)
```
- Verbose mode display
- Shows all header fields
- Decodes function-specific data
- Pretty-prints values

```c
void modbus_display_frame_table(
    const modbus_tcp_frame_t *frame,
    const char *src_ip, uint16_t src_port,
    const char *dst_ip, uint16_t dst_port,
    uint32_t packet_num, double timestamp,
    bool print_header)
```
- Compact table format
- Single line per frame
- Color-coded fields
- Conditional header printing

```c
const char* modbus_get_function_name(uint8_t function_code)
```
- Maps function code to human-readable name
- Handles standard functions (0x01-0x2B)
- Detects exception responses (0x80+)
- Returns "Unknown" for undefined codes

**Security Analysis Functions:**

```c
void modbus_init_attack_stats(attack_stats_t *stats, bool enable_report, const char *input_filename)
```
- Initializes statistics structure
- Opens report file if enabled
- Writes report header

```c
void modbus_update_attack_stats(
    attack_stats_t *stats,
    const modbus_tcp_frame_t *frame,
    double timestamp)
```
- Updates per-frame statistics
- Tracks timing information
- Detects exception responses
- Records function code usage
- Identifies sequential patterns

```c
void modbus_finalize_attack_stats(attack_stats_t *stats)
```
- Calculates final metrics
- Computes exception rate
- Detects scanning patterns
- Writes report summary
- Closes report file

**Function Code Decoding:**

Detailed parsing for:
- **0x03/0x04** (Read Registers): Start address + quantity
- **0x06** (Write Single): Address + value
- **0x10** (Write Multiple): Start address + quantity + byte count
- **0x80+** (Exceptions): Exception code

**Data Structures:**

```c
typedef struct {
    uint16_t transaction_id;
    uint16_t protocol_id;    // Always 0x0000 for Modbus TCP
    uint16_t length;         // Bytes following (unit_id + PDU)
    uint8_t unit_id;
} modbus_mbap_header_t;

typedef struct {
    modbus_mbap_header_t mbap;
    uint8_t function_code;
    uint8_t *data;           // Dynamically allocated
    uint16_t data_length;
} modbus_tcp_frame_t;

typedef struct {
    uint32_t total_frames;
    uint32_t exception_count;
    uint32_t unique_functions_seen;
    bool function_codes_seen[256];
    bool sequential_pattern_detected;
    float exception_rate;
    
    // Timing
    double first_packet_time;
    double last_packet_time;
    double total_duration;
    double avg_frame_rate;
    uint32_t rapid_burst_count;
    
    // Reporting
    FILE *report_file;
    bool report_enabled;
} attack_stats_t;
```

**Design Rationale:**
- Single module for all Modbus logic
- Stateless parsing (no global state)
- Caller manages frame lifetime via modbus_free_frame()
- Display and analysis coupled (v2.0 will separate)

---

### colors.h (Terminal Output)

**Responsibilities:**
- Define ANSI color codes
- Support color-coded terminal output

**Definitions:**

```c
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
```

**Usage Pattern:**

```c
printf("%sCyan Text%s\n", COLOR_CYAN, COLOR_RESET);
```

**Design Rationale:**
- Simple header-only approach
- Standard ANSI codes (cross-platform)
- Easy to disable for redirection

---

## Data Structures

### Core Data Flow

```
Raw Bytes (from PCAP)
        │
        ▼
┌────────────────────┐
│ uint8_t *payload   │  Raw Modbus TCP frame
│ uint32_t length    │
└────────┬───────────┘
         │
         │ modbus_parse_frame()
         ▼
┌────────────────────────────┐
│ modbus_tcp_frame_t frame   │  Parsed structure
│  ├─ mbap (header)          │
│  ├─ function_code          │
│  └─ data (allocated)       │
└────────┬───────────────────┘
         │
         │ modbus_update_attack_stats()
         ▼
┌────────────────────────────┐
│ attack_stats_t stats       │  Accumulated analysis
│  ├─ frame counters         │
│  ├─ timing data            │
│  └─ function tracking      │
└────────────────────────────┘
```

### Memory Management

**Allocation Strategy:**
- Stack allocation for small structures (headers, stats)
- Heap allocation for variable-length data (frame->data)
- Caller-managed lifetime (modbus_free_frame() releases data)

**Example:**

```c
modbus_tcp_frame_t frame;  // Stack-allocated structure
modbus_parse_frame(payload, len, &frame);  // Allocates frame.data
// ... use frame ...
modbus_free_frame(&frame);  // Frees frame.data
```

---

## Algorithms

### 1. MBAP Header Parsing

**Algorithm:**

```
Input: payload (uint8_t *), payload_len
Output: parsed frame or false

1. Validate payload_len >= 8 (MBAP + function code minimum)
2. Extract transaction_id (bytes 0-1, big-endian)
3. Extract protocol_id (bytes 2-3, big-endian)
4. Validate protocol_id == 0x0000
5. Extract length (bytes 4-5, big-endian)
6. Validate length <= (payload_len - 6)
7. Extract unit_id (byte 6)
8. Extract function_code (byte 7)
9. Calculate data_length = length - 2
10. Allocate and copy data field (bytes 8+)
11. Return true
```

**Endianness Handling:**

```c
// Convert big-endian to host byte order
frame->mbap.transaction_id = ntohs(*(uint16_t*)&payload[0]);
frame->mbap.protocol_id = ntohs(*(uint16_t*)&payload[2]);
frame->mbap.length = ntohs(*(uint16_t*)&payload[4]);
```

**Validation:**
- Protocol ID must be 0x0000 (Modbus TCP)
- Length field must match payload
- Minimum frame size: 8 bytes

---

### 2. Exception Detection

**Algorithm:**

```
For each frame:
    1. Check if function_code >= 0x80
    2. If yes:
        - Increment exception_count
        - Extract exception code (data[0])
    3. Update total_frames

After all frames:
    exception_rate = (exception_count / total_frames) * 100.0
    if exception_rate > 20.0:
        Flag as potential threat
```

**Thresholds:**
- **Normal:** <5% exception rate
- **Warning:** 5-20% exception rate
- **Threat:** >20% exception rate

**Rationale:** High exception rates suggest:
- Scanning/enumeration attempts
- Misconfigured clients
- Attack in progress

---

### 3. Sequential Scanning Detection

**Algorithm:**

```
Initialize:
    last_function_code = 0xFF (invalid)
    sequential_probes = 0

For each frame:
    if function_code == (last_function_code + 1):
        sequential_probes++
        if sequential_probes >= 3:
            sequential_pattern_detected = true
    else:
        sequential_probes = 0
    
    last_function_code = function_code

Result:
    if sequential_pattern_detected:
        Flag as reconnaissance pattern
```

**Detection Pattern:**

```
Normal traffic:    0x03, 0x03, 0x10, 0x03, 0x10, ...
Scanning traffic:  0x01, 0x02, 0x03, 0x04, 0x05, ...
                   ^~~~ Sequential increment detected
```

**Rationale:** Attackers often enumerate function codes systematically.

---

### 4. Timing Analysis

**Burst Detection:**

```
Initialize:
    last_timestamp = 0.0
    rapid_burst_count = 0

For each frame:
    time_since_last = timestamp - last_timestamp
    
    if time_since_last < 0.1:  // 100ms threshold
        rapid_burst_count++
    
    last_timestamp = timestamp

Result:
    avg_frame_rate = total_frames / total_duration
    burst_percentage = (rapid_burst_count / total_frames) * 100
```

**Threshold:**
- **0.1 seconds**: Typical PLC poll interval is 100-1000ms
- Frames closer than 100ms suggest:
  - Automated tooling
  - Rapid scanning
  - Flood attempts

---

## Security Analysis

### Threat Model

**Assumptions:**
- Attacker has network access to OT segment
- Goal: Reconnaissance, enumeration, or disruption
- Observable via passive traffic analysis

**Out of Scope:**
- Encrypted traffic (Modbus over TLS)
- Payload-level exploits (buffer overflows, etc.)
- Physical attacks
- Social engineering

### Detection Capabilities

| Threat Type | Detection Method | Indicator |
|-------------|------------------|-----------|
| **Reconnaissance** | Sequential function code scanning | 3+ consecutive function codes |
| **Enumeration** | High function code diversity | 10+ unique function codes |
| **Flood/DoS** | Rapid frame bursts | High burst percentage |
| **Misconfiguration** | Exception rate | >20% exception responses |
| **Unusual behavior** | Timing anomalies | Irregular intervals |

### False Positives

**Possible Causes:**
- Legitimate diagnostic tools (same patterns as attacks)
- Startup/initialization sequences (may scan functions)
- Network issues (retransmissions appear as bursts)

**Mitigation:**
- Set thresholds conservatively
- Require multiple indicators
- Provide detailed logs for manual review

---

## Design Decisions

### Why Monolithic Architecture?

**Advantages:**
- Faster initial development
- Simpler debugging (all code visible)
- No plugin loading complexity
- Easier to understand for learners

**Trade-offs:**
- Adding protocols requires modifying code
- No runtime extensibility
- Analysis tied to display
- Hard to test components independently

**Decision:** Acceptable for v1.0 to prove concept before investing in framework.

---

### Why libpcap Dependency?

**Alternatives Considered:**
1. **Manual PCAP parsing:** Too complex, error-prone
2. **Pre-extracted payloads:** Requires manual Wireshark step
3. **Live capture only:** Limits forensic use case

**Decision:** libpcap provides:
- Industry-standard library
- Robust PCAP parsing
- Cross-platform support
- Future: enables live capture

**Trade-off:** External dependency, but acceptable given value.

---

### Why Statistics in modbus_parser?

**Question:** Should security analysis be in main.c?

**Current Approach:**
- Statistics tracking in modbus_parser.c
- Report generation in modbus_parser.c

**Rationale:**
- Protocol-specific statistics (exception codes, function patterns)
- Tight coupling to frame parsing
- All Modbus logic in one module

**v2.0 Change:** Will move to separate analyzer plugins.

---

### Why Two Display Modes?

**Table Mode:**
- Quick overview of traffic
- Compact for large captures
- Easy to scan for patterns

**Verbose Mode:**
- Detailed frame breakdown
- Learning tool (see all fields)
- Debugging protocol issues

**Decision:** Support both rather than force users to choose.

---

## Performance Characteristics

### Scalability

**Current Limitations:**

| Metric | Current Approach | Limitation |
|--------|------------------|------------|
| PCAP Size | Load entire file | Memory proportional to size |
| Frame Count | Process sequentially | Linear time complexity |
| Report Generation | Write per-frame | I/O overhead |
| Memory Usage | Allocate per frame | Peak = concurrent frames |

**Typical Performance (MSYS2/Windows):**
- **Small PCAP** (<10MB, <1000 frames): <1 second
- **Medium PCAP** (100MB, 10k frames): ~5-10 seconds
- **Large PCAP** (>1GB, 100k+ frames): May be slow or fail

**Bottlenecks:**
1. libpcap I/O (unavoidable)
2. Per-frame printf() calls (table mode)
3. Report file I/O (every frame write)

**Future Optimization (v2.0):**
- Streaming PCAP processing
- Batch report writing
- Optional display suppression
- Progress indicators for large files

---

### Memory Usage

**Per-Frame Allocation:**

```
Frame structure: ~32 bytes (stack)
Frame data: variable (heap) - typically 4-10 bytes for Modbus
MBAP header: 7 bytes
Statistics: ~200 bytes (persistent)
```

**Peak Memory:**
- Dominated by libpcap internal buffers
- Frame data freed after processing
- Statistics structure persists

**Typical:** <50MB for processing multi-GB PCAPs

---

## Future Architecture (v2.0)

### Migration Path

See [MIGRATION.md](MIGRATION.md) for detailed plan.

**High-Level Changes:**

```
Current (v1.0):           Future (v2.0):
                          
main.c                    main.c
├─ pcap_reader              ├─ pcap_reader
└─ modbus_parser            ├─ plugin_loader
                            ├─ logger
                            └─ analyzer_framework
                                ├─ modbus_plugin
                                ├─ pccc_plugin
                                └─ timing_analyzer
```

### Plugin Architecture

**Protocol Plugin Interface:**

```c
struct protocol_plugin_ops {
    const char *name;
    const char *description;
    uint16_t default_port;
    
    int (*init)(void);
    bool (*parse)(const uint8_t *data, size_t len, struct frame *frame);
    void (*display)(const struct frame *frame, display_mode_t mode);
    void (*cleanup)(void);
};
```

**Analyzer Plugin Interface:**

```c
struct traffic_analyzer_ops {
    const char *name;
    int (*init)(void);
    void (*update)(const struct frame *frame);
    void (*report)(void);
    void (*cleanup)(void);
};
```

### Shared Library (libprotocolparse)

**Components:**
- Plugin registration system
- Logging framework (pr_*() macros)
- Endianness helpers (be*_to_cpu())
- Common data structures
- Configuration system

**Benefits:**
- Reusable across projects
- Independent testing
- Clear API boundaries
- Versioned interfaces

---

## Appendices

### A. Function Code Reference

| Code | Name | Category |
|------|------|----------|
| 0x01 | Read Coils | Bit Access |
| 0x02 | Read Discrete Inputs | Bit Access |
| 0x03 | Read Holding Registers | Word Access |
| 0x04 | Read Input Registers | Word Access |
| 0x05 | Write Single Coil | Bit Access |
| 0x06 | Write Single Register | Word Access |
| 0x0F | Write Multiple Coils | Bit Access |
| 0x10 | Write Multiple Registers | Word Access |
| 0x14 | Read File Record | File Access |
| 0x15 | Write File Record | File Access |
| 0x16 | Mask Write Register | Word Access |
| 0x17 | Read/Write Multiple Registers | Word Access |
| 0x18 | Read FIFO Queue | Word Access |
| 0x2B | Encapsulated Interface | Diagnostics |

### B. Exception Code Reference

| Code | Name | Meaning |
|------|------|---------|
| 0x01 | Illegal Function | Function not supported |
| 0x02 | Illegal Data Address | Invalid register address |
| 0x03 | Illegal Data Value | Invalid value for register |
| 0x04 | Server Device Failure | Unrecoverable error |
| 0x05 | Acknowledge | Long operation in progress |
| 0x06 | Server Device Busy | Try again later |
| 0x08 | Memory Parity Error | Extended memory error |
| 0x0A | Gateway Path Unavailable | Gateway misconfigured |
| 0x0B | Gateway Target No Response | Target device not responding |

### C. Related Documentation

**Implementation Details:**
- [API.md](API.md) - Public API reference
- [MIGRATION.md](MIGRATION.md) - v2.0 refactoring plan

**Standards:**
- [C_CODING_STANDARD.md](C_CODING_STANDARD.md) - Code conventions
- [WRE_v2_0.md](WRE_v2_0.md) - Working practices

---

**Document Version:** 1.0  
**Corresponding Code:** v1.0-stable  
**Next Review:** Upon v2.0 planning completion
