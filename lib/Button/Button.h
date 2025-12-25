#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin,
           uint32_t debounceMs = 50,
           uint32_t longPressMs = 800,
           uint32_t doubleClickMs = 400);

    void begin();
    void update();

    bool isPressed() const;

    bool wasPressed();
    bool wasLongPressed();
    bool wasDoubleClicked();

private:
    uint8_t _pin;

    uint32_t _debounceMs;
    uint32_t _longPressMs;
    uint32_t _doubleClickMs;

    bool _stableState;
    bool _lastReading;

    bool _pressedEvent;
    bool _longPressEvent;
    bool _doubleClickEvent;

    bool _longPressTriggered;

    uint32_t _lastDebounceTime;
    uint32_t _pressStartTime;
    uint32_t _lastReleaseTime;
};

#endif
