# WiFi Controlled Robot Car

[![Platform](https://img.shields.io/badge/platform-ESP32-blue)](https://www.espressif.com/en/products/socs/esp32)
[![Framework](https://img.shields.io/badge/framework-Arduino-00979C)](https://www.arduino.cc/)
[![IoT](https://img.shields.io/badge/IoT-Robotics-orange)](https://en.wikipedia.org/wiki/Internet_of_things)

An ESP32-based WiFi controlled robotic car with responsive web interface, real-time speed control, and a **persistent software safety interlock mechanism** that provides a verifiable safe state for physical interaction.

📄 **Full Paper:** [IEEE Conference Paper](docs/IEEE_Paper.pdf)

---

## 📋 Table of Contents

- [Project Overview](#project-overview)
- [Key Features](#key-features)
- [Demonstration](#demonstration)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Software Stack](#software-stack)
- [Installation and Setup](#installation-and-setup)
- [Usage Guide](#usage-guide)
- [Experimental Results](#experimental-results)
- [Future Improvements](#future-improvements)
- [Repository Structure](#repository-structure)
- [Troubleshooting](#troubleshooting)
- [Acknowledgments](#acknowledgments)
- [License](#license)

---

## 🎯 Project Overview

This project presents the design and implementation of a low-cost Wi-Fi controlled robotic car using an ESP32 microcontroller. Unlike conventional systems that rely on existing Wi-Fi infrastructure, this robot creates its **own Access Point (SoftAP mode)** , eliminating complex network configuration.

### The Problem This Solves

| Challenge | Conventional Approach | Our Solution |
|-----------|----------------------|--------------|
| Network Setup | Requires existing WiFi router | Creates own WiFi network |
| Configuration | Complex SSID/password setup | Connect directly - no config needed |
| Safety | Only momentary stop commands | Persistent safety lock (HTTP 403) |
| Control | Dedicated hardware transmitter | Any smartphone/computer with browser |

### Key Innovation: Safety Interlock

A core contribution of this work is the **persistent software-based safety interlock**—implemented as a system-wide state machine—that explicitly rejects all movement commands when engaged. When locked, the robot returns HTTP 403 (Forbidden) responses, providing a verifiable safe state for physical interaction.

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| **Infrastructure-Free** | Creates own WiFi network (SoftAP mode) - no router needed |
| **Persistent Safety Lock** | Software interlock rejects all commands when engaged (HTTP 403) |
| **Web-Based Control** | Responsive HTML/CSS/JS interface accessible from any browser |
| **Real-Time Speed Control** | PWM-based speed regulation (0-255 range) |
| **Multiple Input Methods** | Touch, mouse, and keyboard controls (arrow keys + WASD) |
| **Visual Status Feedback** | Color-coded UI indicating Active (green) / Locked (red) state |
| **Low Latency** | 50-100 ms response time - imperceptible for RC control |
| **Command Logging** | Real-time debug log of all commands sent |

---

## 🎬 Demonstration




**📸 System Diagrams:** [System Architecture](images/system_architecture.png) | [State Machine](images/state_machine.png)

---

## 🔧 Hardware Components

| Component | Specification | Purpose |
|-----------|---------------|---------|
| **ESP32 Dev Board** | 240 MHz, 520 KB SRAM | Main microcontroller, WiFi AP, web server |
| **L298N Motor Driver** | Dual H-bridge, 5-12V | Motor direction and speed control |
| **7V Li-ion Battery** | 2S, 7.4V nominal | Power source |
| **4x DC Gear Motors** | 3-6V, 100-200 RPM | Wheel actuation |
| **Foamboard Chassis** | 25cm x 20cm | Structural base |
| **Jumper Wires** | Male-female | Electrical connections |

### Pin Mapping

| Motor Driver Pin | ESP32 GPIO | Function |
|-----------------|------------|----------|
| IN1 | 27 | Motor A direction |
| IN2 | 26 | Motor A direction |
| ENA | 14 | Motor A speed (PWM) |
| IN3 | 25 | Motor B direction |
| IN4 | 33 | Motor B direction |
| ENB | 32 | Motor B speed (PWM) |

**📸 Prototype Photos:** [Top View](images/prototype.jpeg)

---

## 💻 Software Stack

### Communication Layer (HTTP Server)

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Serves web control interface |
| `/command?cmd=<dir>,<speed>` | GET | Movement command (e.g., `f,150`) |
| `/toggleStop` | POST | Toggles safety interlock |
| `/getStopStatus` | GET | Returns current lock state |
| `/speed?value=<0-255>` | GET | Sets motor speed |

### Command Format
Format: <direction>,<speed>

Directions:
f - Forward
b - Backward
l - Left turn
r - Right turn
s - Stop

Example: "f,150" = Forward at speed

### Web Interface [View](images/web_interface.jpeg)


### Web Interface Features

- Responsive design (mobile/desktop)
- Touch and mouse support
- Keyboard controls (Arrow keys + WASD)
- Real-time speed slider
- Command log for debugging
- Visual safety state indication

---

## 🚀 Installation and Setup

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (version 2.0 or higher)
- ESP32 board package installed
- USB cable for programming

### Step 1: Install ESP32 Board Package

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to "Additional Board Manager URLs":https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP32" and install

### Step 2: Install Required Libraries

Install these libraries via **Sketch → Include Library → Manage Libraries**:

| Library | Purpose |
|---------|---------|
| `WiFi.h` | Built-in - WiFi connectivity |
| `WebServer.h` | Built-in - HTTP server |
| `AsyncUDP.h` | Built-in - UDP support |

### Step 3: Clone and Upload

```bash
# Clone the repository
git clone https://github.com/Rajiunnabi/Wi-Fi-Controlled-Robotic-Car-Using-ESP32.git
cd WiFi-Robot-Car

# Open the main sketch in Arduino IDE
open src/Freebot_V1.ino

