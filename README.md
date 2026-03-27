# BitStrait: A Fault-Tolerant Telemetry Protocol

**BitStrait** is a lightweight C-based communication protocol designed to demonstrate the transition from high-risk "Legacy" data packing to modern, framed, and checksummed "Binary" streams. Built with a focus on memory safety and resilience against bit-level interference on macOS/Unix systems.

---

## The Engineering Challenge
In embedded systems (Satellites, UAVs, Industrial IoT), data streams often suffer from:
1.  **Bit-Flipping:** Electromagnetic interference altering data in transit.
2.  **Desynchronization:** The receiver losing track of where a data packet starts.
3.  **Memory Corruption:** Malformed packets overwriting critical system variables (Stack Smashing).

BitStrait solves these using **Consistent Overhead Byte Stuffing (COBS)** and **CRC-8 Checksums**.

---

## Technical Architecture

### 1. Framing: COBS (Consistent Overhead Byte Stuffing)
Traditional "Start/Stop" byte methods risk being confused with the data itself. BitStrait uses COBS to eliminate `0x00` from the payload. This allows the `0x00` byte to serve as a **guaranteed** synchronization marker. If a packet is corrupted, the parser resets the moment it sees the next `0x00`.

### 2. Fault Detection: CRC-8
Every packet includes a Cyclic Redundancy Check (CRC-8) using the polynomial $x^8 + x^2 + x^1 + 1$ ($0x07$). This ensures that even a single bit-flip is detected before the data is "Unpacked" into the system state.

### 3. Memory Safety: Hardened Ring Buffer
The system utilizes a 32-byte Ring Buffer to decouple data reception from processing.
* **Bounds Guarding:** Specifically hardened against "Quintillion-Count" overflows identified during stress testing.
* **Crumple Zones:** Buffers are sized to $2^n$ to provide memory alignment and safety padding.

---

## UI & Simulation
The project includes a real-time **ncurses** dashboard that visualizes the "Architectural Progression":
* **V1 View:** Shows raw hex data and its vulnerability to `0x00` collisions.
* **V2 View:** Shows the live COBS wire-stream and framing overhead.
* **Diagnostic Panel:** Real-time Battery (simulated slow discharge), Altitude, and CRC health.

---

## Debugging Insights (Post-Mortem)
During development, several critical "Pitfalls" were identified and fixed using **LLDB**:

* **The Infinite Search Bug:** Pressing `F` (Fault) originally corrupted the `0x00` delimiter. The parser would search infinitely through RAM until it hit a segment fault. 
    * *Fix:* Protected the framing byte from the interference injector.
* **The Stack Smash:** A buffer overflow once overwrote the `rx_ring.count` variable with a massive 64-bit integer.
    * *Fix:* Implemented strict `parser_idx` resetting and range-validation in the HAL.

---

## Getting Started

### Prerequisites
* `clang` or `gcc`
* `ncurses` library

### Building & Running
```bash
# Compile the project
clang -Wall -Iinclude -g src/*.c -o bitstrait -lncurses

# Run the simulation
./bitstrait
```
---

## Controls
* **[F]**: Toggle Bit-Flip Fault Injection.
* **[Q]**: Graceful Shutdown and UI Cleanup.

------

## 🛡️ NASA "Power of 10" Compliance
To ensure mission-critical reliability, this project was refactored to satisfy the **JPL Institutional Coding Standard for the C Programming Language**:

1.  **Simple Control Flow:** No `goto`, `setjmp`, or recursion.
2.  **Fixed Loop Bounds:** All loops have a verifiable upper bound to prevent infinite hangs.
3.  **No Dynamic Memory:** Zero use of `malloc` or `free`. All buffers are stack-allocated.
4.  **Minimal Function Length:** Functions are modular and focused on a single task.
5.  **Strict Assertion Density:** Critical pointers and indices are guarded by `assert()` and boundary checks.
6.  **Minimal Data Scope:** Variables are declared at the lowest possible scope to prevent state leakage.
7.  **Check Returns:** All sub-function return values (like COBS decode length) are validated.
8.  **Limited Preprocessor:** Macros are used only for constants, not for complex code generation.
9.  **Pointer Safety:** No function pointers or hidden dereferencing.
10. **Static Analysis:** 100% clean audit via `cppcheck --enable=all --inconclusive`.

---

## 🗺️ Future Roadmap
BitStrait is designed to evolve from a local simulation into a production-ready aerospace communication stack:

* **Multi-Node Addressing:** Implementing a Source/Destination ID header within the COBS payload to allow a single Ground Station to manage a swarm of sensors.
* **Forward Error Correction (FEC):** Integrating Hamming Codes or Reed-Solomon encoding to *repair* corrupted bits in high-interference environments, reducing the need for retransmissions.

* **Hardware-In-The-Loop (HITL):** Porting `bitstrait_hal.c` to STM32 (UART) or ESP32 to test protocol jitter and latency on real silicon.
* **Fragmentation & Reassembly:** Adding a Sequence Number and "More-Fragments" bit to allow the transmission of large data logs across small, MTU-limited COBS frames.

## License
Copyright (C) 2026

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**. 
Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed. 

See the included `LICENSE` file for the full legal text.
