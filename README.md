# MuSE BLE Advertiser & Web Controller

This project is for the ESP32 and allows control of BLE advertisement data via a simple web interface hosted on the device itself.

It is designed to send specific manufacturer BLE data packets, which can be used to control compatible devices.

# Features

- Wi-Fi connectivity – ESP32 connects to your local Wi-Fi network
- Web-based UI – adjust modes via slider and buttons
- BLE advertising – sends specific manufacturer data over BLE depending on selected mode
- Supports 7 speed modes, 7 vibration modes, and an off mode

## Getting Started

### Requirements

- ESP32 development board (tested on NodeMCU ESP32 and ESP32S)
- Arduino IDE
- ESP32 Board Package (recommended version: v2.0.18 - arduino.5)
- Libraries:
- [`NimBLE-Arduino`](https://github.com/h2zero/NimBLE-Arduino) (version **1.4.2**)
- Built-in libraries: WiFi.h, WebServer.h

### Installation

1. Install the ESP32 Board Package in Arduino IDE
2. Install the NimBLE-Arduino library from the Library Manager (make sure it's version **1.4.2**)
3. Open the muse.ino file
4. Replace Wi-Fi credentials:
   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *password = "YOUR_WIFI_PASSWORD";

5. Upload to your ESP32
6. Open Serial Monitor at 115200 baud to confirm startup

### Usage
- Once powered, the ESP32 connects to Wi-Fi and starts hosting a web interface.
- Open a browser and navigate to the IP shown in the Serial Monitor.
- Use the slider to select speed modes (0–7).
- Use the buttons to trigger vibration modes (8–14).
- BLE advertising data updates immediately when you change a mode.

### Troubleshooting
- If you get errors like setScanResponse, setMaxPreferred, etc., check your NimBLE library version.
- BLE devices like Lovespouse may require specific manufacturer data. This code sends predefined formats, but may need adaptation for specific devices.
