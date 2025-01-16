#include "sensormanager.h"

SensorManager::SensorManager(DHT_Async &dht, HardwareSerial &gpsSerial):
    dht(dht),
    gpsSerial(gpsSerial)
{
}

/// @brief Read the temperature and humidity from the DHT sensor
/// @param temperature Reference to a float to record temperature
/// @param humidity Reference to a float to record humidity
/// @return Boolean on if a measurement was taken
bool SensorManager::readDHT(float &temperature, float &humidity)
{
    static unsigned long measurement_timestamp = millis();

    if (millis() - measurement_timestamp > 4000ul) {
        if (dht.measure(&temperature, &humidity)) {
            measurement_timestamp = millis();
            return true;
        }
    }
    return false;
}

/// @brief Consume data from GPS device and parse.
/// @return Boolean on if location, date, and time are updated.
bool SensorManager::readGPS()
{
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
    return (gps.location.isUpdated() && gps.time.isUpdated() && gps.date.isUpdated());
}
