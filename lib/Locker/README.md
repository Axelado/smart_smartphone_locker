# Locker Library

`Locker` is the FSM (Finite State Machine) library used by this project to drive a timed lock.

## Features

- 4 states: `Open`, `TimeConfig`, `TimeConfirm`, `Closed`
- Button event handling (Plus / Minus / OK)
- 16x2 I2C LCD output
- Relay control with **pulse unlock** (HIGH for a short time, then LOW)
- LittleFS persistence to resume a running timer after a reboot

## Public API

### Constructor

```cpp
Locker(LCD_I2C *lcd, Relay *relay, Button *btnPlus, Button *btnMinus, Button *btnOk);
```

### Methods

#### `void begin()`

Call from `setup()`.

- Initializes LittleFS
- Reads a saved timer (if present)
- If a saved timer exists, resumes into `Closed`
- Otherwise starts in `Open`

#### `void update()`

Call from `loop()`.

- Updates buttons
- Executes FSM transitions
- Updates countdown display while in `Closed`
- Performs periodic persistence while the timer is running (default: every 15 minutes)

#### `uint32_t getConfigSeconds() const`

Returns the configured duration (seconds).

## States & behavior

### `Open`

- Display: `State: OPEN` (no time shown on the idle screen)
- OK: `pulseUnlock(1s)` (manual unlock pulse)
- Plus/Minus: enter `TimeConfig`

### `TimeConfig`

- Display line 0: `Configure time`
- Display line 1: `Time 05m30s`
- Plus: increase by 10 minutes
- Minus: decrease by 10 minutes (or go back to `Open` if already 0)
- OK: go to `TimeConfirm`

### `TimeConfirm`

- Display line 0: `Time 05m30s`
- Display line 1: `Press OK to lock`
- Minus: back to `TimeConfig`
- OK: lock + start timer → `Closed`

### `Closed`

- Display line 0: `State: CLOSED`
- Display line 1: `Remain 05m30s` (updated each loop)
- When timer elapses: `pulseUnlock(1s)` + `clearSavedTimer()` → `Open`

## Persistence (LittleFS)

- File: `/locker_timer.bin`
- Format: 4 bytes little-endian `uint32_t` (remaining milliseconds)
- Atomic write: write to `*.tmp` then rename
- Save moments:
  - when the timer starts
  - every 15 minutes while counting down

### Limitations

- No RTC/NTP compensation: power-off time is not accounted for.
- Resume accuracy is about ±15 minutes.

## Defaults

| Setting | Value | Notes |
| ------- | ----- | ----- |
| time step | 10 minutes | Plus/Minus in `TimeConfig` |
| max time | 24 hours | clamp in `increaseConfig()` |
| unlock pulse | 1000 ms | relay HIGH duration |
| save interval | 15 minutes | periodic persistence |

## Example

```cpp
#include <Arduino.h>
#include "LCD_I2C.h"
#include "Button.h"
#include "Relay.h"
#include "Locker.h"

#define BUTTON_OK_PIN D5
#define BUTTON_MINUS_PIN D3
#define BUTTON_PLUS_PIN D7
#define RELAY_LOCK_PIN D8

LCD_I2C lcd(0x27, 16, 2);
Button buttonOk(BUTTON_OK_PIN);
Button buttonMinus(BUTTON_MINUS_PIN);
Button buttonPlus(BUTTON_PLUS_PIN);
Relay lockRelay(RELAY_LOCK_PIN, true);

Locker locker(&lcd, &lockRelay, &buttonPlus, &buttonMinus, &buttonOk);

void setup() {
  Serial.begin(115200);
  lcd.begin();
  buttonOk.begin();
  buttonMinus.begin();
  buttonPlus.begin();
  lockRelay.begin();
  locker.begin();
}

void loop() {
  locker.update();
  delay(10);
}
```
