# PayPer – RFID Payment Terminal

An ESP32-based contactless payment terminal that uses an RC522 RFID reader, a 4×4 keypad, an SSD1306 OLED display, and Firebase Realtime Database as the back-end. A companion web application (see `/website`) handles account management and transaction processing.

---

## How It Works

```
Tap card  →  Enter PIN  →  Enter amount  →  Transaction result
```

1. **Card scan** – the RC522 reads the card UID and pushes it to Firebase (`rfid/data/uid`). The web-app responds with `rfid/data/isExist`.
2. **PIN entry** – the user types a PIN on the keypad and presses **A** to confirm. The firmware pushes it to `rfid/password/key`. The web-app responds with `rfid/password/isCorrect`.
3. **Amount entry** – the user types an amount and presses **A**. The firmware pushes it to `rfid/amount/amount`. The web-app responds with `rfid/amount/isDoable`.
4. **Result** – the OLED shows success or failure. Firebase fields are cleared for the next session.

---

## Hardware

| Component | Notes |
|-----------|-------|
| ESP32 dev board | Any 38-pin variant |
| RC522 RFID reader | SPI |
| 4×4 matrix keypad | |
| SSD1306 OLED 128×64 | I²C, address `0x3C` |
| Green LED | GPIO 12 |
| Red LED | GPIO 2 |
| Buzzer | GPIO 16 |

### Circuit Diagram

<!-- Add your circuit image below -->
![Circuit Diagram](circuit.png)

### Pin Connections

#### RC522 (SPI)
| RC522 | ESP32 |
|-------|-------|
| SDA (SS) | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| RST | GPIO 0 |
| 3.3V | 3.3V |
| GND | GND |

#### Keypad
| Row / Col | ESP32 GPIO |
|-----------|-----------|
| Row 1–4 | 14, 27, 26, 25 |
| Col 1–4 | 4, 32, 13, 15 |

#### OLED (I²C)
| OLED | ESP32 |
|------|-------|
| SDA | GPIO 21 |
| SCL | GPIO 22 |

---

## Project Structure

```
payper-rfid/
├── platformio.ini          # Build configuration & library dependencies
├── src/
│   ├── config.h            # ← PUT YOUR CREDENTIALS HERE
│   ├── main.cpp            # setup() / loop() + state machine
│   ├── wifi_helper.h/.cpp  # Wi-Fi connection with LED feedback
│   ├── firebase_helper.h/.cpp  # Firebase read/write wrappers
│   └── display_helper.h/.cpp   # OLED helper functions
└── website/                # Companion web application (add your files here)
```

---

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- A Firebase project with Realtime Database enabled

### 1 – Configure credentials

Open `src/config.h` and fill in:

```cpp
#define WIFI_SSID     "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define API_KEY       "your_firebase_api_key"
#define DATABASE_URL  "https://your-project-default-rtdb.firebaseio.com/"
```

> **Security note:** Never commit real credentials to a public repository. Consider using a `.env` approach or PlatformIO's `extra_scripts` to inject secrets at build time.

### 2 – Firebase Database Rules (development)

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

Tighten these rules before going to production.

### 3 – Build & flash

```bash
# Install dependencies and compile
pio run

# Flash to connected ESP32
pio run --target upload

# Open serial monitor
pio device monitor
```

---

## Firebase Data Schema

```
rfid/
├── data/
│   ├── uid        (string)  — card UID written by firmware
│   ├── count      (int)     — scan counter
│   └── isExist    (bool)    — written by web-app: card registered?
├── password/
│   ├── key        (string)  — PIN written by firmware
│   └── isCorrect  (bool)    — written by web-app: PIN matches?
└── amount/
    ├── amount     (string)  — amount written by firmware
    └── isDoable   (bool)    — written by web-app: sufficient balance?
```

---

## Contributing

Pull requests are welcome. For major changes please open an issue first.

---

## License

MIT
