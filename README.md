# Modbus TCP Parser & Security Analyzer

**Version:** 1.0-stable  
**Status:** Production-ready with planned v2.0 refactoring  
**License:** MIT (or specify)  
**Author:** Marty (with Claude AI assistance)

---

## Overview

Industrial protocol analyzer for Modbus TCP communications captured in PCAP files. Provides frame-by-frame parsing, security threat detection, timing analysis, and comprehensive markdown reporting capabilities.

### What It Does

- **Parses Modbus TCP frames** from packet capture files
- **Detects security anomalies** including scanning patterns and exception floods
- **Analyzes timing patterns** to identify bursts and unusual behavior
- **Generates detailed reports** in markdown format for documentation
- **Provides real-time display** with color-coded output for interactive analysis

### Use Cases

- **OT Security Analysis**: Identify reconnaissance, scanning, and attack patterns
- **Protocol Debugging**: Understand Modbus communications in industrial networks
- **Network Forensics**: Analyze historical traffic captures
- **Learning Tool**: Understand industrial protocol structures and security

---

## Current Status: v1.0-stable

### What Works ✅

- Full Modbus TCP frame parsing (MBAP header + PDU)
- All standard function codes (0x01-0x18, 0x2B)
- Exception response handling
- Security threat detection:
  - Exception rate analysis
  - Sequential scanning detection
  - Rapid burst identification
  - Function code coverage tracking
- Timing analysis (frame rate, intervals, bursts)
- Dual display modes (table and verbose)
- Markdown report generation
- Color-coded terminal output
- Cross-platform support (Windows/MSYS2, Linux, macOS)

### Known Limitations ⚠️

- **Pre-v2.0 architecture**: Monolithic design without plugin system
- **Basic logging**: Uses printf() instead of structured logging framework
- **No unit tests**: Integration testing only via PCAP files
- **Limited protocol support**: Modbus TCP only (PCCC planned)
- **PCAP-only input**: Cannot process live capture or raw hex

### What's Next 🚀

See [MIGRATION.md](MIGRATION.md) for the detailed roadmap to v2.0, which includes:
- Modular plugin architecture for multiple protocols
- Kernel-style logging framework
- Shared analysis framework (libprotocolparse)
- Comprehensive unit test suite
- Support for additional industrial protocols (PCCC, EtherNet/IP)

---

## Quick Start

### Prerequisites

**Required:**
- C compiler (GCC 13+, Clang 15+, or MSVC 2022)
- CMake 3.20+
- libpcap (or WinPcap on Windows)

**MSYS2 Setup (Windows):**
```bash
# Install dependencies
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-libpcap
```

**Linux:**
```bash
sudo apt install build-essential cmake libpcap-dev  # Debian/Ubuntu
sudo dnf install gcc cmake libpcap-devel            # Fedora
```

### Building

```bash
# Clone repository
git clone <repository-url>
cd modbus-parser

# Build
mkdir build
cd build
cmake ..
cmake --build .

# Binary location: build/modbus-parser (or modbus-parser.exe)
```

### Running

**Basic Analysis (Table Format):**
```bash
./modbus-parser capture.pcap
```

**Verbose Output:**
```bash
./modbus-parser -v capture.pcap
```

**Generate Report:**
```bash
./modbus-parser -r capture.pcap
# Creates: capture_analysis.md
```

**Combined:**
```bash
./modbus-parser -v -r capture.pcap
```

---

## Features in Detail

### 1. Frame Parsing

**Supported Frame Elements:**
- MBAP Header (7 bytes):
  - Transaction ID (2 bytes)
  - Protocol ID (2 bytes) - validates 0x0000
  - Length (2 bytes)
  - Unit ID (1 byte)
- Function Code (1 byte)
- Data payload (variable length)

**Supported Function Codes:**
```
0x01 - Read Coils
0x02 - Read Discrete Inputs
0x03 - Read Holding Registers
0x04 - Read Input Registers
0x05 - Write Single Coil
0x06 - Write Single Register
0x0F - Write Multiple Coils
0x10 - Write Multiple Registers
0x14 - Read File Record
0x15 - Write File Record
0x16 - Mask Write Register
0x17 - Read/Write Multiple Registers
0x18 - Read FIFO Queue
0x2B - Encapsulated Interface Transport
0x81+ - Exception Responses (with codes 0x01-0x0B)
```

### 2. Security Analysis

**Exception Rate Detection:**
- Tracks ratio of exception responses to total frames
- Threshold: >20% indicates possible scanning or misconfiguration

**Sequential Scanning Detection:**
- Identifies systematic function code enumeration
- Patterns: 0x01→0x02→0x03→... suggest reconnaissance

**Rapid Burst Analysis:**
- Counts frames within 0.1 seconds of previous frame
- High burst count may indicate automated tools or attacks

**Function Code Coverage:**
- Tracks unique function codes observed
- Wide coverage (>10 different codes) suggests scanning

### 3. Timing Analysis

**Metrics Calculated:**
- Total capture duration
- Average frame rate (frames/second)
- Inter-frame timing patterns
- Burst detection (frames < 0.1s apart)

**Use Cases:**
- Identify polling vs event-driven communications
- Detect unusual traffic patterns
- Baseline normal behavior for anomaly detection

### 4. Output Formats

**Table Mode (Default):**
```
Packet | Timestamp | Source | Destination | Trans ID | Unit | Function | Details
-------|-----------|--------|-------------|----------|------|----------|--------
1      | 21:45:53  | 192... | 192...      | 0x00D1   | 0x01 | Read ... | Addr=1, Qty=1
```

**Verbose Mode (-v):**
```
--- Frame 1 ---
Connection: 192.168.110.131:2074 -> 192.168.110.138:502
MBAP Header:
  Transaction ID: 0x00D1
  Protocol ID: 0x0000
  Length: 6 bytes
  Unit ID: 0x01
Function: 0x03 (Read Holding Registers)
Data: Start Address: 0x0001, Quantity: 1
```

**Report Mode (-r):**
Generates markdown file with:
- Traffic summary table
- Security analysis section
- Exception rate statistics
- Threat indicators
- Timing analysis
- Function code distribution
- Coverage analysis

---

## Architecture

### Module Structure

```
modbus-parser/
├── main.c              Entry point, CLI handling, analysis orchestration
├── pcap_reader.c/h     PCAP file parsing, TCP stream extraction
├── modbus_parser.c/h   Modbus TCP frame parsing, validation
├── colors.h            Terminal color definitions
└── CMakeLists.txt      Build configuration
```

### Data Flow

```
PCAP File → pcap_reader → TCP Payload Extraction
                              ↓
                         modbus_parser → Frame Structure
                              ↓
                         Security Analyzer → Attack Statistics
                              ↓
                         Display/Report → User Output
```

### Key Design Decisions

**Why libpcap?**
- Industry-standard packet capture library
- Handles complex PCAP format parsing
- Cross-platform support
- Future: could support live capture

**Why Modbus TCP first?**
- Most common industrial protocol
- Simpler than RTU (no CRC calculation)
- Widely used in OT environments
- Good learning protocol

**Why security focus?**
- OT networks increasingly targeted
- Simple anomaly detection valuable
- Educational for OT security learning
- Demonstrates practical application

For detailed architecture documentation, see [ARCHITECTURE.md](ARCHITECTURE.md).

---

## Documentation

### Project Documentation
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design and module details
- **[API.md](API.md)** - Public API reference
- **[MIGRATION.md](MIGRATION.md)** - v2.0 refactoring plan
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Development guidelines

### Standards & Guidelines
- **[C_CODING_STANDARD.md](C_CODING_STANDARD.md)** - v2.0 coding conventions
- **[WRE_v2_0.md](WRE_v2_0.md)** - Working relationship expectations

### Examples & References
- **[Modbus_analysis.md](Modbus_analysis.md)** - Sample report output
- **[Project_Analysis_and_Path_Forward.md](Project_Analysis_and_Path_Forward.md)** - Strategic planning

---

## Development

### Project Structure (v1.0)

```c
// Current (monolithic) structure
main.c:
  - CLI argument parsing
  - PCAP file processing
  - Frame analysis orchestration
  - Report generation
  - Statistics calculation

pcap_reader:
  - PCAP file parsing (libpcap)
  - TCP/IP layer extraction
  - Port filtering (502)
  - Payload extraction

modbus_parser:
  - Frame structure parsing
  - MBAP header validation
  - Function code decoding
  - Data field interpretation
  - Display formatting
  - Attack statistics tracking
```

### Building from Source

**Debug Build:**
```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

**Release Build:**
```bash
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

**With Verbose Output:**
```bash
cmake --build . --verbose
```

### Testing

**Current Testing Strategy:**
- Manual testing with real PCAP files
- Visual inspection of output
- Comparison with Wireshark analysis

**Test PCAP Requirements:**
```
Recommended test cases:
- Normal polling traffic
- Exception responses
- Scanning patterns
- Rapid bursts
- Multiple unit IDs
- All function codes
```

**Future (v2.0):**
- Unit test suite
- Automated regression tests
- Fuzzing with malformed frames

---

## Troubleshooting

### Build Issues

**"libpcap not found"**
```bash
# MSYS2
pacman -S mingw-w64-ucrt-x86_64-libpcap

# Linux
sudo apt install libpcap-dev
```

**"CMake version too old"**
```bash
# Minimum: CMake 3.20
cmake --version

# Upgrade if needed (MSYS2)
pacman -S mingw-w64-ucrt-x86_64-cmake
```

**Compiler warnings with -Wall**
```bash
# Current v1.0 may have minor warnings
# Acceptable for stable release
# v2.0 target: zero warnings
```

### Runtime Issues

**"Failed to open PCAP file"**
- Check file exists and path is correct
- Ensure file is valid PCAP format
- Try opening in Wireshark first

**"No Modbus TCP traffic found"**
- Verify capture contains TCP port 502
- Use Wireshark filter: `tcp.port == 502`
- Check if traffic uses non-standard port

**Report generation fails**
- Ensure write permissions in directory
- Check disk space
- File naming: `<input>_analysis.md`

---

## Contributing

### v1.0 Contributions (Current)

**Bugs & Issues:**
- Open GitHub issue with:
  - PCAP file (if shareable)
  - Command used
  - Expected vs actual output
  - System information

**Feature Requests:**
- Check MIGRATION.md first (may be planned for v2.0)
- Describe use case
- Explain why it's valuable

**Code Contributions:**
- Follow C_CODING_STANDARD.md (as much as practical)
- Test with multiple PCAP files
- Document new features

### v2.0 Planning (Future)

**Major refactoring in progress** - see [MIGRATION.md](MIGRATION.md)

If interested in contributing to v2.0:
1. Review framework design proposal
2. Consider PCCC protocol plugin (reference implementation)
3. Help with unit test suite
4. Improve documentation

---

## Roadmap

### v1.0-stable (CURRENT)
- [x] Modbus TCP parsing
- [x] Security analysis
- [x] Report generation
- [x] Cross-platform support
- [x] Comprehensive documentation

### v1.1 (Maintenance)
- [ ] Bug fixes from user reports
- [ ] Performance optimization
- [ ] Additional function code details
- [ ] Enhanced report formatting

### v2.0 (Planned - Q1 2026)
- [ ] Plugin architecture
- [ ] Logging framework
- [ ] Shared library (libprotocolparse)
- [ ] Unit test suite
- [ ] PCCC protocol support
- [ ] Configuration system
- [ ] API documentation (Doxygen)

### v3.0 (Future)
- [ ] Live capture support
- [ ] EtherNet/IP protocol
- [ ] Real-time alerting
- [ ] Database integration
- [ ] Web interface

See [MIGRATION.md](MIGRATION.md) for detailed v2.0 planning.

---

## FAQ

**Q: Why not support Modbus RTU?**  
A: RTU is serial-based and requires different capture approach. TCP is more common in modern OT networks. May add RTU in future if there's demand.

**Q: Can this detect all attacks?**  
A: No. It detects common reconnaissance patterns and anomalies. Not a replacement for IDS/IPS. Best used as forensic analysis tool.

**Q: Is this production-ready?**  
A: For analysis and learning, yes. For automated security monitoring, recommend v2.0 framework with proper testing.

**Q: What about encrypted Modbus?**  
A: Not supported. Modbus TCP is typically unencrypted. If TLS-wrapped, decrypt PCAP first.

**Q: Can I use this commercially?**  
A: Check LICENSE file. Intended for educational and analysis purposes.

**Q: Performance with large PCAPs?**  
A: Current version loads entire PCAP. For multi-GB files, consider:
  - Pre-filter with tcpdump: `tcpdump -r large.pcap -w filtered.pcap 'tcp port 502'`
  - Split into smaller chunks
  - v2.0 will have streaming support

**Q: How accurate is the security analysis?**  
A: Conservative thresholds to minimize false positives. Tune based on your network:
  - Exception rate: 20% threshold
  - Burst detection: 0.1s threshold
  - Scanning: sequential function codes

---

## References

### Modbus Specification
- Modbus Application Protocol Specification V1.1b3 (Modbus Organization)
- Modbus Messaging on TCP/IP Implementation Guide V1.0b (2006)

### Related Tools
- **Wireshark**: Visual packet analysis (comparison/validation)
- **tcpdump**: PCAP filtering and manipulation
- **modbus-cli**: Command-line Modbus client (testing)

### Learning Resources
- [Modbus Protocol Overview](https://www.modbustools.com/modbus.html)
- [Industrial Control Systems Security](https://ics-cert.us-cert.gov/)
- [OT Security Best Practices](https://www.cisa.gov/ot)

---

## Acknowledgments

**Development:**
- Claude AI (Anthropic) - Architectural guidance and implementation
- Marty - Project direction and industrial protocol expertise

**Libraries:**
- libpcap - Packet capture library
- CMake - Build system

**Inspiration:**
- Linux Kernel coding style
- Industrial protocol analysis tools
- OT security community

---

## Support

**Issues:** Open GitHub issue with details  
**Questions:** Check FAQ first, then discussions  
**Security:** For vulnerability reports, contact [details]

---

## License

[Specify License - MIT, GPL, Apache 2.0, etc.]

Copyright (c) 2025 Marty

---

**Project Start:** November 2025  
**Last Updated:** December 2, 2025  
**Maintained:** Yes (v1.x maintenance, v2.0 planning)
