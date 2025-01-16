#ifndef SDMANAGER_H
#define SDMANAGER_H

#include <SD.h>
#include <ArduinoJson.h>

class SDManager
{
private:
    const int csPin;

public:
    SDManager(int csPin);
    bool begin();
    void save(const char* data);
    void processLines(std::function<void(const char*)> callback);
};

#endif