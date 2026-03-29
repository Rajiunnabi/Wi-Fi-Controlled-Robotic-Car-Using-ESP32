# WiFi Controlled Robot Car

[![Platform](https://img.shields.io/badge/platform-ESP32-blue)](https://www.espressif.com/en/products/socs/esp32)
[![Framework](https://img.shields.io/badge/framework-Arduino-00979C)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
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

### Web Interface
