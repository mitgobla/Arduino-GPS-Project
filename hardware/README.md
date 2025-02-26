# Arduino GPS Project Hardware

ESP32 development board using Platform.IO with ArduinoCore, utilizing a NEO-6M GPS module, SD card module, portable battery, DHT22 temperature and humidity sensor, and a project box case.

![Device in a junction box](docs/project_in_box.png)

## Table of Contents

- [Arduino GPS Project Hardware](#arduino-gps-project-hardware)
  - [Table of Contents](#table-of-contents)
  - [Hardware](#hardware)
  - [Wiring](#wiring)
  - [Software](#software)
    - [Configuration](#configuration)
    - [Classes](#classes)
    - [Methodology](#methodology)
      - [Setup](#setup)
      - [Loop](#loop)

## Hardware

- [ESP32-DevKitC 32U](https://www.amazon.co.uk/gp/product/B0DMRYRX6T)
- [NEO-6M GPS Module](https://www.amazon.co.uk/dp/B08XGN4YLY)
- DHT22 Temperature and Humidity Sensor
- [Micro SD Card Module](https://www.amazon.co.uk/dp/B06XHJTGGC)
- 5V Portable Battery
- Outdoor Junction Box for Project Case
- 25mm gland for cable entry
- DC Step Down Module (5V to 3.3V)
- Small breadboard
- 20mm Power switch
- Jumper wires

## Wiring

- **Portable Battery**
  - 5V → 5V Breadboard Rail
  - GND → GND Breadboard Rail
  - _Power Switch is on ground line_
- **DC Step Down Module**
  - 5V → 5V Breadboard Rail
  - GND → GND Breadboard Rail
  - 3.3V → 3.3V Breadboard Rail
- **ESP32-DevKitC 32U**
  - 5V Breadboard Rail → 5V Pin
  - GND Breadboard Rail → GND Pin
- **NEO-6M GPS Module**
  - VCC → 5V Breadboard Rail
  - GND → GND Breadboard Rail
  - TX → GPIO 16
  - RX → GPIO 17
- **DHT22 Temperature and Humidity Sensor**
  - VCC → 3.3V Breadboard Rail
  - GND → GND Breadboard Rail
  - Data → GPIO 22
- **Micro SD Card Module**
  - VCC → 5V Breadboard Rail
  - GND → GND Breadboard Rail
  - MISO → GPIO 19
  - MOSI → GPIO 23
  - SCK → GPIO 18
  - CS → GPIO 5

## Software

Using Platform.io with Arduino framework.

Libraries used:

- **toannv17/DHT Sensors Non-Blocking**: Modified version of the DHT sensor library to be non-blocking.
- **mikalhart/TinyGPSPlus**: GPS library for parsing NMEA data from NEO-6M module.
- **knolleary/PubSubClient**: MQTT library for ESP32 to publish data as a client.
- **bblanchon/ArduinoJson**: JSON library for parsing and creating JSON data.

### Configuration

JSON-based configuration used to store WiFi and MQTT connection details.

1. Copy `config.example.json` to `config.json` and fill in the values.
2. Upload the `config.json` to the SD card before inserting it into the module.

| Key | Description |
| --- | ----------- |
| `wifiSSID` | WiFi SSID |
| `wifiPassword` | WiFi Password |
| `mqttServer` | MQTT Server Address |
| `mqttPort` | MQTT Server Port |
| `mqttClientName` | MQTT Client Name of the device |
| `mqttUsername` | MQTT Username for authentication |
| `mqttPassword` | MQTT Password for authentication |
| `mqttTopic` | MQTT Topic to publish to |
| `publishInterval` | Interval in milliseconds to publish data to MQTT server |

### Classes

| Class | Description |
| ----- | ----------- |
| `Config` | Configuration class to read and parse the `config.json` file. |
| `SDManager` | SD card manager class to save and parse data on the SD card. |
| `SensorManager` | Sensor manager class for DHT22 sensor and GPS module. |

### Methodology

#### Setup

1. Initialize SD card and read configuration.
2. Initialize WiFi and MQTT connection.

#### Loop

1. Read GPS data and parse it.
2. Check if connected to WiFi and MQTT server.
3. Check if it is time to publish data.
   1. Read DHT22 sensor data.
   2. Create JSON payload.
   3. Publish data to MQTT server if connected.
   4. Also publish any data saved on the SD card.
   5. Save data to SD card if not connected.

