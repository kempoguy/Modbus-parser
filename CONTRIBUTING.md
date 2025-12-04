# Contributing to Modbus Parser

**Version:** 1.0  
**Last Updated:** December 2, 2025

---

## Welcome

Thank you for considering contributing to the Modbus Parser project! This document provides guidelines for contributing code, documentation, bug reports, and feature requests.

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Development Setup](#development-setup)
3. [Coding Standards](#coding-standards)
4. [Contribution Workflow](#contribution-workflow)
5. [Testing Requirements](#testing-requirements)
6. [Documentation](#documentation)
7. [Commit Messages](#commit-messages)
8. [Pull Request Process](#pull-request-process)
9. [Bug Reports](#bug-reports)
10. [Feature Requests](#feature-requests)

---

## Getting Started

### What Can You Contribute?

**Code Contributions:**
- Bug fixes
- Performance improvements
- Additional function code support
- Platform compatibility fixes
- Memory leak fixes

**Documentation:**
- README improvements
- Example code
- Tutorial content
- API clarifications
- Typo corrections

**Testing:**
- PCAP test cases
- Bug reports with reproducible steps
- Platform testing results
- Performance benchmarking

**Design Input:**
- v2.0 architecture feedback
- Plugin interface suggestions
- Security analysis improvements

### First-Time Contributors

**Good First Issues:**
- Documentation improvements
- Adding function code names
- Code comment additions
- Example program creation
- Test PCAP file contributions

**Learning Resources:**
- [README.md](README.md) - Project overview
- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
- [API.md](API.md) - API reference
- [C_CODING_STANDARD.md](C_CODING_STANDARD.md) - Code style

---

## Development Setup

### Prerequisites

**Required Tools:**
```bash
# Compiler
gcc --version       # GCC 13+ or Clang 15+

# Build System
cmake --version     # CMake 3.20+

# Library
# MSYS2
pacman -S mingw-w64-ucrt-x86_64-libpcap

# Linux
sudo apt install libpcap-dev  # Debian/Ubuntu
```

**Optional Tools:**
```bash
# Static Analysis
cppcheck --version

# Documentation
doxygen --version

# Debugging
gdb --version
```

### Repository Setup

```bash
# Clone repository
git clone <repository-url>
cd modbus-parser

# Create development branch
git checkout -b feature/your-feature-name

# Build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Test
./modbus-parser ../test_data/sample.pcap
```

### Development Build

```bash
# Debug build with all warnings
mkdir build-dev
cd build-dev
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_FLAGS="-Wall -Wextra -Werror" \
      ..
cmake --build .

# Run static analysis
cppcheck --enable=all --suppress=missingIncludeSystem \
         ../src/*.c 2> cppcheck_report.txt
```

---

## Coding Standards

### Read This First

**Primary Reference:** [C_CODING_STANDARD.md](C_CODING_STANDARD.md)

All code contributions must follow the coding standard. Key points:

**Naming:**
```c
// Functions: module_verb_noun
bool modbus_parse_frame(...)
void pcap_process_file(...)

// Variables: lowercase_underscores
uint32_t frame_count;
const char *src_ip;

// Constants: MODULE_NAME
#define MODBUS_DEFAULT_PORT 502
```

**Documentation:**
```c
/**
 * function_name() - Brief description
 * @param1: Description of param1
 * @param2: Description of param2
 *
 * Detailed description of function behavior,
 * preconditions, postconditions, etc.
 *
 * Return: Description of return value
 */
int function_name(type param1, type param2)
{
    // Implementation
}
```

**Error Handling:**
```c
// Check all allocations
data = malloc(size);
if (!data) {
    fprintf(stderr, "Allocation failed\n");
    return -1;
}

// Validate parameters
if (payload == NULL || length < MIN_SIZE) {
    return false;
}

// Clean up on error paths
if (error_condition) {
    free(allocated_data);
    return false;
}
```

### Code Style Checklist

Before submitting:
- [ ] Follows naming conventions
- [ ] All functions documented
- [ ] No compiler warnings (-Wall -Wextra)
- [ ] No magic numbers (use named constants)
- [ ] Error handling on all allocations
- [ ] Memory leaks checked
- [ ] Consistent indentation (tabs/spaces per project)
- [ ] Max line length 80-100 characters

---

## Contribution Workflow

### Step-by-Step Process

**1. Identify Work**
- Check existing issues
- Discuss larger changes first
- Ensure no duplicate effort

**2. Create Branch**
```bash
git checkout -b feature/descriptive-name
# OR
git checkout -b fix/bug-description
```

**3. Make Changes**
- Write code following standards
- Add/update documentation
- Add tests if applicable

**4. Test Locally**
```bash
# Build
cmake --build build

# Test with various PCAP files
./build/modbus-parser test1.pcap
./build/modbus-parser test2.pcap

# Check for memory leaks (Linux)
valgrind --leak-check=full ./build/modbus-parser test.pcap
```

**5. Commit Changes**
```bash
git add <files>
git commit -m "type: brief description"
# See commit message guidelines below
```

**6. Push and Create PR**
```bash
git push origin feature/descriptive-name
# Create pull request on GitHub
```

---

## Testing Requirements

### Manual Testing

**Minimum Test Cases:**
```bash
# Normal traffic
./modbus-parser normal_polling.pcap

# Exception responses
./modbus-parser exceptions.pcap

# Verbose mode
./modbus-parser -v normal_polling.pcap

# Report generation
./modbus-parser -r normal_polling.pcap
# Check: <filename>_analysis.md created and valid
```

**Platform Testing:**
- Windows (MSYS2)
- Linux (Ubuntu/Fedora)
- macOS (if available)

### Test PCAP Files

**Required Characteristics:**
- Valid PCAP format
- Contains Modbus TCP (port 502)
- At least 10 frames
- Mix of function codes
- Include both requests and responses

**Creating Test Files:**
```bash
# Extract from larger capture
tcpdump -r large.pcap -w test.pcap 'tcp port 502'

# Validate in Wireshark
wireshark test.pcap
# Filter: tcp.port == 502
```

### Automated Testing (v2.0)

Future testing infrastructure:
```bash
# Unit tests
cd build
ctest

# Regression tests
./tests/run_regression.sh
```

---

## Documentation

### When to Update Documentation

**Always Update:**
- Adding new functions → Update API.md
- Changing behavior → Update README.md, ARCHITECTURE.md
- New features → Update README.md features section
- Bug fixes → Add to changelog/release notes

**Documentation Files:**

| File | Update When... |
|------|----------------|
| README.md | User-facing changes |
| ARCHITECTURE.md | Design changes |
| API.md | API additions/changes |
| MIGRATION.md | Breaking changes |
| C_CODING_STANDARD.md | Style changes |

### Documentation Style

**Markdown Files:**
- Use ATX-style headers (`#`, `##`, `###`)
- Code blocks with language tags (```c, ```bash)
- Tables for structured data
- Bullet lists for items
- Numbered lists for procedures

**Code Comments:**
```c
/**
 * Doxygen-style comments for functions
 * Brief description on first line
 *
 * Detailed description follows blank line
 */

// Single-line comments for implementation notes
// Explain WHY, not WHAT (code shows what)

/* Multi-line comments for complex algorithms */
```

---

## Commit Messages

### Format

```
type: brief description (50 chars max)

Detailed explanation of changes if needed. Wrap at 72 characters.
Explain what and why, not how (code shows how).

Fixes: #123
Related: #456
```

### Commit Types

| Type | Usage |
|------|-------|
| `feat` | New feature |
| `fix` | Bug fix |
| `docs` | Documentation only |
| `style` | Code formatting (no logic change) |
| `refactor` | Code restructuring (no behavior change) |
| `perf` | Performance improvement |
| `test` | Adding tests |
| `chore` | Build/tooling changes |

### Examples

**Good:**
```
feat: add support for function code 0x2B

Implements parsing and display for Encapsulated Interface Transport
function. Tested with real PCAP containing MEI requests.

Closes: #42
```

**Good:**
```
fix: memory leak in modbus_parse_frame()

frame->data was not freed on protocol_id validation failure.
Added free() before early return.

Fixes: #78
```

**Bad:**
```
Update code
```

**Bad:**
```
Fixed stuff and improved things
```

---

## Pull Request Process

### Before Submitting

**Checklist:**
- [ ] Code follows C_CODING_STANDARD.md
- [ ] All functions documented
- [ ] Compiles without warnings
- [ ] Tested on at least one platform
- [ ] Documentation updated
- [ ] Commits follow message format
- [ ] No unnecessary files (build artifacts, etc.)

### PR Description Template

```markdown
## Description
Brief summary of changes

## Motivation
Why is this change needed?

## Changes Made
- List specific changes
- Include any breaking changes

## Testing
- [ ] Tested on Windows/Linux/macOS
- [ ] Tested with <specific PCAP files>
- [ ] No memory leaks (valgrind clean)

## Documentation
- [ ] README.md updated
- [ ] API.md updated (if applicable)
- [ ] Function comments added

## Related Issues
Closes: #123
Related: #456
```

### Review Process

**What to Expect:**
1. Automated checks (if CI enabled)
2. Code review by maintainer
3. Feedback and requested changes
4. Approval and merge

**Review Criteria:**
- Correctness
- Code quality
- Documentation completeness
- Test coverage
- Performance impact

### After Merge

- Delete your branch
- Pull latest main/master
- Start next contribution from updated main

---

## Bug Reports

### Before Reporting

**Check First:**
1. Is this a known issue? (search existing issues)
2. Are you using the latest version?
3. Can you reproduce consistently?

### Bug Report Template

```markdown
## Bug Description
Clear description of the bug

## Steps to Reproduce
1. Run command: `./modbus-parser <file>`
2. Expected: <expected behavior>
3. Actual: <actual behavior>

## Environment
- OS: Windows 10 / Ubuntu 22.04 / macOS 13
- Compiler: GCC 13.1 / Clang 15.0
- libpcap version: 1.10.4
- Build type: Debug / Release

## PCAP File
- [ ] Attached (if shareable)
- [ ] Describe traffic if not shareable

## Additional Context
- Error messages
- Screenshots
- Crash dumps
```

### Providing PCAP Files

**If Shareable:**
- Attach to issue
- Or provide download link

**If Not Shareable:**
- Describe traffic pattern
- Provide hex dump of problematic frames
- Explain security/privacy concerns

---

## Feature Requests

### Before Requesting

**Consider:**
1. Is this aligned with project goals?
2. Could this be done as an external tool?
3. Is this planned for v2.0? (check MIGRATION.md)

### Feature Request Template

```markdown
## Feature Description
Clear description of proposed feature

## Use Case
Why is this needed? What problem does it solve?

## Proposed Solution
How might this be implemented?

## Alternatives Considered
Other approaches, why not sufficient?

## Additional Context
- Examples from other tools
- Relevant standards/specifications
- Related issues
```

### Feature Evaluation Criteria

**Likely to Accept:**
- Aligns with project goals (OT protocol analysis)
- Broadly useful to users
- Reasonable implementation complexity
- Well-defined scope

**Likely to Defer:**
- Better suited for v2.0 (architectural changes)
- Niche use case
- High complexity, low value
- Overlaps with existing functionality

---

## Project-Specific Guidelines

### v1.0 vs v2.0 Contributions

**v1.0 (Current):**
- Bug fixes: Always welcome
- Small improvements: Welcome
- New features: Consider carefully (monolithic architecture)
- Major refactoring: Wait for v2.0

**v2.0 (Planned):**
- Framework contributions: Very welcome
- Plugin development: Essential
- Architecture feedback: Critical
- Test infrastructure: High priority

### Industrial Protocol Knowledge

**Helpful but Not Required:**
- Modbus specification understanding
- PCAP analysis experience
- OT/SCADA familiarity

**We Can Help:**
- Protocol questions
- PCAP interpretation
- Security analysis concepts

### Communication

**Channels:**
- GitHub Issues: Bug reports, feature requests
- GitHub Discussions: Design questions, general help
- Pull Request comments: Code-specific discussion

**Response Time:**
- Issues: Within 1 week
- Pull requests: Within 2 weeks
- Complex questions: May take longer

---

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (see LICENSE file).

---

## Code of Conduct

### Our Standards

**We Value:**
- Respectful communication
- Constructive feedback
- Collaborative problem-solving
- Learning and teaching

**Not Tolerated:**
- Harassment or discrimination
- Unconstructive criticism
- Spam or off-topic content

**If Issues Arise:**
- Report to project maintainers
- Private communication if needed
- We'll address promptly and fairly

---

## Getting Help

**Need Assistance?**
- Check [README.md](README.md) first
- Read [API.md](API.md) for API questions
- Search existing issues
- Ask in GitHub Discussions
- Create new issue with "question" label

**Development Questions:**
- Architecture: See ARCHITECTURE.md
- Coding style: See C_CODING_STANDARD.md
- Building: See README.md
- Testing: See this document

---

## Recognition

### Contributors

All contributors will be acknowledged in:
- README.md contributors section
- Release notes for their contributions
- Git commit history

### Significant Contributions

Major contributions may result in:
- Maintainer status
- Decision-making participation
- Feature ownership

---

## Thank You

Every contribution, no matter how small, helps improve this project. We appreciate your time and effort in making the Modbus Parser better!

**Happy Contributing!**

---

**Document Version:** 1.0  
**Last Updated:** December 2, 2025  
**Maintained by:** Project maintainers
