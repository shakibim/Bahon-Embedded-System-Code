# Code of Embedded System of Bahon (An IoT-Based Embedded System for Smart Public Transportation)

This repository contains the **embedded system source code** for the IoT-based **Automated Fare Collection System** project. The embedded system is responsible for interacting with hardware modules, collecting real-time passenger and location data, and transmitting it to the cloud server for processing and fare calculation.
The system is built around an **ESP32 microcontroller**, which connects with components such as a **GPS module**, **RFID scanner**, **OLED display**, and **buzzer** to provide a complete smart transport hardware solution. It communicates with the server over Wi-Fi or GSM, allowing seamless integration between the hardware, web admin panel, and mobile applications.


## System Overview

The embedded system operates as the bridge between hardware components and the cloud server. It collects data from sensors and input devices, processes it locally, and sends it to the cloud in real time for storage and analysis.

- The **ESP32** serves as the central controller.  
- It collects data from the **GPS** and **RFID/NFC** modules, processes it, and transmits it to the server in real time.  
- The **GPS module** provides accurate location tracking for vehicles.  
- The **RDM6300 RFID scanner** reads passenger RFID cards, using their unique tag numbers as identifiers.  
- The **OLED display** provides visual feedback such as “Scan Successful” or “Error: Card Not Recognized”.  
- The **active buzzer** gives audible alerts during key events (successful or failed scans).  
- Additionally, a **supervisor interface** in the mobile app allows QR code scanning as an alternative to RFID cards, ensuring flexibility in passenger identification.


## Hardware Components

| No. | Component | Description |
|-----|------------|-------------|
| 1 | **ESP32 Microcontroller** | Central unit that coordinates communication between hardware modules and the server. |
| 2 | **NEO-M8N GPS Module** | Provides real-time location data using latitude and longitude. |
| 3 | **RDM6300 / MFRC522 RFID Scanner** | Reads RFID tags to identify passengers. |
| 4 | **OLED Display (0.96 inch, I2C, 128x64)** | Displays system messages, alerts, and status updates. |
| 5 | **Active Buzzer Module** | Emits sound signals for events such as successful scans or errors. |
| 6 | **10x15cm Veroboard** | Holds and connects all components to form the circuit. |
| 7 | **5V Battery Pack / Power Supply** | Powers the entire system, ensuring portability. |
| 8 | **125kHz Printable RFID Card** | Used by passengers for identification during scanning. |
| 9 | **Capacitors and Resistors** | Stabilize voltage and regulate current within the circuit. |
| 10 | **PCB Board / Breadboard** | Used for assembling and testing the system circuit. |
| 11 | **AGPS Antenna** | Improves GPS signal reception and location accuracy. |
| 12 | **ABS 18650 Battery Holder** | Holds and secures 3.7V batteries for portable use. |
| 13 | **TP4056 with Boost Converter** | Charges the 3.7V lithium battery and converts voltage to 5V. |
| 14 | **Switches** | Allow manual control of system power and operation. |


## Functional Summary

- Real-time vehicle location tracking via GPS  
- Passenger identification through RFID or QR scanning  
- Instant data transmission to cloud database (Firebase/Server)  
- OLED display for system status and alerts  
- Buzzer for audio feedback  
- Portable and energy-efficient design suitable for mobile use  

## Circuit Diagram

<p align="center">
  <img width="650" alt="Circuit Diagram 1" src="https://github.com/user-attachments/assets/252e9242-e90d-4c90-99a9-848b05be08fb" />
</p>

<p align="center">
  <img width="720" alt="Circuit Diagram 2" src="https://github.com/user-attachments/assets/73b7c9a2-cb31-44c0-a2f9-f3a0908d2cf9" />
</p>

---

## Implemented Embedded System

<p align="center">
  <img width="700" alt="Implemented System 1" src="https://github.com/user-attachments/assets/bd307982-64fe-4b0d-beeb-89d1206a0cff" />
</p>

<p align="center">
  <img width="720" alt="Implemented System 2" src="https://github.com/user-attachments/assets/1fc5a622-aee7-4cbd-9fc7-f53563bc1a78" />
</p>

### Author

**Sk. Md. Shakib Imran**  
Department of Computer Science & Engineering  
East West University, Dhaka, Bangladesh  
Email: [shakibim233@gmail.com](mailto:shakibim233@gmail.com)
