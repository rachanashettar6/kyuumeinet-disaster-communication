KyuumeiNet: LoRa-Based Earthquake Emergency Communication Network

A Disaster Resilient Communication System for Emergency Response and Rescue Operations

Overview

KyuumeiNet is a completed disaster communication system developed to provide reliable emergency communication during earthquakes and other large-scale natural disasters. The system enables victims to transmit SOS alerts and GPS coordinates to rescue teams even when conventional communication infrastructure such as cellular networks and internet services become unavailable.

Built using ESP32 microcontrollers and SX1278 LoRa transceivers, KyuumeiNet establishes a long-range, low-power, infrastructure-independent communication network capable of operating in post-disaster environments where traditional communication systems fail.
The project focuses on enhancing emergency response efficiency by enabling rapid victim identification, location tracking, and rescue coordination.


---

Motivation

Countries such as Japan frequently experience earthquakes and natural disasters that can severely damage communication infrastructure. During these events, survivors often face difficulties in contacting rescue teams due to network outages and overloaded communication channels.
KyuumeiNet was developed to address this challenge by creating an independent emergency communication platform capable of maintaining connectivity in disaster-affected areas.


---

System Architecture

Mobile Application
        │
        ▼
ESP32 User Node
        │
        ▼
SX1278 LoRa Communication
        │
        ▼
Relay Node
        │
        ▼
Base Station
        │
        ▼
Rescue Team Dashboard


---

Working Principle

1. The user activates the emergency SOS feature through a mobile application.
2. GPS coordinates are captured from the mobile device.
3. The User Node receives the emergency request.
4. The SOS packet is transmitted through the SX1278 LoRa module.
5. Relay Nodes forward the packet over long distances.
6. The Base Station receives and processes the emergency information.
7. Rescue operators view victim details and location information.
8. An acknowledgement message is sent back to the user through the LoRa network.
9. The user receives confirmation that the alert has been successfully delivered.


---

Key Features

Long-range LoRa communication
Earthquake emergency response support
Infrastructure-independent operation
GPS-based victim localization
SOS alert transmission
Multi-hop relay communication
Rescue acknowledgement mechanism
Low-power operation
Battery-powered deployment
Real-time emergency notification
Expandable communication architecture
Reliable operation in network outage conditions


---

Hardware Components

User Node

ESP32 Development Board
SX1278 LoRa Module (433 MHz)
433 MHz Antenna
Smartphone


Relay Node

ESP32 Development Board
SX1278 LoRa Module
High-Gain Antenna


Base Station

ESP32 Development Board
SX1278 LoRa Module
Monitoring Dashboard


---

Software Technologies

Arduino IDE
Embedded C++
ESP32 Framework
LoRa Communication Library
Firebase Studio
GPS Services
Wi-Fi Communication
Serial Monitoring Dashboard

---

LoRa Communication Specifications


| Parameter           | Value                        |
| ------------------- | ---------------------------- |
| Frequency           | 433 MHz                      |
| Module              | SX1278                       |
| Communication Type  | LoRa (Chirp Spread Spectrum) |
| Maximum Range       | Up to 15 km (Line of Sight)  |
| Network Requirement | None                         |
| Power Consumption   | Low Power                    |
| Data Transmission   | GPS Coordinates + SOS Data   |
| Communication Mode  | Peer-to-Peer                 |


---

Applications

Earthquake Emergency Communication
Disaster Management Systems
Search and Rescue Operations
Humanitarian Relief Networks
Remote Area Communication
Emergency Alert Infrastructure
Smart City Safety Systems
Public Safety Networks


---

Technical Highlights

Designed using ESP32 and SX1278 LoRa modules
Supports long-distance communication without cellular networks
Provides location-aware emergency alerts
Enables communication in disaster-hit regions
Utilizes low-power wireless technology
Supports relay-based communication architecture
Suitable for deployment in earthquake-prone regions


---

Results

Successful transmission of SOS alerts through LoRa communication
Reliable GPS coordinate sharing between nodes
Stable relay-node message forwarding
Successful acknowledgement delivery
Infrastructure-independent communication achieved
Long-range communication validated using SX1278 LoRa modules



---

Future Scope

AI-Assisted Rescue Prioritization
Earthquake Detection Sensor Integration
Solar-Powered Relay Stations
Offline Mapping Support
Mesh Networking Architecture
Satellite Communication Backup
Real-Time Disaster Analytics Dashboard
Multi-User Emergency Coordination


---

Project Impact

KyuumeiNet demonstrates how low-power long-range wireless communication can be leveraged to build resilient emergency communication systems capable of operating during infrastructure failures. The project contributes toward safer disaster response mechanisms and improved rescue coordination in earthquake-prone regions.


---

Technologies

ESP32 
LoRa SX1278 
Embedded Systems 
IoT Wireless Communication 
GPS Tracking 
Disaster Management 
Emergency Response

---

Author

Rachana Shettar
Electronics and Communication Engineering
REVA University