# Smart Smartphone Locker

Smart timed-locker controller for ESP8266 (Wemos D1 Mini) using an LCD, buttons, and a relay.

## Overview

This project implements a finite state machine (FSM) to control a lock with:

- Time configuration via buttons (10-minute step)
- Automatic locking with countdown timer
- Manual unlock via a relay pulse (1s)
- Robust persistence (LittleFS) to resume after power loss
- 16x2 I2C LCD output

## Hardware

- **MCU:** ESP8266 (Wemos D1 Mini)
- **Display:** 16x2 I2C LCD (default address 0x27)
- **Buttons:** 3 buttons (OK, Plus, Minus)
- **Relay:** 1 relay driving the lock
- **Lock:** compatible with pulse operation (HIGH = unlock, then back to LOW)

### Default wiring

```text
Button OK:    D5
Button Minus: D6
Button Plus:  D7
Relay Lock:   D8
LCD I2C:      SDA/SCL (D2/D1 on D1 Mini)
```

## Build & Upload (PlatformIO)

```bash
platformio run
platformio run --target upload
platformio device monitor
```

## State Machine

Main states:

1. **Open**: idle screen, manual unlock pulse available (OK)
2. **TimeConfig**: adjust duration (Plus/Minus)
3. **TimeConfirm**: confirm before locking
4. **Closed**: locked with active countdown

Full diagram: [docs/fsm.svg](docs/fsm.svg)

### Main transitions

- Open + Plus/Minus → enter config → TimeConfig
- Open + OK → pulse unlock (1s) → Open
- TimeConfig + Plus → +10 min → TimeConfig
- TimeConfig + Minus → -10 min (or → Open if already 0) → TimeConfig/Open
- TimeConfig + OK → confirm → TimeConfirm
- TimeConfirm + Minus → back to config → TimeConfig
- TimeConfirm + OK → lock + start timer → Closed
- Closed + timer elapsed → pulse unlock (1s) + clear saved timer → Open

## Usage

- **Boot:** shows `State: OPEN` (no time shown on the idle screen).
- **Manual unlock:** press OK in **Open** → relay HIGH for 1s then LOW.
- **Configure & lock:** Open → (Plus/Minus) → TimeConfig → OK → TimeConfirm → OK.
- **Cancel:**
  - TimeConfig: if time is 0, Minus returns to Open
  - TimeConfirm: Minus returns to TimeConfig

## Persistence (LittleFS)

- **Storage:** LittleFS
- **File:** `/locker_timer.bin` (4 bytes, little-endian `uint32_t`, remaining milliseconds)
- **Writes:**
  - when the timer starts
  - every 15 minutes while counting down
- **Boot resume:** if remaining time > 0, resumes into **Closed**.

### Limitations

- Power-off time is not compensated (no RTC/NTP).
- Resume accuracy is about ±15 minutes (save interval).

## Tuning

- Time step (10 minutes): adjust `step` in `increaseConfig()` / `decreaseConfig()`.
- Manual unlock pulse (1 second): `_manualUnlockDurationMs` in the `Locker` constructor.
- Periodic save interval (15 minutes): `_saveIntervalMs` in the `Locker` constructor.

## Project layout

```text
smart_smartphone_locker/
├── src/
│   └── main.cpp
├── lib/
│   ├── Button/
│   ├── LCD_I2C/
│   ├── Relay/
│   └── Locker/
├── test_apps/
│   ├── test_button/
│   ├── test_lcd_i2c/
│   ├── test_relay/
│   └── test_persistence/
├── docs/
│   └── fsm.svg
└── platformio.ini
```

## License

TBD
