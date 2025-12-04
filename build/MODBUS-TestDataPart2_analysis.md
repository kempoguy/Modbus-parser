# Modbus TCP Security Analysis Report

**Generated:** 2025-12-04 13:44:36  
**Source File:** `./MODBUS-TestDataPart2.pcap`  

---

## Traffic Summary

| Packet | Timestamp | Source | Destination | Trans ID | Unit | Function | Details |
|--------|-----------|--------|-------------|----------|------|----------|---------|
| 1 | 02:24:52.212845 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 2 | 02:24:52.783649 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x00: Unknown/Reserved Function) |
| 3 | 02:24:52.984181 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=0, Qty=0 |
| 4 | 02:24:53.492403 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x01: Read Coils) |
| 5 | 02:24:53.519653 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Discrete Inputs | Addr=0, Qty=0 |
| 6 | 02:24:53.990688 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x02: Read Discrete Inputs) |
| 7 | 02:24:54.045123 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Holding Registers | Addr=0, Qty=0 |
| 8 | 02:24:54.576738 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x03: Read Holding Registers) |
| 9 | 02:24:54.577603 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Input Registers | Addr=0, Qty=0 |
| 10 | 02:24:55.256016 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x04: Read Input Registers) |
| 11 | 02:24:55.357835 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Write Single Coil | Addr=0, Qty=0 |
| 12 | 02:24:55.832303 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Write Single Coil | Addr=0, Qty=0 |
| 13 | 02:24:55.888998 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Write Single Register | Addr=0, Qty=0 |
| 14 | 02:24:56.426682 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x06: Write Single Register) |
| 15 | 02:24:56.670247 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Exception Status | - |
| 16 | 02:24:57.165539 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Read Exception Status | - |
| 17 | 02:24:57.200517 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Diagnostic | - |
| 18 | 02:24:57.712498 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Diagnostic | - |
| 19 | 02:24:57.732438 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Program Controller (Obsolete) | - |
| 20 | 02:24:58.211477 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x09: Program Controller (Obsolete)) |
| 21 | 02:24:58.264236 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Poll Controller (Obsolete) | - |
| 22 | 02:24:58.744596 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x0A: Poll Controller (Obsolete)) |
| 23 | 02:24:58.794047 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Get Comm Event Counter | - |
| 24 | 02:24:59.290882 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x0B: Get Comm Event Counter) |
| 25 | 02:24:59.326457 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Get Comm Event Log | - |
| 26 | 02:24:59.834549 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x0C: Get Comm Event Log) |
| 27 | 02:24:59.857902 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Program 484 (Obsolete) | - |
| 28 | 02:25:00.420690 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x0D: Program 484 (Obsolete)) |
| 29 | 02:25:00.638972 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Poll 484 (Obsolete) | - |
| 30 | 02:25:01.100048 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x0E: Poll 484 (Obsolete)) |
| 31 | 02:25:01.173171 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Write Multiple Coils | Addr=0, Qty=0 |
| 32 | 02:25:01.676647 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x0F: Write Multiple Coils) |
| 33 | 02:25:01.700544 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Write Multiple Registers | Addr=0, Qty=0 |
| 34 | 02:25:02.236242 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x10: Write Multiple Registers) |
| 35 | 02:25:02.482474 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Report Server ID | - |
| 36 | 02:25:03.170618 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x11: Report Server ID) |
| 37 | 02:25:03.264410 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read General Reference (Obsolete) | - |
| 38 | 02:25:03.722404 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x12: Read General Reference (Obsolete)) |
| 39 | 02:25:03.795763 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Write General Reference (Obsolete) | - |
| 40 | 02:25:04.280006 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x13: Write General Reference (Obsolete)) |
| 41 | 02:25:04.335458 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read File Record | - |
| 42 | 02:25:04.958574 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x14: Read File Record) |
| 43 | 02:25:05.106429 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Write File Record | - |
| 44 | 02:25:05.606024 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x15: Write File Record) |
| 45 | 02:25:05.638731 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Mask Write Register | - |
| 46 | 02:25:06.126921 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x16: Mask Write Register) |
| 47 | 02:25:06.170715 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read/Write Multiple Registers | - |
| 48 | 02:25:06.665323 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x17: Read/Write Multiple Registers) |
| 49 | 02:25:06.700712 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read FIFO Queue | - |
| 50 | 02:25:07.202989 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x18: Read FIFO Queue) |
| 51 | 02:25:07.251986 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 52 | 02:25:07.721091 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x19: Unknown/Reserved Function) |
| 53 | 02:25:07.779520 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 54 | 02:25:08.288043 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x1A: Unknown/Reserved Function) |
| 55 | 02:25:08.312844 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 56 | 02:25:08.794212 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x1B: Unknown/Reserved Function) |
| 57 | 02:25:08.841075 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 58 | 02:25:09.414124 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x1C: Unknown/Reserved Function) |
| 59 | 02:25:09.623426 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 60 | 02:25:10.093489 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x1D: Unknown/Reserved Function) |
| 61 | 02:25:10.154892 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 62 | 02:25:10.705044 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x1E: Unknown/Reserved Function) |
| 63 | 02:25:10.936369 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 64 | 02:25:11.416718 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x1F: Unknown/Reserved Function) |
| 65 | 02:25:11.467298 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 66 | 02:25:11.976794 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x20: Unknown/Reserved Function) |
| 67 | 02:25:11.997263 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 68 | 02:25:12.494611 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x21: Unknown/Reserved Function) |
| 69 | 02:25:12.529443 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 70 | 02:25:13.013725 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x22: Unknown/Reserved Function) |
| 71 | 02:25:13.060709 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 72 | 02:25:13.563241 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x23: Unknown/Reserved Function) |
| 73 | 02:25:13.591900 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 74 | 02:25:14.102765 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x24: Unknown/Reserved Function) |
| 75 | 02:25:14.123322 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 76 | 02:25:14.609591 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x25: Unknown/Reserved Function) |
| 77 | 02:25:14.654715 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 78 | 02:25:15.148323 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x26: Unknown/Reserved Function) |
| 79 | 02:25:15.185519 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 80 | 02:25:15.666316 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x27: Unknown/Reserved Function) |
| 81 | 02:25:15.717502 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 82 | 02:25:16.246100 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 83 | 02:25:16.247647 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 84 | 02:25:16.839806 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x29: Unknown/Reserved Function) |
| 85 | 02:25:17.028222 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 86 | 02:25:17.510279 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 87 | 02:25:17.561063 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Encapsulated Interface Transport | - |
| 88 | 02:25:18.097522 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x2B: Encapsulated Interface Transport) |
| 89 | 02:25:18.341875 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 90 | 02:25:18.886018 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x2C: Unknown/Reserved Function) |
| 91 | 02:25:19.123075 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 92 | 02:25:19.637518 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x2D: Unknown/Reserved Function) |
| 93 | 02:25:19.653353 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 94 | 02:25:20.132919 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x2E: Unknown/Reserved Function) |
| 95 | 02:25:20.185847 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 96 | 02:25:20.661039 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x2F: Unknown/Reserved Function) |
| 97 | 02:25:20.716707 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 98 | 02:25:21.200303 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x30: Unknown/Reserved Function) |
| 99 | 02:25:21.247668 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 100 | 02:25:21.780850 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x31: Unknown/Reserved Function) |
| 101 | 02:25:22.028628 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 102 | 02:25:22.516424 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x32: Unknown/Reserved Function) |
| 103 | 02:25:22.560815 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 104 | 02:25:23.075892 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x33: Unknown/Reserved Function) |
| 105 | 02:25:23.091742 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 106 | 02:25:23.568356 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x34: Unknown/Reserved Function) |
| 107 | 02:25:23.623460 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 108 | 02:25:24.126967 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x35: Unknown/Reserved Function) |
| 109 | 02:25:24.155055 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 110 | 02:25:24.737832 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x36: Unknown/Reserved Function) |
| 111 | 02:25:24.934600 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 112 | 02:25:25.426974 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x37: Unknown/Reserved Function) |
| 113 | 02:25:25.467348 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 114 | 02:25:25.969700 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x38: Unknown/Reserved Function) |
| 115 | 02:25:25.998259 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 116 | 02:25:26.501101 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x39: Unknown/Reserved Function) |
| 117 | 02:25:26.528817 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 118 | 02:25:27.019176 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x3A: Unknown/Reserved Function) |
| 119 | 02:25:27.060548 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 120 | 02:25:27.553318 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x3B: Unknown/Reserved Function) |
| 121 | 02:25:27.590910 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 122 | 02:25:28.090342 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x3C: Unknown/Reserved Function) |
| 123 | 02:25:28.121865 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 124 | 02:25:28.626285 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x3D: Unknown/Reserved Function) |
| 125 | 02:25:28.654478 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 126 | 02:25:29.144399 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x3E: Unknown/Reserved Function) |
| 127 | 02:25:29.186068 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 128 | 02:25:29.663558 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x3F: Unknown/Reserved Function) |
| 129 | 02:25:29.715909 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 130 | 02:25:30.380543 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x40: Unknown/Reserved Function) |
| 131 | 02:25:30.498619 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 132 | 02:25:30.955008 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x41: Unknown/Reserved Function) |
| 133 | 02:25:31.030108 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 134 | 02:25:31.508232 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x42: Unknown/Reserved Function) |
| 135 | 02:25:31.560523 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 136 | 02:25:32.064038 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x43: Unknown/Reserved Function) |
| 137 | 02:25:32.091769 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 138 | 02:25:32.617245 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x44: Unknown/Reserved Function) |
| 139 | 02:25:32.622756 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 140 | 02:25:33.217551 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x45: Unknown/Reserved Function) |
| 141 | 02:25:33.406960 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 142 | 02:25:33.898173 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x46: Unknown/Reserved Function) |
| 143 | 02:25:33.937591 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 144 | 02:25:34.493195 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x47: Unknown/Reserved Function) |
| 145 | 02:25:34.716747 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 146 | 02:25:35.212568 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x48: Unknown/Reserved Function) |
| 147 | 02:25:35.247819 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 148 | 02:25:35.750950 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x49: Unknown/Reserved Function) |
| 149 | 02:25:35.778655 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 150 | 02:25:36.289846 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x4A: Unknown/Reserved Function) |
| 151 | 02:25:36.316627 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 152 | 02:25:36.820614 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x4B: Unknown/Reserved Function) |
| 153 | 02:25:36.841480 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 154 | 02:25:37.326833 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x4C: Unknown/Reserved Function) |
| 155 | 02:25:37.373899 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 156 | 02:25:37.875669 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x4D: Unknown/Reserved Function) |
| 157 | 02:25:37.903078 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 158 | 02:25:38.393501 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x4E: Unknown/Reserved Function) |
| 159 | 02:25:38.435422 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 160 | 02:25:38.983494 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x4F: Unknown/Reserved Function) |
| 161 | 02:25:39.215614 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 162 | 02:25:39.720696 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x50: Unknown/Reserved Function) |
| 163 | 02:25:39.748032 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 164 | 02:25:40.242679 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x51: Unknown/Reserved Function) |
| 165 | 02:25:40.280427 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 166 | 02:25:40.778208 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x52: Unknown/Reserved Function) |
| 167 | 02:25:40.810125 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 168 | 02:25:41.311599 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x53: Unknown/Reserved Function) |
| 169 | 02:25:41.341975 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 170 | 02:25:41.851939 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x54: Unknown/Reserved Function) |
| 171 | 02:25:41.875446 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 172 | 02:25:42.402905 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x55: Unknown/Reserved Function) |
| 173 | 02:25:42.405601 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 174 | 02:25:42.948265 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x56: Unknown/Reserved Function) |
| 175 | 02:25:43.186144 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 176 | 02:25:43.639225 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x57: Unknown/Reserved Function) |
| 177 | 02:25:43.717171 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 178 | 02:25:44.200324 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x58: Unknown/Reserved Function) |
| 179 | 02:25:44.248246 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 180 | 02:25:44.731019 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x59: Unknown/Reserved Function) |
| 181 | 02:25:44.779340 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 182 | 02:25:45.309828 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x5A: Unknown/Reserved Function) |
| 183 | 02:25:45.310425 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 184 | 02:25:45.801031 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x5B: Unknown/Reserved Function) |
| 185 | 02:25:45.841206 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 186 | 02:25:46.326314 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x5C: Unknown/Reserved Function) |
| 187 | 02:25:46.372811 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 188 | 02:25:46.881362 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x5D: Unknown/Reserved Function) |
| 189 | 02:25:46.903162 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 190 | 02:25:47.379569 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x5E: Unknown/Reserved Function) |
| 191 | 02:25:47.438777 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 192 | 02:25:47.917181 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x5F: Unknown/Reserved Function) |
| 193 | 02:25:47.967051 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 194 | 02:25:48.522537 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x60: Unknown/Reserved Function) |
| 195 | 02:25:48.748473 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 196 | 02:25:49.242449 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x61: Unknown/Reserved Function) |
| 197 | 02:25:49.279423 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 198 | 02:25:49.786175 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x62: Unknown/Reserved Function) |
| 199 | 02:25:49.810436 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 200 | 02:25:50.318859 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x63: Unknown/Reserved Function) |
| 201 | 02:25:50.341221 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 202 | 02:25:50.839948 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x64: Unknown/Reserved Function) |
| 203 | 02:25:50.875912 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 204 | 02:25:51.355365 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x65: Unknown/Reserved Function) |
| 205 | 02:25:51.406902 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 206 | 02:25:51.927690 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x66: Unknown/Reserved Function) |
| 207 | 02:25:51.935928 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 208 | 02:25:52.439603 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x67: Unknown/Reserved Function) |
| 209 | 02:25:52.466630 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 210 | 02:25:52.967534 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x68: Unknown/Reserved Function) |
| 211 | 02:25:52.998681 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 212 | 02:25:53.481426 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x69: Unknown/Reserved Function) |
| 213 | 02:25:53.529702 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 214 | 02:25:54.018321 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x6A: Unknown/Reserved Function) |
| 215 | 02:25:54.061161 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 216 | 02:25:54.733392 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x6B: Unknown/Reserved Function) |
| 217 | 02:25:54.841907 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 218 | 02:25:55.355329 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x6C: Unknown/Reserved Function) |
| 219 | 02:25:55.372853 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 220 | 02:25:55.910090 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x6D: Unknown/Reserved Function) |
| 221 | 02:25:56.154536 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 222 | 02:25:56.642231 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x6E: Unknown/Reserved Function) |
| 223 | 02:25:56.685825 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 224 | 02:25:57.266232 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x6F: Unknown/Reserved Function) |
| 225 | 02:25:57.466866 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 226 | 02:25:58.032310 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x70: Unknown/Reserved Function) |
| 227 | 02:25:58.247705 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 228 | 02:25:58.726774 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x71: Unknown/Reserved Function) |
| 229 | 02:25:58.779633 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 230 | 02:25:59.440579 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x72: Unknown/Reserved Function) |
| 231 | 02:25:59.560630 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 232 | 02:26:00.040220 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x73: Unknown/Reserved Function) |
| 233 | 02:26:00.092684 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 234 | 02:26:00.610615 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x74: Unknown/Reserved Function) |
| 235 | 02:26:00.622678 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 236 | 02:26:01.149960 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x75: Unknown/Reserved Function) |
| 237 | 02:26:01.154943 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 238 | 02:26:01.618613 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x76: Unknown/Reserved Function) |
| 239 | 02:26:01.686229 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 240 | 02:26:02.181216 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x77: Unknown/Reserved Function) |
| 241 | 02:26:02.217101 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 242 | 02:26:02.688996 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x78: Unknown/Reserved Function) |
| 243 | 02:26:02.748615 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 244 | 02:26:03.226712 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x79: Unknown/Reserved Function) |
| 245 | 02:26:03.287103 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 246 | 02:26:03.781229 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x7A: Unknown/Reserved Function) |
| 247 | 02:26:03.810807 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 248 | 02:26:04.324461 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x7B: Unknown/Reserved Function) |
| 249 | 02:26:04.341227 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 250 | 02:26:04.870228 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x7C: Unknown/Reserved Function) |
| 251 | 02:26:04.874327 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 252 | 02:26:05.346071 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 253 | 02:26:05.406871 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 254 | 02:26:05.897322 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Value (FC=0x7E: Unknown/Reserved Function) |
| 255 | 02:26:05.935638 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Unknown/Reserved Function | - |
| 256 | 02:26:06.425892 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Function (FC=0x7F: Unknown/Reserved Function) |
| 257 | 02:26:06.972800 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=0, Qty=8 |
| 258 | 02:26:07.462158 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Read Coils | - |
| 259 | 02:26:07.500047 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=0, Qty=1 |
| 260 | 02:26:08.001262 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Read Coils | - |
| 261 | 02:26:08.029848 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=65535, Qty=1 |
| 262 | 02:26:08.505228 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 263 | 02:26:08.560348 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=32767, Qty=1 |
| 264 | 02:26:09.106358 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 265 | 02:26:09.341789 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=16383, Qty=1 |
| 266 | 02:26:09.807100 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 267 | 02:26:09.875500 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=8191, Qty=1 |
| 268 | 02:26:10.362597 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 269 | 02:26:10.406438 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=4095, Qty=1 |
| 270 | 02:26:10.940211 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 271 | 02:26:11.185636 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=2047, Qty=1 |
| 272 | 02:26:11.734925 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 273 | 02:26:11.967092 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=1023, Qty=1 |
| 274 | 02:26:12.448954 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Read Coils | - |
| 275 | 02:26:12.498222 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=1535, Qty=1 |
| 276 | 02:26:13.009668 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Read Coils | - |
| 277 | 02:26:13.029056 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=1791, Qty=1 |
| 278 | 02:26:13.499917 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 279 | 02:26:13.561177 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=1663, Qty=1 |
| 280 | 02:26:14.078419 | 166.161.16.230:502 | 192.168.66.235:2582 | 0x0000 | 0x01 | Exception Response | Exception: Illegal Data Address (FC=0x01: Read Coils) |
| 281 | 02:26:14.091152 | 192.168.66.235:2582 | 166.161.16.230:502 | 0x0000 | 0x01 | Read Coils | Addr=1599, Qty=1 |

---

## Security Analysis

### Exception Rate Analysis

- **Total Frames:** 281
- **Exception Responses:** 130 (46.3%)

### Threat Indicators

- ⚠️ **SEQUENTIAL PROBING** - Function codes tested in sequence
- ⚠️ **BROAD ENUMERATION** - 128 different function codes tested

### Timing Analysis

- **Total Duration:** 81.88 seconds
- **Average Frame Rate:** 3.43 frames/second
- **Rapid Bursts:** 112 frames (< 0.1s apart)


### Function Code Summary

**Unique functions tested:** 128

| Function Code | Function Name | Count |
|---------------|---------------|-------|
| 0x00 | Unknown/Reserved Function | 1 |
| 0x01 | Read Coils | 18 |
| 0x02 | Read Discrete Inputs | 1 |
| 0x03 | Read Holding Registers | 1 |
| 0x04 | Read Input Registers | 1 |
| 0x05 | Write Single Coil | 2 |
| 0x06 | Write Single Register | 1 |
| 0x07 | Read Exception Status | 2 |
| 0x08 | Diagnostic | 2 |
| 0x09 | Program Controller (Obsolete) | 1 |
| 0x0A | Poll Controller (Obsolete) | 1 |
| 0x0B | Get Comm Event Counter | 1 |
| 0x0C | Get Comm Event Log | 1 |
| 0x0D | Program 484 (Obsolete) | 1 |
| 0x0E | Poll 484 (Obsolete) | 1 |
| 0x0F | Write Multiple Coils | 1 |
| 0x10 | Write Multiple Registers | 1 |
| 0x11 | Report Server ID | 1 |
| 0x12 | Read General Reference (Obsolete) | 1 |
| 0x13 | Write General Reference (Obsolete) | 1 |
| 0x14 | Read File Record | 1 |
| 0x15 | Write File Record | 1 |
| 0x16 | Mask Write Register | 1 |
| 0x17 | Read/Write Multiple Registers | 1 |
| 0x18 | Read FIFO Queue | 1 |
| 0x19 | Unknown/Reserved Function | 1 |
| 0x1A | Unknown/Reserved Function | 1 |
| 0x1B | Unknown/Reserved Function | 1 |
| 0x1C | Unknown/Reserved Function | 1 |
| 0x1D | Unknown/Reserved Function | 1 |
| 0x1E | Unknown/Reserved Function | 1 |
| 0x1F | Unknown/Reserved Function | 1 |
| 0x20 | Unknown/Reserved Function | 1 |
| 0x21 | Unknown/Reserved Function | 1 |
| 0x22 | Unknown/Reserved Function | 1 |
| 0x23 | Unknown/Reserved Function | 1 |
| 0x24 | Unknown/Reserved Function | 1 |
| 0x25 | Unknown/Reserved Function | 1 |
| 0x26 | Unknown/Reserved Function | 1 |
| 0x27 | Unknown/Reserved Function | 1 |
| 0x28 | Unknown/Reserved Function | 2 |
| 0x29 | Unknown/Reserved Function | 1 |
| 0x2A | Unknown/Reserved Function | 2 |
| 0x2B | Encapsulated Interface Transport | 1 |
| 0x2C | Unknown/Reserved Function | 1 |
| 0x2D | Unknown/Reserved Function | 1 |
| 0x2E | Unknown/Reserved Function | 1 |
| 0x2F | Unknown/Reserved Function | 1 |
| 0x30 | Unknown/Reserved Function | 1 |
| 0x31 | Unknown/Reserved Function | 1 |
| 0x32 | Unknown/Reserved Function | 1 |
| 0x33 | Unknown/Reserved Function | 1 |
| 0x34 | Unknown/Reserved Function | 1 |
| 0x35 | Unknown/Reserved Function | 1 |
| 0x36 | Unknown/Reserved Function | 1 |
| 0x37 | Unknown/Reserved Function | 1 |
| 0x38 | Unknown/Reserved Function | 1 |
| 0x39 | Unknown/Reserved Function | 1 |
| 0x3A | Unknown/Reserved Function | 1 |
| 0x3B | Unknown/Reserved Function | 1 |
| 0x3C | Unknown/Reserved Function | 1 |
| 0x3D | Unknown/Reserved Function | 1 |
| 0x3E | Unknown/Reserved Function | 1 |
| 0x3F | Unknown/Reserved Function | 1 |
| 0x40 | Unknown/Reserved Function | 1 |
| 0x41 | Unknown/Reserved Function | 1 |
| 0x42 | Unknown/Reserved Function | 1 |
| 0x43 | Unknown/Reserved Function | 1 |
| 0x44 | Unknown/Reserved Function | 1 |
| 0x45 | Unknown/Reserved Function | 1 |
| 0x46 | Unknown/Reserved Function | 1 |
| 0x47 | Unknown/Reserved Function | 1 |
| 0x48 | Unknown/Reserved Function | 1 |
| 0x49 | Unknown/Reserved Function | 1 |
| 0x4A | Unknown/Reserved Function | 1 |
| 0x4B | Unknown/Reserved Function | 1 |
| 0x4C | Unknown/Reserved Function | 1 |
| 0x4D | Unknown/Reserved Function | 1 |
| 0x4E | Unknown/Reserved Function | 1 |
| 0x4F | Unknown/Reserved Function | 1 |
| 0x50 | Unknown/Reserved Function | 1 |
| 0x51 | Unknown/Reserved Function | 1 |
| 0x52 | Unknown/Reserved Function | 1 |
| 0x53 | Unknown/Reserved Function | 1 |
| 0x54 | Unknown/Reserved Function | 1 |
| 0x55 | Unknown/Reserved Function | 1 |
| 0x56 | Unknown/Reserved Function | 1 |
| 0x57 | Unknown/Reserved Function | 1 |
| 0x58 | Unknown/Reserved Function | 1 |
| 0x59 | Unknown/Reserved Function | 1 |
| 0x5A | Unknown/Reserved Function | 1 |
| 0x5B | Unknown/Reserved Function | 1 |
| 0x5C | Unknown/Reserved Function | 1 |
| 0x5D | Unknown/Reserved Function | 1 |
| 0x5E | Unknown/Reserved Function | 1 |
| 0x5F | Unknown/Reserved Function | 1 |
| 0x60 | Unknown/Reserved Function | 1 |
| 0x61 | Unknown/Reserved Function | 1 |
| 0x62 | Unknown/Reserved Function | 1 |
| 0x63 | Unknown/Reserved Function | 1 |
| 0x64 | Unknown/Reserved Function | 1 |
| 0x65 | Unknown/Reserved Function | 1 |
| 0x66 | Unknown/Reserved Function | 1 |
| 0x67 | Unknown/Reserved Function | 1 |
| 0x68 | Unknown/Reserved Function | 1 |
| 0x69 | Unknown/Reserved Function | 1 |
| 0x6A | Unknown/Reserved Function | 1 |
| 0x6B | Unknown/Reserved Function | 1 |
| 0x6C | Unknown/Reserved Function | 1 |
| 0x6D | Unknown/Reserved Function | 1 |
| 0x6E | Unknown/Reserved Function | 1 |
| 0x6F | Unknown/Reserved Function | 1 |
| 0x70 | Unknown/Reserved Function | 1 |
| 0x71 | Unknown/Reserved Function | 1 |
| 0x72 | Unknown/Reserved Function | 1 |
| 0x73 | Unknown/Reserved Function | 1 |
| 0x74 | Unknown/Reserved Function | 1 |
| 0x75 | Unknown/Reserved Function | 1 |
| 0x76 | Unknown/Reserved Function | 1 |
| 0x77 | Unknown/Reserved Function | 1 |
| 0x78 | Unknown/Reserved Function | 1 |
| 0x79 | Unknown/Reserved Function | 1 |
| 0x7A | Unknown/Reserved Function | 1 |
| 0x7B | Unknown/Reserved Function | 1 |
| 0x7C | Unknown/Reserved Function | 1 |
| 0x7D | Unknown/Reserved Function | 2 |
| 0x7E | Unknown/Reserved Function | 1 |
| 0x7F | Unknown/Reserved Function | 1 |
| 0x80 | Exception Response | 1 |
| 0x81 | Exception Response | 9 |
| 0x82 | Exception Response | 1 |
| 0x83 | Exception Response | 1 |
| 0x84 | Exception Response | 1 |
| 0x86 | Exception Response | 1 |
| 0x89 | Exception Response | 1 |
| 0x8A | Exception Response | 1 |
| 0x8B | Exception Response | 1 |
| 0x8C | Exception Response | 1 |
| 0x8D | Exception Response | 1 |
| 0x8E | Exception Response | 1 |
| 0x8F | Exception Response | 1 |
| 0x90 | Exception Response | 1 |
| 0x91 | Exception Response | 1 |
| 0x92 | Exception Response | 1 |
| 0x93 | Exception Response | 1 |
| 0x94 | Exception Response | 1 |
| 0x95 | Exception Response | 1 |
| 0x96 | Exception Response | 1 |
| 0x97 | Exception Response | 1 |
| 0x98 | Exception Response | 1 |
| 0x99 | Exception Response | 1 |
| 0x9A | Exception Response | 1 |
| 0x9B | Exception Response | 1 |
| 0x9C | Exception Response | 1 |
| 0x9D | Exception Response | 1 |
| 0x9E | Exception Response | 1 |
| 0x9F | Exception Response | 1 |
| 0xA0 | Exception Response | 1 |
| 0xA1 | Exception Response | 1 |
| 0xA2 | Exception Response | 1 |
| 0xA3 | Exception Response | 1 |
| 0xA4 | Exception Response | 1 |
| 0xA5 | Exception Response | 1 |
| 0xA6 | Exception Response | 1 |
| 0xA7 | Exception Response | 1 |
| 0xA9 | Exception Response | 1 |
| 0xAB | Exception Response | 1 |
| 0xAC | Exception Response | 1 |
| 0xAD | Exception Response | 1 |
| 0xAE | Exception Response | 1 |
| 0xAF | Exception Response | 1 |
| 0xB0 | Exception Response | 1 |
| 0xB1 | Exception Response | 1 |
| 0xB2 | Exception Response | 1 |
| 0xB3 | Exception Response | 1 |
| 0xB4 | Exception Response | 1 |
| 0xB5 | Exception Response | 1 |
| 0xB6 | Exception Response | 1 |
| 0xB7 | Exception Response | 1 |
| 0xB8 | Exception Response | 1 |
| 0xB9 | Exception Response | 1 |
| 0xBA | Exception Response | 1 |
| 0xBB | Exception Response | 1 |
| 0xBC | Exception Response | 1 |
| 0xBD | Exception Response | 1 |
| 0xBE | Exception Response | 1 |
| 0xBF | Exception Response | 1 |
| 0xC0 | Exception Response | 1 |
| 0xC1 | Exception Response | 1 |
| 0xC2 | Exception Response | 1 |
| 0xC3 | Exception Response | 1 |
| 0xC4 | Exception Response | 1 |
| 0xC5 | Exception Response | 1 |
| 0xC6 | Exception Response | 1 |
| 0xC7 | Exception Response | 1 |
| 0xC8 | Exception Response | 1 |
| 0xC9 | Exception Response | 1 |
| 0xCA | Exception Response | 1 |
| 0xCB | Exception Response | 1 |
| 0xCC | Exception Response | 1 |
| 0xCD | Exception Response | 1 |
| 0xCE | Exception Response | 1 |
| 0xCF | Exception Response | 1 |
| 0xD0 | Exception Response | 1 |
| 0xD1 | Exception Response | 1 |
| 0xD2 | Exception Response | 1 |
| 0xD3 | Exception Response | 1 |
| 0xD4 | Exception Response | 1 |
| 0xD5 | Exception Response | 1 |
| 0xD6 | Exception Response | 1 |
| 0xD7 | Exception Response | 1 |
| 0xD8 | Exception Response | 1 |
| 0xD9 | Exception Response | 1 |
| 0xDA | Exception Response | 1 |
| 0xDB | Exception Response | 1 |
| 0xDC | Exception Response | 1 |
| 0xDD | Exception Response | 1 |
| 0xDE | Exception Response | 1 |
| 0xDF | Exception Response | 1 |
| 0xE0 | Exception Response | 1 |
| 0xE1 | Exception Response | 1 |
| 0xE2 | Exception Response | 1 |
| 0xE3 | Exception Response | 1 |
| 0xE4 | Exception Response | 1 |
| 0xE5 | Exception Response | 1 |
| 0xE6 | Exception Response | 1 |
| 0xE7 | Exception Response | 1 |
| 0xE8 | Exception Response | 1 |
| 0xE9 | Exception Response | 1 |
| 0xEA | Exception Response | 1 |
| 0xEB | Exception Response | 1 |
| 0xEC | Exception Response | 1 |
| 0xED | Exception Response | 1 |
| 0xEE | Exception Response | 1 |
| 0xEF | Exception Response | 1 |
| 0xF0 | Exception Response | 1 |
| 0xF1 | Exception Response | 1 |
| 0xF2 | Exception Response | 1 |
| 0xF3 | Exception Response | 1 |
| 0xF4 | Exception Response | 1 |
| 0xF5 | Exception Response | 1 |
| 0xF6 | Exception Response | 1 |
| 0xF7 | Exception Response | 1 |
| 0xF8 | Exception Response | 1 |
| 0xF9 | Exception Response | 1 |
| 0xFA | Exception Response | 1 |
| 0xFB | Exception Response | 1 |
| 0xFC | Exception Response | 1 |
| 0xFE | Exception Response | 1 |
| 0xFF | Exception Response | 1 |

### Function Code Coverage

Codes observed: `0x00`, `0x01`, `0x02`, `0x03`, `0x04`, `0x05`, `0x06`, `0x07`, `0x08`, `0x09`, `0x0A`, `0x0B`, `0x0C`, `0x0D`, `0x0E`, `0x0F`, `0x10`, `0x11`, `0x12`, `0x13`, `0x14`, `0x15`, `0x16`, `0x17`, `0x18`, `0x19`, `0x1A`, `0x1B`, `0x1C`, `0x1D`, `0x1E`, `0x1F`, `0x20`, `0x21`, `0x22`, `0x23`, `0x24`, `0x25`, `0x26`, `0x27`, `0x28`, `0x29`, `0x2A`, `0x2B`, `0x2C`, `0x2D`, `0x2E`, `0x2F`, `0x30`, `0x31`, `0x32`, `0x33`, `0x34`, `0x35`, `0x36`, `0x37`, `0x38`, `0x39`, `0x3A`, `0x3B`, `0x3C`, `0x3D`, `0x3E`, `0x3F`, `0x40`, `0x41`, `0x42`, `0x43`, `0x44`, `0x45`, `0x46`, `0x47`, `0x48`, `0x49`, `0x4A`, `0x4B`, `0x4C`, `0x4D`, `0x4E`, `0x4F`, `0x50`, `0x51`, `0x52`, `0x53`, `0x54`, `0x55`, `0x56`, `0x57`, `0x58`, `0x59`, `0x5A`, `0x5B`, `0x5C`, `0x5D`, `0x5E`, `0x5F`, `0x60`, `0x61`, `0x62`, `0x63`, `0x64`, `0x65`, `0x66`, `0x67`, `0x68`, `0x69`, `0x6A`, `0x6B`, `0x6C`, `0x6D`, `0x6E`, `0x6F`, `0x70`, `0x71`, `0x72`, `0x73`, `0x74`, `0x75`, `0x76`, `0x77`, `0x78`, `0x79`, `0x7A`, `0x7B`, `0x7C`, `0x7D`, `0x7E`, `0x7F`

---

*Report generated by Modbus TCP Parser*
