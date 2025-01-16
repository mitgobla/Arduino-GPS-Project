#include "config.h"

Config::Config()
{
}

/// @brief Load the config.json from the SD card
/// @param filename Path to the config file. Default "config.json"
/// @return Boolean on if the config was loaded successfully
bool Config::load(const char *filename = "config.json")
{
    // Open config file
    File configFile = SD.open(filename, FILE_READ);
    if (!configFile) {
        Serial.println("Failed to open config file.");
        return false;
    }

    // Parse config file
    JsonDocument configJson;
    DeserializationError error = deserializeJson(configJson, configFile);
    configFile.close();

    if (error) {
        Serial.println("Failed to parse config file.");
        return false;
    }

    // Assign variables
    wifiSSID = configJson["wifiSSID"] | "";
    wifiPassword = configJson["wifiPassword"] | "";
    mqttServer = configJson["mqttServer"] | "";
    mqttPort = configJson["mqttPort"] | 1883;
    mqttClient = configJson["mqttClient"] | "esp32-dev";
    mqttUsername = configJson["mqttUsername"] | "";
    mqttPassword = configJson["mqttPassword"] | "";

    Serial.println("Config loaded successfully.");
    return true;
}

const char *Config::getWiFiSSID() const
{
    return wifiSSID;
}

const char *Config::getWiFiPassword() const
{
    return wifiPassword;
}

const char *Config::getMQTTServer() const
{
    return mqttServer;
}

unsigned int Config::getMQTTPort() const
{
    return mqttPort;
}

const char *Config::getMQTTClient() const
{
    return mqttClient;
}

const char *Config::getMQTTUsername() const
{
    return mqttUsername;
}

const char *Config::getMQTTPassword() const
{
    return mqttPassword;
}
