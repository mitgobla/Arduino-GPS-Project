#include "config.h"

Config::Config()
{
}

/// @brief Load the config.json from the SD card
/// @param filename Path to the config file. Default "config.json"
/// @return Boolean on if the config was loaded successfully
bool Config::load(const char *filename /*= "/config.json"*/)
{
    // Open config file
    File configFile = SD.open(filename, FILE_READ);
    if (!configFile) {
        Serial.println("[Config] Failed to open config file.");
        return false;
    }

    // Parse config file
    JsonDocument configJson;
    DeserializationError error = deserializeJson(configJson, configFile);
    configFile.close();

    if (error) {
        Serial.println("[Config] Failed to parse config file.");
        Serial.println(error.c_str());
        return false;
    }

    // Assign variables
    strlcpy(wifiSSID, configJson["wifiSSID"] | "", sizeof(wifiSSID));
    strlcpy(wifiPassword, configJson["wifiPassword"] | "", sizeof(wifiPassword));
    strlcpy(mqttServer, configJson["mqttServer"] | "", sizeof(mqttServer));
    mqttPort = configJson["mqttPort"] | 1883;
    strlcpy(mqttClientName, configJson["mqttClientName"] | "esp32-dev", sizeof(mqttClientName));
    strlcpy(mqttUsername, configJson["mqttUsername"] | "", sizeof(mqttUsername));
    strlcpy(mqttPassword, configJson["mqttPassword"] | "", sizeof(mqttPassword));
    strlcpy(mqttTopic, configJson["mqttTopic"] | "", sizeof(mqttTopic));
    publishInterval = configJson["publishInterval"] | 5000;

    Serial.println("[Config] Config loaded successfully.");
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

const char *Config::getMQTTClientName() const
{
    return mqttClientName;
}

const char *Config::getMQTTUsername() const
{
    return mqttUsername;
}

const char *Config::getMQTTPassword() const
{
    return mqttPassword;
}

const char *Config::getMQTTTopic() const
{
    return mqttTopic;
}

unsigned int Config::getPublishInterval() const
{
    return publishInterval;
}
