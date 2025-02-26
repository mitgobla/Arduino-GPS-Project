#include <Arduino.h>

// Libraries
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Local classes
#include "sensor/sensormanager.h"
#include "sd/sdmanager.h"
#include "config/config.h"

// Definitions
#define DHT_PIN             22
#define DHT_TYPE            DHT_TYPE_22
#define SD_CS_PIN           5
#define GPS_RX_PIN          16
#define GPS_TX_PIN          17
#define GPS_HW_SERIAL       2
#define GPS_BAUD            9600
#define USB_BAUD            115200
#define RECONNECT_INTERVAL  2500

// Global objects
DHT_Async dht(DHT_PIN, DHT_TYPE);
HardwareSerial gpsSerial(GPS_HW_SERIAL);
TinyGPSPlus gps;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Config config;
SDManager sdManager(SD_CS_PIN);
SensorManager sensorManager(dht, gpsSerial);

// Global variables
unsigned long lastPublish = 0; // Tracks last time data was published
unsigned long lastUpdate = 0;

// Function declerations

/// @brief Attempt to connect to WiFi once.
bool connectToWiFi();

/// @brief Attempt to connect to MQTT broker once.
bool connectToMQTT();

/// @brief Publish data to MQTT broker.
void publishData(const char* data);

/// @brief Process stored data on SD card
void processSDCardData();

/// @brief Check if GPS data is available
bool hasGPS();

void setup() {
    // Serial setup
    Serial.begin(USB_BAUD);
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    if (!sdManager.begin()) {
        Serial.println("[SD] SD card initialization failed.");
        while (1);
    }

    if (!config.load()) {
        Serial.println("[Config] Failed to load configuration file.");
        while(1);
    }

    delay(100);

    WiFi.mode(WIFI_STA);
    connectToWiFi();
    mqttClient.setServer(config.getMQTTServer(), config.getMQTTPort());
    connectToMQTT();
}

void loop() {
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
    if (millis() - lastUpdate >= RECONNECT_INTERVAL) {
        lastUpdate = millis();
        if (mqttClient.connected()) {
            mqttClient.loop();
        } else {
            // If we are not connected to MQTT:
            // 1. First check if we are connected to WiFi.
            // 2. If connected to Wifi, try connecting to MQTT again.
            if (connectToWiFi()) {
                connectToMQTT();
            }
        }
    }
    // We can publish a mesage after getPublishInterval milliseconds
    if (millis() - lastPublish >= config.getPublishInterval())
    {
        lastPublish = millis();

        // Prepare variables for data
        float temperature = 0.0;
        float humidity = 0.0;
        JsonDocument data;

        // Read DHT sensor and append to data
        if (sensorManager.readDHT(temperature, humidity))
        {
            data["tmp"] = temperature;
            data["hmd"] = humidity;

            // Read GPS device and append to data
            // Only if we have DHT data
            if (hasGPS())
            {
                data["lat"] = gps.location.lat();
                data["lng"] = gps.location.lng();
                data["alt"] = gps.altitude.meters();
                data["sat"] = gps.satellites.value();

                // Create timestamp from GPS date and time
                char timestamp[25];
                snprintf(
                    timestamp,
                    sizeof(timestamp),
                    "%04d-%02d-%02dT%02d:%02d:%02dZ",
                    gps.date.year(),
                    gps.date.month(),
                    gps.date.day(),
                    gps.time.hour(),
                    gps.time.minute(),
                    gps.time.second());
                data["ts"] = timestamp;
            }
        }

        // Only publish if there is data
        if (!data.isNull())
        {
            // Publish if connected
            // Otherwise save to SD card
            char payload[512];
            serializeJson(data, payload);
            if (WiFi.status() == WL_CONNECTED && mqttClient.connected())
            {
                publishData(payload);
                // Also publish any saved data on the SD card
                sdManager.processLines(publishData);
            }
            else
            {
                Serial.print("[SD] Saving data to SD card.");
                Serial.println(payload);
                sdManager.save(payload);
            }
        }
    }
}

bool connectToWiFi()
{
    // Make 10 attempts to connect to WiFi
    // Output the status for debugging
    // Wait 1 second between each attempt
    unsigned int attempts = 0;
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("[WiFi] Connecting to WiFi... ");
        Serial.println(config.getWiFiSSID());
        WiFi.begin(config.getWiFiSSID(), config.getWiFiPassword());
        while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            switch (WiFi.status()) {
                case WL_NO_SSID_AVAIL: Serial.println("[WiFi] SSID not found"); break;
                case WL_CONNECT_FAILED:
                    Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
                    break;
                case WL_CONNECTION_LOST: Serial.println("[WiFi] Connection was lost"); break;
                case WL_SCAN_COMPLETED:  Serial.println("[WiFi] Scan is completed"); break;
                case WL_DISCONNECTED:    Serial.println("[WiFi] WiFi is disconnected"); break;
                case WL_CONNECTED:
                    Serial.println("[WiFi] WiFi is connected!");
                    Serial.print("[WiFi] IP address: ");
                    Serial.println(WiFi.localIP());
                    break;
                default:
                    Serial.print("[WiFi] WiFi Status: ");
                    Serial.println(WiFi.status());
                    break;
            }
            delay(1000);
            attempts++;
        }
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi] No WiFi connection.");
            return false;
        } else {
            Serial.println("[WiFi] WiFi connected.");
        }
    }
    return true;
}


bool connectToMQTT()
{
    if (!mqttClient.connected()) {
        Serial.println("[MQTT] Connecting to MQTT...");
        if (!mqttClient.connect(config.getMQTTClientName(), config.getMQTTUsername(), config.getMQTTPassword())) {
            Serial.println("[MQTT] No MQTT connection.");
            return false;
        } else {
            Serial.println("[MQTT] MQTT connected.");
        }
    }
    return true;
}

void publishData(const char *data)
{
    // Only publish if connected to WiFi and MQTT, attempt first
    if (connectToWiFi() && connectToMQTT()) {
        Serial.print("[MQTT] Publishing data...");
        Serial.println(data);
        mqttClient.publish(config.getMQTTTopic(), data);
    }
}

bool hasGPS()
{
    return (gps.location.isUpdated() && gps.time.isUpdated() && gps.date.isUpdated());
}
