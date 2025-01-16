#include "sdmanager.h"

SDManager::SDManager(int csPin):
    csPin(csPin)
{
}

/// @brief Start the SD system
/// @return Boolean of success
bool SDManager::begin()
{
    return SD.begin(csPin);
}

/// @brief Append data to the data file on the SD
/// @param data The string data to append
void SDManager::save(const char *data)
{
    File file = SD.open("data.txt", FILE_APPEND);
    if (file) {
        file.println(data);
        file.close();
    } else {
        Serial.println("Failed to append data to file.");
    }
}

/// @brief Process each stored line and send to a function, before deleting the data file.
/// @param callback Function to receive each stored line.
void SDManager::processLines(std::function<void(const char*)> callback)
{
    File file = SD.open("data.txt", FILE_READ);
    if (file) {
        Serial.println("Reading stored data...");
        while (file.available()) {
            char buffer[256];
            size_t len = file.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
            buffer[len] = '\0';
            callback(buffer);
        }
        file.close();
        SD.remove("data.txt");
        Serial.println("Stored data cleared.");
    }
}
