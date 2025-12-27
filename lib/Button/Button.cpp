#include "Button.h"

Button::Button(uint8_t pin,
               uint32_t debounceMs,
               uint32_t longPressMs,
               uint32_t doubleClickMs)
    : _pin(pin),
      _debounceMs(debounceMs),
      _longPressMs(longPressMs),
      _doubleClickMs(doubleClickMs),
      _stableState(HIGH),
      _lastReading(HIGH),
      _pressedEvent(false),
      _longPressEvent(false),
      _doubleClickEvent(false),
      _longPressTriggered(false),
      _lastDebounceTime(0),
      _pressStartTime(0),
      _lastReleaseTime(0) {}

void Button::begin() {
    pinMode(_pin, INPUT_PULLUP);
    Serial.print("[Button] Initialized on pin: ");
    Serial.println(_pin);
}

void Button::update() {
    bool reading = digitalRead(_pin);
    uint32_t now = millis();

    // Anti-rebond
    if (reading != _lastReading) {
        _lastDebounceTime = now;
    }

    if ((now - _lastDebounceTime) > _debounceMs) {

        // État stabilisé changé
        if (reading != _stableState) {
            _stableState = reading;

            // Appui (HIGH → LOW)
            if (_stableState == LOW) {
                _pressStartTime = now;
                _longPressTriggered = false;
            }

            // Relâchement (LOW → HIGH)
            else {
                uint32_t pressDuration = now - _pressStartTime;

                if (!_longPressTriggered) {
                    if ((now - _lastReleaseTime) < _doubleClickMs) {
                        _doubleClickEvent = true;
                        _lastReleaseTime = 0;
                    } else {
                        _pressedEvent = true;
                        _lastReleaseTime = now;
                    }
                }
            }
        }
    }

    // Détection appui long
    if (_stableState == LOW && !_longPressTriggered) {
        if ((now - _pressStartTime) >= _longPressMs) {
            _longPressEvent = true;
            _longPressTriggered = true;
            _lastReleaseTime = 0;
        }
    }

    _lastReading = reading;
}

bool Button::isPressed() const {
    return (_stableState == LOW);
}

bool Button::wasPressed() {
    if (_pressedEvent) {
        _pressedEvent = false;
        Serial.print("[Button] Pressed (pin: ");
        Serial.print(_pin);
        Serial.println(")");
        return true;
    }
    return false;
}

bool Button::wasLongPressed() {
    if (_longPressEvent) {
        _longPressEvent = false;
        Serial.print("[Button] Long pressed (pin: ");
        Serial.print(_pin);
        Serial.println(")");
        return true;
    }
    return false;
}

bool Button::wasDoubleClicked() {
    if (_doubleClickEvent) {
        _doubleClickEvent = false;
        Serial.print("[Button] Double clicked (pin: ");
        Serial.print(_pin);
        Serial.println(")");
        return true;
    }
    return false;
}
