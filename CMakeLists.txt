cmake_minimum_required(VERSION 3.20)
project(modbus_parser C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Debug flags with warnings
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -Wall -Wextra")

# Find libpcap
find_path(PCAP_INCLUDE_DIR pcap.h PATHS /ucrt64/include)
find_library(PCAP_LIBRARY NAMES pcap wpcap PATHS /ucrt64/lib)

if(NOT PCAP_INCLUDE_DIR OR NOT PCAP_LIBRARY)
    message(FATAL_ERROR "libpcap not found. Install with: pacman -S mingw-w64-ucrt-x86_64-libpcap")
endif()

# Include directories
include_directories(${PCAP_INCLUDE_DIR})

# Source files
set(SOURCES
    src/main.c
    src/pcap_reader.c
    src/modbus_parser.c
)

# Create executable
add_executable(modbus_parser ${SOURCES})

# Link libpcap and required Windows libraries
target_link_libraries(modbus_parser ${PCAP_LIBRARY} ws2_32)
