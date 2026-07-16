# 📊 ESP32-S3 + ICM-20948 Real-Time Wireless IMU Dashboard

[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32--S3-orange)](https://platformio.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Framework](https://img.shields.io/badge/Framework-Arduino-blue)](https://www.arduino.cc/)

> **Live 3D motion tracking directly in your browser – no internet required!**

This project turns an **ESP32-S3** and an **ICM-20948** 9-DOF sensor into a standalone Wi-Fi access point that serves a beautiful, real-time dashboard. Connect your phone or laptop to the ESP32's Wi-Fi, open `http://192.168.4.1`, and watch accelerometer, gyroscope, magnetometer, and temperature data update at 20Hz.

---

## ✨ Features

- **📶 Self-Hosted Wi-Fi**: ESP32 creates its own network – no router or internet needed.
- **🚀 20Hz Real-Time Streaming**: Smooth, low-latency data updates via WebSockets.
- **📊 Live Dashboard**: Beautiful dark-themed UI with scrolling charts (Chart.js).
- **🛡️ Fail‑Safe Fallback**: If the sensor isn't connected, it automatically generates sine waves so the dashboard never breaks.
- **💾 CSV Export**: Download your sensor data with one click for offline analysis.
- **⚡ Built with PlatformIO**: Easy to compile, upload, and modify.

---

## 🛠️ Hardware Requirements

| Component | Quantity |
| :--- | :--- |
| ESP32-S3-DevKitC-1 (or any ESP32-S3 with 16MB Flash) | 1 |
| ICM-20948 9-DOF IMU Breakout | 1 |
| Jumper Wires (Female-to-Female) | 6 |

---

## 🔌 Wiring Guide (SPI Mode)

Connect the ICM-20948 to your ESP32-S3:

| ICM-20948 Pin | ESP32-S3 Pin | Wire Color |
| :--- | :--- | :--- |
| VCC | 3.3V | Red |
| GND | GND | Black |
| SCK / SCL | GPIO 12 | Yellow |
| MOSI / SDA | GPIO 11 | Green |
| MISO / SDO | GPIO 13 | Blue |
| CS | GPIO 10 | Orange |

> **⚠️ Critical**: The ICM-20948 is **3.3V only** – do not connect to 5V. Ensure the `ADR` jumper on the breakout board is **OPEN** to enable SPI mode.

---

## 🚀 Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/ICM20949_WebServer.git
cd ICM20949_WebServer
2. Install Dependencies

    Install VS Code and the PlatformIO extension.

    PlatformIO will automatically download the required libraries (SparkFun ICM-20948, ArduinoJson, ESPAsyncWebServer).

3. Upload the Filesystem (LittleFS)
bash

pio run --target uploadfs

4. Upload the Firmware
bash

pio run --target upload

5. Connect and View

    On your phone/PC, connect to the Wi-Fi network ESP32-Dashboard.

    Password: 12345678.

    Open your browser and go to http://192.168.4.1.

    Tilt the board and watch the data change in real-time!

📂 Project Structure
text

ICM20949_WebServer/
├── data/
│   ├── chart.min.js         # Chart.js library (served locally)
│   └── index.html           # Dashboard HTML/CSS/JS
├── src/
│   └── main.cpp             # ESP32 firmware (Wi-Fi AP + WebSocket + Sensor)
├── platformio.ini           # PlatformIO configuration
└── README.md                # This file

🧠 How It Works

    ESP32 Firmware:

        Initializes the ICM-20948 over SPI.

        Starts an Access Point (ESP32-Dashboard).

        Runs a WebSocket server on ws://192.168.4.1/ws.

        Reads sensor data every 50ms and pushes JSON packets to all connected clients.

    Fallback Logic:

        If the sensor fails to initialize, the ESP32 generates smooth sine waves. This allows you to test the dashboard even without the hardware.

    Web Dashboard:

        Loads index.html and chart.min.js directly from the ESP32's flash (no internet required).

        Establishes a WebSocket connection and updates the numbers and chart in real-time.

        Includes auto‑reconnect logic and CSV download.
🐛 Troubleshooting
Issue	Solution
Sensor not detected	Check MISO/MOSI wiring. Ensure the ADR jumper is OPEN.
Dashboard loads but numbers stay at 0	Open the browser console (F12). If Chart is not defined appears, clear your browser cache.
WebSocket disconnects	Temporarily disable Windows Firewall or add an inbound rule for port 80.
Cannot upload filesystem	Ensure the data folder is in the project root and you have selected the correct partition scheme in platformio.ini.
🤝 Contributing

Pull requests are welcome! If you have improvements (e.g., 3D orientation cube, data logging to SD card, or MQTT integration), feel free to open an issue or submit a PR.
🙏 Acknowledgements

    SparkFun for the ICM-20948 library.

    me-no-dev for the AsyncWebServer library.

    Chart.js for the beautiful charts.

⭐ If you found this useful, please give it a star! It helps others discover the project.
text


---

## 📄 Step 4: The `README.md` Placeholder Images

In the README, I added placeholders for screenshots. **You must replace them** with actual images to get stars:

1. **Take a screenshot** of your dashboard on your phone (`http://192.168.4.1`).
2. **Take a photo** of your wired setup (ESP32 + ICM-20948).
3. Save them as `dashboard.png` and `wiring.jpg` inside a new folder called `images/` in your repo.
4. Update the README links to:
   ```markdown
   ![Dashboard](images/dashboard.png)
   ![Wiring](images/wiring.jpg)