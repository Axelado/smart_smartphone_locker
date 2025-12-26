#include <Arduino.h>
#include <LittleFS.h>

const char *path = "/locker_timer.bin";

void setup() {
    Serial.begin(115200);
    delay(100);
    if (!LittleFS.begin()) {
        Serial.println("LittleFS.begin() failed");
        while (1) delay(1000);
    }

    // Write a test value (e.g., 90 seconds -> 90000 ms)
    uint32_t remainingMs = 90000;
    const char *tmp = "/locker_timer.bin.tmp";
    File f = LittleFS.open(tmp, "w");
    if (!f) {
        Serial.println("Failed to open tmp file for writing");
        return;
    }
    f.write((uint8_t)(remainingMs & 0xFF));
    f.write((uint8_t)((remainingMs >> 8) & 0xFF));
    f.write((uint8_t)((remainingMs >> 16) & 0xFF));
    f.write((uint8_t)((remainingMs >> 24) & 0xFF));
    f.flush();
    f.close();

    if (LittleFS.exists(path)) LittleFS.remove(path);
    LittleFS.rename(tmp, path);
    Serial.println("Wrote test timer (90000 ms)");

    // Read back
    File r = LittleFS.open(path, "r");
    if (!r) {
        Serial.println("Failed to open file for reading");
        return;
    }
    if (r.size() < 4) {
        Serial.println("File too small");
        r.close();
        return;
    }
    uint32_t val = 0;
    val = (uint8_t)r.read();
    val |= ((uint32_t)(uint8_t)r.read()) << 8;
    val |= ((uint32_t)(uint8_t)r.read()) << 16;
    val |= ((uint32_t)(uint8_t)r.read()) << 24;
    r.close();

    Serial.print("Read back ms: ");
    Serial.println(val);
}

void loop() {
    // nothing
}
