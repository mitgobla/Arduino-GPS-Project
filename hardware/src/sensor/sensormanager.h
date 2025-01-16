#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <DHT_Async.h>
#include <TinyGPS++.h>

class SensorManager
{
private:
    DHT_Async& dht;
    HardwareSerial& gpsSerial;
    TinyGPSPlus gps;

public:
    SensorManager(DHT_Async& dht, HardwareSerial& gpsSerial);
    bool readDHT(float& temperature, float& humidity);
    bool readGPS();
};

#endif