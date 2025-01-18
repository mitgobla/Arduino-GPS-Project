#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <SD.h>

class Config
{
private:
    char wifiSSID[32] = "";
    char wifiPassword[64] = "";
    char mqttServer[64] = "";
    unsigned int mqttPort = 1883;
    char mqttClientName[32] = "esp32-dev";
    char mqttUsername[32] = "";
    char mqttPassword[32] = "";
    char mqttTopic[32] = "device/0/data";
    unsigned int publishInterval = 5000;

public:
    Config();
    bool load(const char *filename = "/config.json");
    const char *getWiFiSSID() const;
    const char *getWiFiPassword() const;
    const char *getMQTTServer() const;
    unsigned int getMQTTPort() const;
    const char *getMQTTClientName() const;
    const char *getMQTTUsername() const;
    const char *getMQTTPassword() const;
    const char *getMQTTTopic() const;
    unsigned int getPublishInterval() const;
};

#endif