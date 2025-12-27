#include "Locker.h"
#include <LittleFS.h>
#include <FS.h>

Locker::Locker(LCD_I2C *lcd, Relay *relay, Button *btnPlus, Button *btnMinus, Button *btnOk)
        : _lcd(lcd), _relay(relay), _btnPlus(btnPlus), _btnMinus(btnMinus), _btnOk(btnOk),
            _state(State::Open), _configSeconds(30), _timerRunning(false), _timerEndMs(0),
            _lastSaveMs(0), _saveIntervalMs(15 * 60 * 1000), _manualUnlockEndMs(0), _manualUnlockDurationMs(1000) {} // default 15 minutes, 1s pulse

void Locker::begin() {
    // Ensure peripherals are initialised by caller before calling begin
    // Check if there was a saved timer (persistence). If yes resume and go to Closed.
    uint32_t remainingMs = 0;
    // init LittleFS (safe to call repeatedly)
    if (!LittleFS.begin()) {
        // If LittleFS fails, continue without persistence
        Serial.println("[Locker] ERROR: LittleFS init failed");
    } else {
        Serial.println("[Locker] LittleFS initialized");
    }

    if (readSavedTimer(remainingMs) && remainingMs > 0) {
        // Start timer with saved remaining milliseconds (rounded up to seconds)
        Serial.print("[Locker] Resuming saved timer: ");
        Serial.print(remainingMs);
        Serial.println(" ms");
        startTimer((remainingMs + 999) / 1000);
        goToState(State::Closed);
    } else {
        Serial.println("[Locker] No saved timer found, starting in Open state");
        goToState(State::Open);
    }
}

void Locker::update() {
    // Update buttons
    if (_btnPlus) _btnPlus->update();
    if (_btnMinus) _btnMinus->update();
    if (_btnOk) _btnOk->update();

    // Handle button events
    if (_btnPlus && _btnPlus->wasPressed()) {
        if (_state == State::Open) {
            goToState(State::TimeConfig);
        }
        if (_state == State::TimeConfig) {
            increaseConfig();
            displayConfig();
        }
    }

    if (_btnMinus && _btnMinus->wasPressed()) {
        if (_state == State::Open) {
            goToState(State::TimeConfig);
        }
        else if (_state == State::TimeConfig) {
            // If config is already 0 and user presses minus, go back to Open
            if (_configSeconds == 0) {
                goToState(State::Open);
            } else {
                decreaseConfig();
                displayConfig();
            }
        }
        else if (_state == State::TimeConfirm) {
            goToState(State::TimeConfig);
        }
    }

    if (_btnOk && _btnOk->wasPressed()) {
        if (_state == State::Open) {
            // Manual unlock pulse: activate relay briefly
            pulseUnlock(_manualUnlockDurationMs);
        } else if (_state == State::TimeConfig) {
            goToState(State::TimeConfirm);
        } else if (_state == State::TimeConfirm) {
            // Confirm and lock
            lockDoor();
            startTimer(_configSeconds);
            goToState(State::Closed);
        }
    }

    // Timer expiry handling
    if (_timerRunning) {
        displayTimer();
        uint32_t now = millis();

        // Periodic persistence: save remaining ms every _saveIntervalMs
        if (_lastSaveMs == 0 || (uint32_t)(now - _lastSaveMs) >= _saveIntervalMs) {
            uint32_t remainingMs = (_timerEndMs > now) ? (_timerEndMs - now) : 0;
            saveTimer(remainingMs);
            _lastSaveMs = now;
        }

        // handle wrap-around: comparison safe with unsigned
        if (now >= _timerEndMs) {
            // timer expired
            stopTimer();
            // pulse unlock for the configured manual duration
            pulseUnlock(_manualUnlockDurationMs);
            goToState(State::Open);
            clearSavedTimer();
        }
    }

    // Handle manual unlock pulse timeout (only when not in Closed timer)
    if (_manualUnlockEndMs != 0) {
        uint32_t now2 = millis();
        if (now2 >= _manualUnlockEndMs) {
            // end of pulse -> lock again
            lockDoor();
            _manualUnlockEndMs = 0;
        }
    }
}

uint32_t Locker::getConfigSeconds() const {
    return _configSeconds;
}

void Locker::goToState(State s) {
    _state = s;
    switch (s) {
        case State::Open:
            Serial.println("[Locker] State -> OPEN");
            if (_lcd) {
                _lcd->clear();
                _lcd->printLine(0, "State: OPEN");
            }
            break;
        case State::TimeConfig:
            Serial.println("[Locker] State -> TIME_CONFIG");
            if (_lcd) {
                _lcd->clear();
                _lcd->printLine(0, "Configure time");
                displayConfig();
            }
            break;
        case State::TimeConfirm:
            Serial.println("[Locker] State -> TIME_CONFIRM");
            if (_lcd) {
                _lcd->clear();
                // show configured time on line 0 and instruction on line 1
                displayConfig(0);
                _lcd->printLine(1, "Press OK to lock");
            }
            break;
        case State::Closed:
            Serial.println("[Locker] State -> CLOSED");
            if (_lcd) {
                _lcd->clear();
                _lcd->printLine(0, "State: CLOSED");
                _lcd->printLine(1, "Locked");
            }
            break;
    }
}

void Locker::displayConfig(uint8_t line) {
    if (!_lcd) return;
    char buf[17];
    uint32_t s = _configSeconds;
    uint32_t mins = s / 60;
    uint32_t secs = s % 60;
    // Compact format to avoid snprintf warnings, e.g. "Time 05m30s"
    snprintf(buf, sizeof(buf), "Time %02um%02us", (unsigned)mins, (unsigned)secs);
    _lcd->printLine(line, String(buf));
}

void Locker::displayTimer() {
    if (!_lcd || !_timerRunning) return;
    char buf[17];
    uint32_t now = millis();
    uint32_t remainingMs = (_timerEndMs > now) ? (_timerEndMs - now) : 0;
    uint32_t totalSecs = (remainingMs + 999) / 1000; // round up
    uint32_t mins = totalSecs / 60;
    uint32_t secs = totalSecs % 60;
    // Compact format to avoid snprintf warnings, e.g. "Remain 05m30s"
    snprintf(buf, sizeof(buf), "Remain %02um%02us", (unsigned)mins, (unsigned)secs);
    _lcd->printLine(1, String(buf));
}

void Locker::increaseConfig() {
    // increase by 10 minute steps, cap at 24h
    uint32_t step = 10 * 60; // 10 minutes in seconds
    uint32_t maxS = 24UL * 3600UL;
    _configSeconds = min(_configSeconds + step, maxS);
    Serial.print("[Locker] Config increased to: ");
    Serial.print(_configSeconds / 60);
    Serial.println(" minutes");
}

void Locker::decreaseConfig() {
    uint32_t step = 10 * 60; // 10 minutes
    if (_configSeconds > step) _configSeconds -= step;
    else _configSeconds = 0;
    Serial.print("[Locker] Config decreased to: ");
    Serial.print(_configSeconds / 60);
    Serial.println(" minutes");
}

void Locker::lockDoor() {
    Serial.println("[Locker] Locking door...");
    if (_relay) _relay->off();
}

void Locker::unlockDoor() {
    Serial.println("[Locker] Unlocking door...");
    if (_relay) _relay->on();
}

void Locker::pulseUnlock(uint32_t durationMs) {
    if (!_relay) return;
    Serial.print("[Locker] Manual unlock pulse for ");
    Serial.print(durationMs);
    Serial.println(" ms");
    _relay->on();
    uint32_t now = millis();
    _manualUnlockEndMs = now + durationMs;
}

void Locker::startTimer(uint32_t seconds) {
    if (seconds == 0) return;
    uint32_t now = millis();
    // convert seconds to ms, watch for overflow
    uint32_t durationMs = seconds * 1000UL;
    _timerEndMs = now + durationMs;
    _timerRunning = true;
    // save remaining ms for persistence (atomic write)
    saveTimer(durationMs);
    _lastSaveMs = now;
    Serial.print("[Locker] Timer started: ");
    Serial.print(seconds);
    Serial.println(" seconds");
}

void Locker::stopTimer() {
    Serial.println("[Locker] Timer stopped");
    _timerRunning = false;
    _timerEndMs = 0;
    _lastSaveMs = 0;
}

// Persistence stubs: user can implement using EEPROM or file system.
bool Locker::readSavedTimer(uint32_t &remainingMs) {
    remainingMs = 0;
    const char *path = "/locker_timer.bin";
    if (!LittleFS.exists(path)) {
        Serial.println("[Locker] No saved timer file found");
        return false;
    }
    File f = LittleFS.open(path, "r");
    if (!f) {
        Serial.println("[Locker] ERROR: Failed to open timer file");
        return false;
    }
    if (f.size() < 4) {
        f.close();
        Serial.println("[Locker] ERROR: Timer file corrupted (invalid size)");
        return false;
    }
    uint32_t val = 0;
    // read 4 bytes (little endian)
    val = (uint8_t)f.read();
    val |= ((uint32_t)(uint8_t)f.read()) << 8;
    val |= ((uint32_t)(uint8_t)f.read()) << 16;
    val |= ((uint32_t)(uint8_t)f.read()) << 24;
    f.close();
    remainingMs = val;
    Serial.print("[Locker] Saved timer read: ");
    Serial.print(remainingMs);
    Serial.println(" ms");
    return true;
}

void Locker::saveTimer(uint32_t remainingMs) {
    const char *tmp = "/locker_timer.bin.tmp";
    const char *path = "/locker_timer.bin";
    // Write to temp file then rename for atomicity
    File f = LittleFS.open(tmp, "w");
    if (!f) {
        Serial.println("[Locker] ERROR: Failed to save timer file");
        return;
    }
    // write 4 bytes little endian
    f.write((uint8_t)(remainingMs & 0xFF));
    f.write((uint8_t)((remainingMs >> 8) & 0xFF));
    f.write((uint8_t)((remainingMs >> 16) & 0xFF));
    f.write((uint8_t)((remainingMs >> 24) & 0xFF));
    f.flush();
    f.close();
    // remove old and rename
    if (LittleFS.exists(path)) LittleFS.remove(path);
    LittleFS.rename(tmp, path);
    Serial.print("[Locker] Timer saved: ");
    Serial.print(remainingMs);
    Serial.println(" ms");
}

void Locker::clearSavedTimer() {
    const char *path = "/locker_timer.bin";
    if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        Serial.println("[Locker] Saved timer cleared");
    }
}
