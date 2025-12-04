# Migration to v2.0 Standards

**Version:** 1.0 → 2.0 Migration Plan  
**Status:** Planning Phase  
**Target Date:** Q1 2026  
**Last Updated:** December 2, 2025

---

## Table of Contents

1. [Overview](#overview)
2. [Compliance Audit](#compliance-audit)
3. [Migration Strategy](#migration-strategy)
4. [Phase Breakdown](#phase-breakdown)
5. [Breaking Changes](#breaking-changes)
6. [Risk Assessment](#risk-assessment)
7. [Testing Strategy](#testing-strategy)
8. [Timeline](#timeline)

---

## Overview

### Purpose

This document outlines the plan to migrate the Modbus Parser from its current v1.0 monolithic implementation to a v2.0 architecture that fully complies with the C Coding Standard v2.0 and supports multi-protocol analysis.

### Goals

**Primary:**
- Achieve 100% compliance with C_CODING_STANDARD.md v2.0
- Extract reusable framework (libprotocolparse)
- Enable multi-protocol support via plugins
- Maintain backward compatibility where possible

**Secondary:**
- Add comprehensive unit testing
- Improve documentation
- Enhance security analysis capabilities
- Support live capture (future)

### Non-Goals

- Rewrite working Modbus parser from scratch
- Support protocols outside industrial control (ICS/SCADA)
- Build GUI interface
- Real-time performance optimization (batch analysis focus)

---

## Compliance Audit

### Current State vs v2.0 Standard

| Standard Section | v1.0 Compliance | Gap Analysis | Priority |
|------------------|-----------------|--------------|----------|
| **Naming Conventions** | 80% | Some function names inconsistent | Low |
| **File Organization** | 90% | Header guards correct, minor issues | Low |
| **Documentation** | 30% | Missing function docs, no API docs | High |
| **Plugin Architecture** | 0% | No plugin system at all | Critical |
| **Logging System** | 0% | Using printf(), no log levels | High |
| **Endianness Handling** | 60% | Direct ntohs(), no abstraction | Medium |
| **Traffic Analysis** | 40% | Monolithic, not framework-based | High |
| **Error Handling** | 70% | Inconsistent patterns | Medium |
| **Build System** | 80% | CMake good, needs templates | Low |
| **Testing** | 0% | No unit tests | Critical |

### Detailed Gap Analysis

#### 1. Plugin Architecture (Critical Gap)

**Current:**
```c
// Monolithic - all in modbus_parser.c
bool modbus_parse_frame(...) { }
void modbus_display_frame(...) { }
```

**Required v2.0:**
```c
// Plugin pattern with ops structure
struct protocol_plugin_ops {
    const char *name;
    int (*init)(void);
    bool (*parse)(const uint8_t *data, size_t len, struct frame *frame);
    void (*display)(const struct frame *frame, display_mode_t mode);
    void (*cleanup)(void);
};

// Registration system
int protocol_register_plugin(struct protocol_plugin_ops *ops);
```

**Migration Impact:**
- High: Requires architectural refactoring
- Benefit: Enables PCCC and future protocols
- Risk: Could break existing code if not careful

---

#### 2. Logging System (Critical Gap)

**Current:**
```c
// Direct printf() throughout
printf("Failed to parse Modbus frame\n");
printf("Frame parsed successfully\n");
```

**Required v2.0:**
```c
// Kernel-style logging macros
pr_err("Failed to parse frame: %s", reason);
pr_info("Frame parsed successfully");
pr_debug("Parsing frame: %zu bytes", len);
pr_warn("Unusual timing pattern detected");
```

**Migration Impact:**
- Medium: Search-and-replace with validation
- Benefit: Structured logging, filtering, file output
- Risk: Low - mechanical transformation

---

#### 3. Endianness Handling (Medium Gap)

**Current:**
```c
// Direct use of ntohs/ntohl
frame->mbap.transaction_id = ntohs(*(uint16_t*)&payload[0]);
frame->mbap.protocol_id = ntohs(*(uint16_t*)&payload[2]);
```

**Required v2.0:**
```c
// Kernel-style helpers
frame->mbap.transaction_id = be16_to_cpu(*(uint16_t*)&payload[0]);
frame->mbap.protocol_id = be16_to_cpu(*(uint16_t*)&payload[2]);

// Or safe unaligned version
frame->mbap.transaction_id = get_unaligned_be16(&payload[0]);
```

**Migration Impact:**
- Low: Wrapper functions can coexist with ntohs
- Benefit: Portability, clarity, consistency
- Risk: Very low - same semantics

---

#### 4. Traffic Analysis Framework (High Gap)

**Current:**
```c
// Modbus-specific analysis in modbus_parser.c
void modbus_update_attack_stats(attack_stats_t *stats, ...) {
    // Exception detection
    // Timing analysis
    // Pattern detection
}
```

**Required v2.0:**
```c
// Generic analyzer framework
struct traffic_analyzer_ops {
    const char *name;
    void (*update)(const struct frame *frame);
    void (*report)(void);
};

// Protocol-specific analyzers as plugins
static struct traffic_analyzer_ops modbus_security = {
    .name = "modbus-security",
    .update = analyze_modbus_security,
    .report = report_modbus_security
};
```

**Migration Impact:**
- High: Extract and generalize analysis code
- Benefit: Reusable across protocols, composable
- Risk: Medium - requires careful abstraction

---

#### 5. Documentation (High Gap)

**Current:**
```c
// Minimal or no function documentation
bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len, 
                       modbus_tcp_frame_t *frame) {
    // Implementation...
}
```

**Required v2.0:**
```c
/**
 * modbus_parse_frame() - Parse Modbus TCP frame from raw bytes
 * @payload: Raw frame data from network
 * @payload_len: Length of payload in bytes
 * @frame: Output structure to populate
 *
 * Parses MBAP header and PDU from raw bytes. Allocates memory for
 * frame data field which must be freed via modbus_free_frame().
 *
 * Return: true on success, false on parse error or invalid frame
 */
bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len,
                       modbus_tcp_frame_t *frame)
```

**Migration Impact:**
- Medium: Time-consuming but mechanical
- Benefit: API clarity, maintenance, learning
- Risk: None - purely additive

---

#### 6. Testing (Critical Gap)

**Current:**
- No unit tests
- Manual testing with PCAP files only

**Required v2.0:**
```c
// Unit test example (using Check or similar)
START_TEST(test_modbus_parse_valid_frame)
{
    uint8_t frame[] = {
        0x00, 0x01,  // Transaction ID
        0x00, 0x00,  // Protocol ID
        0x00, 0x06,  // Length
        0x01,        // Unit ID
        0x03,        // Function code
        0x00, 0x00,  // Start address
        0x00, 0x0A   // Quantity
    };
    
    modbus_tcp_frame_t parsed;
    ck_assert(modbus_parse_frame(frame, sizeof(frame), &parsed));
    ck_assert_uint_eq(parsed.mbap.transaction_id, 0x0001);
    ck_assert_uint_eq(parsed.function_code, 0x03);
    
    modbus_free_frame(&parsed);
}
END_TEST
```

**Migration Impact:**
- High: Significant new infrastructure
- Benefit: Prevents regressions, enables refactoring
- Risk: Low - doesn't affect production code

---

## Migration Strategy

### Approach: Hybrid (Recommended)

**Phase 1: Stabilization** (v1.0 → v1.1)
- Document current implementation
- Add comprehensive function documentation
- Minor fixes and improvements
- **No architectural changes**
- Tag: `v1.1-stable`

**Phase 2: Framework Extraction** (v1.1 → v2.0-alpha)
- Create libprotocolparse library
- Implement plugin architecture
- Add logging framework
- Add endianness helpers
- **Modbus parser remains v1.1**
- Tag: `v2.0-alpha`

**Phase 3: Reference Implementation** (v2.0-alpha → v2.0-beta)
- Build PCCC parser as first plugin
- Validate all v2.0 standards
- Create documentation and examples
- **Modbus parser still v1.1**
- Tag: `v2.0-beta-pccc`

**Phase 4: Modbus Migration** (v2.0-beta → v2.0)
- Port Modbus parser to plugin architecture
- Maintain CLI compatibility
- Add new analysis capabilities
- Comprehensive testing
- Tag: `v2.0-release`

### Alternative: Big Bang (Not Recommended)

**Approach:** Rewrite everything at once to v2.0

**Why Not:**
- High risk of breaking working code
- No intermediate validation points
- Difficult to track issues
- Long delay before usable release

**When It Makes Sense:**
- If v1.0 code is fundamentally broken (it's not)
- If requirements have completely changed (they haven't)
- If learning exercise only (but we want working tool)

---

## Phase Breakdown

### Phase 1: Stabilization (Week 1)

**Goals:**
- Document current state completely
- No functional changes
- Prepare for extraction

**Deliverables:**

1. **Enhanced Documentation**
   - [x] README.md (comprehensive)
   - [x] ARCHITECTURE.md (current design)
   - [x] MIGRATION.md (this document)
   - [ ] API.md (public interface reference)
   - [ ] CONTRIBUTING.md (development guide)

2. **Function Documentation**
   - [ ] Document all public functions (Doxygen style)
   - [ ] Document all structures
   - [ ] Add file header comments
   - [ ] Explain algorithm choices

3. **Code Cleanup**
   - [ ] Consistent naming (fix 20% non-compliant)
   - [ ] Remove dead code
   - [ ] Fix compiler warnings
   - [ ] Run static analyzer (cppcheck)

4. **Release Preparation**
   - [ ] Verify builds on all platforms
   - [ ] Test with diverse PCAP files
   - [ ] Create test data repository
   - [ ] Tag: `v1.1-stable`

**Success Criteria:**
- Zero compiler warnings with -Wall
- All functions documented
- Clean static analysis report
- Builds and runs on Windows/Linux/macOS

---

### Phase 2: Framework Development (Weeks 2-3)

**Goals:**
- Create reusable components
- Implement v2.0 patterns
- No changes to Modbus parser yet

**Deliverables:**

1. **libprotocolparse Library**
   ```
   libprotocolparse/
   ├── include/
   │   ├── protocol.h      (plugin interface)
   │   ├── analyzer.h      (analyzer interface)
   │   ├── logger.h        (logging framework)
   │   ├── byteorder.h     (endianness helpers)
   │   └── frame.h         (common structures)
   ├── src/
   │   ├── plugin.c        (registration system)
   │   ├── analyzer.c      (analyzer framework)
   │   └── logger.c        (logging implementation)
   ├── tests/
   │   └── test_*.c        (unit tests)
   └── CMakeLists.txt
   ```

2. **Plugin Architecture**
   - [ ] Define protocol_plugin_ops interface
   - [ ] Implement registration system
   - [ ] Create example/test plugin
   - [ ] Document plugin development

3. **Logging Framework**
   - [ ] Implement pr_*() macros
   - [ ] Log level filtering
   - [ ] File and stderr output
   - [ ] Color support
   - [ ] Thread-safe (optional)

4. **Endianness Helpers**
   - [ ] be*_to_cpu() / cpu_to_be*()
   - [ ] le*_to_cpu() / cpu_to_le*()
   - [ ] get_unaligned_*() helpers
   - [ ] Cross-platform testing

5. **Analyzer Framework**
   - [ ] Define traffic_analyzer_ops interface
   - [ ] Implement registration system
   - [ ] Create example analyzer
   - [ ] Document analyzer development

6. **Testing Infrastructure**
   - [ ] Choose test framework (Check)
   - [ ] Set up CMake test targets
   - [ ] Write unit tests for framework
   - [ ] CI/CD setup (optional)

**Success Criteria:**
- Library compiles independently
- All interfaces documented
- Unit tests pass
- Example plugin works
- Zero external dependencies (except test framework)

---

### Phase 3: PCCC Reference Implementation (Weeks 4-5)

**Goals:**
- Validate v2.0 architecture
- Create reference example
- Document patterns

**Deliverables:**

1. **PCCC Protocol Plugin**
   - [ ] PCCC frame parsing
   - [ ] PCIP (EtherNet/IP) encapsulation
   - [ ] Function code decoding
   - [ ] Display formatting

2. **PCCC Analyzer Plugins**
   - [ ] Security analyzer (similar to Modbus)
   - [ ] Timing analyzer
   - [ ] Protocol compliance checker

3. **Testing**
   - [ ] Unit tests for PCCC parser
   - [ ] PCAP test files
   - [ ] Validation against real traffic
   - [ ] Comparison with other tools

4. **Documentation**
   - [ ] Plugin development guide
   - [ ] PCCC protocol reference
   - [ ] Example code walkthrough
   - [ ] Lessons learned document

**Success Criteria:**
- PCCC parser feature-complete
- Passes all unit tests
- Parses real PCAP files correctly
- Documentation sufficient for third-party plugins
- Tag: `v2.0-beta-pccc`

---

### Phase 4: Modbus Migration (Weeks 6-7)

**Goals:**
- Port Modbus to v2.0 architecture
- Maintain backward compatibility
- Add enhanced features

**Deliverables:**

1. **Modbus Plugin**
   - [ ] Port parsing logic to plugin interface
   - [ ] Implement protocol_plugin_ops
   - [ ] Migrate display functions
   - [ ] Update error handling

2. **Modbus Analyzers**
   - [ ] Security analyzer (port from v1.1)
   - [ ] Enhanced timing analysis
   - [ ] New: Protocol compliance checker
   - [ ] New: Traffic profiler

3. **Backward Compatibility**
   - [ ] Maintain CLI interface
   - [ ] Same report format (or enhanced)
   - [ ] Migration guide for users
   - [ ] Deprecation notices where needed

4. **Testing**
   - [ ] Port integration tests
   - [ ] Unit tests for Modbus plugin
   - [ ] Regression testing vs v1.1
   - [ ] Performance benchmarking

5. **Documentation**
   - [ ] Update README for v2.0
   - [ ] Migration guide for users
   - [ ] Architecture diagram update
   - [ ] Release notes

**Success Criteria:**
- Feature parity with v1.1
- All existing PCAP files parse correctly
- New features work
- Performance not significantly degraded
- Tag: `v2.0-release`

---

## Breaking Changes

### Command-Line Interface

**v1.1:**
```bash
modbus-parser -v -r capture.pcap
```

**v2.0:**
```bash
# Same for Modbus (backward compatible)
modbus-parser -v -r capture.pcap

# New: Protocol selection (when multiple protocols)
protocol-parser --protocol modbus -v -r capture.pcap
protocol-parser --protocol pccc -v -r capture.pcap

# Or auto-detect from port
protocol-parser --auto-detect -v -r capture.pcap
```

**Compatibility Strategy:**
- Keep `modbus-parser` binary that defaults to Modbus
- Add `protocol-parser` for multi-protocol
- Detect port and suggest correct protocol

---

### API Changes

**v1.1 (current):**
```c
bool modbus_parse_frame(const uint8_t *payload, uint32_t payload_len,
                       modbus_tcp_frame_t *frame);
void modbus_display_frame(const modbus_tcp_frame_t *frame);
```

**v2.0 (plugin interface):**
```c
// Generic frame structure
struct frame {
    const struct protocol_plugin_ops *protocol;
    void *protocol_data;  // protocol-specific data
    // Common metadata
    uint32_t frame_number;
    double timestamp;
    // ... etc
};

// Plugin calls
bool (*parse)(const uint8_t *data, size_t len, struct frame *frame);
void (*display)(const struct frame *frame, display_mode_t mode);
```

**Compatibility Strategy:**
- v1.1 functions remain available (legacy library)
- v2.0 adds new interface
- Deprecation warnings in v2.0
- Removal in v3.0 (if ever)

---

### Build System

**v1.1:**
```cmake
# Simple single-target
add_executable(modbus-parser main.c pcap_reader.c modbus_parser.c)
```

**v2.0:**
```cmake
# Library + plugins + executable
add_library(protocolparse SHARED <lib sources>)
add_library(plugin_modbus SHARED modbus_plugin.c)
add_library(plugin_pccc SHARED pccc_plugin.c)
add_executable(protocol-parser main.c)
target_link_libraries(protocol-parser protocolparse)
```

**Compatibility Strategy:**
- Keep simple build available for Modbus-only
- Advanced build for full framework
- Document both approaches

---

### Report Format

**v1.1:**
```markdown
# Modbus TCP Security Analysis Report
...
```

**v2.0:**
```markdown
# Protocol Security Analysis Report
**Protocol:** Modbus TCP
**Version:** 2.0
...

## Traffic Summary
[Same format, possibly enhanced]

## Security Analysis
[Same sections, possibly additional]

## New Section: Protocol Compliance
[New feature]
```

**Compatibility Strategy:**
- Same filename scheme
- Same sections (enhanced)
- Additional sections clearly marked
- Option to use v1.1 format

---

## Risk Assessment

### Technical Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| **Framework too complex** | High | Medium | Start simple, iterate; YAGNI principle |
| **Performance regression** | Medium | Low | Benchmark, profile; maintain v1.1 if needed |
| **Plugin system bugs** | High | Medium | Extensive testing; fallback to monolithic |
| **Breaking existing workflows** | High | Low | Backward compatibility layer; clear migration docs |
| **PCCC implementation issues** | Medium | Medium | Thorough protocol research; real traffic testing |
| **Testing infrastructure gaps** | Medium | High | Incremental test addition; CI/CD automation |

### Project Risks

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| **Scope creep** | High | High | Strict phase gates; PCCC only for v2.0 |
| **Timeline slippage** | Medium | Medium | Buffer time; prioritize critical features |
| **Documentation lag** | Medium | High | Document as you go; reviewable docs |
| **Loss of working tool** | High | Low | Git branches; v1.1-stable always available |
| **Abandonment of v2.0** | High | Low | Incremental value; each phase delivers |

---

## Testing Strategy

### Unit Testing

**Framework Components:**
```c
// Test logging system
test_log_levels()
test_log_filtering()
test_log_file_output()

// Test plugin system
test_plugin_registration()
test_plugin_initialization()
test_plugin_invocation()

// Test endianness helpers
test_be16_conversion()
test_le32_conversion()
test_unaligned_access()
```

**Protocol Parsers:**
```c
// Test Modbus parser
test_modbus_parse_valid_frame()
test_modbus_parse_exception()
test_modbus_invalid_protocol_id()
test_modbus_truncated_frame()

// Test PCCC parser
test_pccc_parse_read_request()
test_pccc_parse_write_request()
test_pccc_encapsulation()
```

**Analyzers:**
```c
// Test security analyzer
test_exception_detection()
test_scanning_detection()
test_timing_analysis()
test_burst_detection()
```

---

### Integration Testing

**PCAP Test Suite:**
```
test_data/
├── modbus/
│   ├── normal_polling.pcap          (baseline)
│   ├── exception_flood.pcap         (high exception rate)
│   ├── function_scan.pcap           (sequential scanning)
│   ├── rapid_burst.pcap             (timing anomaly)
│   └── multi_unit.pcap              (various unit IDs)
├── pccc/
│   ├── normal_traffic.pcap
│   ├── read_requests.pcap
│   └── write_commands.pcap
└── mixed/
    └── both_protocols.pcap          (port-based selection)
```

**Test Scenarios:**
- Parse all frames successfully
- Security analysis produces expected results
- Reports match expected format
- CLI options work correctly
- Performance benchmarks meet targets

---

### Regression Testing

**Approach:**
1. Save v1.1 output for test PCAP files
2. Run v2.0 on same files
3. Compare outputs (with tolerance for enhancements)
4. Flag any missing or changed information

**Automated:**
```bash
# Regression test script
for pcap in test_data/*.pcap; do
    v1_output=$(v1.1/modbus-parser $pcap)
    v2_output=$(v2.0/protocol-parser --protocol modbus $pcap)
    compare_outputs.py "$v1_output" "$v2_output"
done
```

---

### Performance Testing

**Metrics:**
- Parse time per frame
- Total PCAP processing time
- Memory usage (peak and average)
- Report generation time

**Targets:**
- Parse time: < 1ms per frame
- Memory: < 100MB for 100k frames
- No significant regression vs v1.1

---

## Timeline

### Realistic Schedule (7 weeks)

```
Week 1: Phase 1 - Stabilization
├─ Days 1-2: Documentation sprint (README, ARCHITECTURE, MIGRATION)
├─ Days 3-4: Function documentation (Doxygen comments)
└─ Day 5: Code cleanup, testing, tag v1.1-stable

Week 2: Phase 2a - Framework Design
├─ Days 1-2: Plugin interface design
├─ Days 3-4: Logging framework implementation
└─ Day 5: Endianness helpers

Week 3: Phase 2b - Framework Completion
├─ Days 1-2: Analyzer framework
├─ Days 3-4: Unit tests for framework
└─ Day 5: Documentation, tag v2.0-alpha

Week 4: Phase 3a - PCCC Protocol Research
├─ Days 1-3: Protocol specification study
├─ Day 4: PCCC frame structure design
└─ Day 5: Parser skeleton

Week 5: Phase 3b - PCCC Implementation
├─ Days 1-2: PCCC parsing logic
├─ Days 3-4: PCCC analyzers
└─ Day 5: Testing, documentation, tag v2.0-beta-pccc

Week 6: Phase 4a - Modbus Migration
├─ Days 1-2: Port Modbus to plugin interface
├─ Days 3-4: Port security analyzers
└─ Day 5: Integration testing

Week 7: Phase 4b - Release Preparation
├─ Days 1-2: Regression testing
├─ Days 3-4: Documentation updates
└─ Day 5: Release v2.0, tag v2.0-release
```

### Optimistic Schedule (5 weeks)

Assumes:
- No major roadblocks
- Efficient framework design (minimal iteration)
- PCCC protocol well-understood

### Pessimistic Schedule (10 weeks)

Assumes:
- Framework design requires iteration
- PCCC protocol complexities
- Testing reveals issues requiring fixes

---

## Success Metrics

### Code Quality

- [ ] 100% compliance with C_CODING_STANDARD.md v2.0
- [ ] Zero compiler warnings (-Wall -Wextra)
- [ ] Clean static analysis (cppcheck, scan-build)
- [ ] All functions documented (Doxygen)

### Functionality

- [ ] Modbus parser: feature parity with v1.1
- [ ] PCCC parser: basic working implementation
- [ ] Framework: supports both protocols
- [ ] Analysis: enhanced over v1.1

### Testing

- [ ] Unit test coverage >80% for framework
- [ ] Integration tests for all features
- [ ] Regression tests pass
- [ ] Performance benchmarks meet targets

### Documentation

- [ ] Architecture documented
- [ ] API reference complete (Doxygen)
- [ ] Plugin development guide
- [ ] User migration guide

---

## Rollback Plan

### If Migration Fails

**Phase 1 Failure (unlikely):**
- Revert documentation changes
- Continue with v1.0
- No impact on functionality

**Phase 2 Failure (framework issues):**
- v1.1-stable remains usable
- Shelve v2.0 temporarily
- Revisit design

**Phase 3 Failure (PCCC issues):**
- v1.1-stable Modbus still works
- Delay PCCC, keep framework
- Learn from attempt

**Phase 4 Failure (Modbus migration issues):**
- v1.1-stable Modbus available
- v2.0-beta-pccc works for PCCC
- Coexist two versions temporarily

**Key Principle:** Always have a working release available.

---

## Appendices

### A. Standards Compliance Checklist

**Naming Conventions:**
- [ ] Functions: `module_verb_noun`
- [ ] Variables: `lowercase_underscores`
- [ ] Constants: `MODULE_NAME`
- [ ] Structs: `struct module_name`

**Documentation:**
- [ ] File headers with purpose, author, license
- [ ] Function documentation (Doxygen style)
- [ ] Complex algorithms explained
- [ ] Public API fully documented

**Code Organization:**
- [ ] Header guards: `MODULE_NAME_H`
- [ ] Includes ordered: system, library, local
- [ ] One public interface per module
- [ ] Private functions marked `static`

**Error Handling:**
- [ ] Check all allocations
- [ ] Validate input parameters
- [ ] Return meaningful error codes
- [ ] Clean up on error paths

**Logging:**
- [ ] Use pr_*() macros consistently
- [ ] Appropriate log levels
- [ ] Informative messages
- [ ] No excessive logging

**Testing:**
- [ ] Unit tests for all modules
- [ ] Edge case testing
- [ ] Error path testing
- [ ] Integration tests

### B. Related Documentation

**Implementation Guides:**
- [API.md](API.md) - v2.0 API reference (TBD)
- [CONTRIBUTING.md](CONTRIBUTING.md) - Development guide (TBD)
- [TESTING.md](TESTING.md) - Test strategy (TBD)

**Standards:**
- [C_CODING_STANDARD.md](C_CODING_STANDARD.md) - Code conventions
- [WRE_v2_0.md](WRE_v2_0.md) - Working practices

**Planning:**
- [Project_Analysis_and_Path_Forward.md](Project_Analysis_and_Path_Forward.md) - Strategic analysis

---

**Document Status:** Living document, updated as migration progresses  
**Next Review:** After Phase 1 completion  
**Feedback:** Open issues on GitHub or discuss in project meetings
