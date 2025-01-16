#ifndef CONFIG_H
#define CONFIG_H

#include <ArduinoJson.h>
#include <SD.h>

class Config
{
private:
    const char *wifiSSID = "";
    const char *wifiPassword = "";
    const char *mqttServer = "";
    unsigned int mqttPort = 1883;
    const char *mqttClient = "esp32-dev";
    const char *mqttUsername = "";
    const char *mqttPassword = "";

public:
    Config();
    bool load(const char *filename);
    const char *getWiFiSSID() const;
    const char *getWiFiPassword() const;
    const char *getMQTTServer() const;
    unsigned int getMQTTPort() const;
    const char *getMQTTClient() const;
    const char *getMQTTUsername() const;
    const char *getMQTTPassword() const;
};

#endif