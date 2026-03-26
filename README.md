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

---

## License
Copyright (C) 2026

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**. 
Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed. 

See the included `LICENSE` file for the full legal text.
