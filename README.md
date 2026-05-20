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

| Component | Quantity | Notes |
|-----------|:--------:|-------|
| ESP32 dev board | 1 | Any 38-pin variant |
| RC522 RFID reader | 1 | SPI interface |
| 4×4 matrix keypad | 1 | Membrane type |
| SSD1306 OLED 128×64 | 1 | I²C, address `0x3C` |
| Active buzzer | 1 | 5 V, GPIO 16 |
| Green LED | 1 | GPIO 12 — Wi-Fi connected / success |
| Red LED | 1 | GPIO 2 — Wi-Fi connecting / error |
| Resistor 220 Ω | 2 | Current-limiting for green and red LEDs |

### Circuit Diagram

<!-- Drop your circuit image here -->
![Circuit Diagram](circuit.png)

### Pin Connections

#### RC522 RFID Reader (SPI)

| RC522 Pin | ESP32 GPIO | Notes |
|-----------|-----------|-------|
| SDA (SS) | GPIO 5 | Chip select |
| SCK | GPIO 18 | SPI clock |
| MOSI | GPIO 23 | Master out |
| MISO | GPIO 19 | Master in |
| RST | GPIO 0 | Reset |
| 3.3V | 3.3V | **Do not use 5 V** |
| GND | GND | |

#### 4×4 Matrix Keypad

| Keypad Pin | ESP32 GPIO |
|------------|-----------|
| Row 1 | GPIO 14 |
| Row 2 | GPIO 27 |
| Row 3 | GPIO 26 |
| Row 4 | GPIO 25 |
| Col 1 | GPIO 4 |
| Col 2 | GPIO 32 |
| Col 3 | GPIO 13 |
| Col 4 | GPIO 15 |

The keypad library drives rows as outputs and reads columns as inputs with internal pull-ups; no external resistors needed on the keypad lines.

#### SSD1306 OLED (I²C)

| OLED Pin | ESP32 GPIO |
|----------|-----------|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| VCC | 3.3V |
| GND | GND |

#### LEDs & Resistors

| Component | ESP32 GPIO | Resistor | Wiring |
|-----------|-----------|----------|--------|
| Green LED (anode) | GPIO 12 | 220 Ω in series | GPIO 12 → 220 Ω → LED anode → LED cathode → GND |
| Red LED (anode) | GPIO 2 | 220 Ω in series | GPIO 2 → 220 Ω → LED anode → LED cathode → GND |

> A 220 Ω resistor limits current to ~15 mA on a 3.3 V output, which is within the ESP32's 40 mA per-pin maximum and gives comfortable LED brightness.

#### Active Buzzer

| Buzzer Pin | ESP32 GPIO | Notes |
|------------|-----------|-------|
| + (positive) | GPIO 16 | Drive HIGH to activate |
| − (negative) | GND | |

> Use an **active** buzzer (has internal oscillator). If you use a passive buzzer, the `tone()` call in the firmware is still correct but you may need to adjust the frequency.

---

## Project Structure

```
payper-rfid/
├── platformio.ini               # Build config & library dependencies
├── circuit.png                  # ← drop your circuit image here
├── src/                         # ESP32 firmware
│   ├── config.h                 # ← PUT YOUR CREDENTIALS HERE
│   ├── main.cpp                 # setup() / loop() + state machine
│   ├── wifi_helper.h/.cpp       # Wi-Fi connection with LED feedback
│   ├── firebase_helper.h/.cpp   # Firebase read/write wrappers
│   └── display_helper.h/.cpp    # OLED helper functions
└── website/                     # React web application
    └── src/
        ├── assets/              # Static images & icons per page
        │   ├── accounts/
        │   ├── landing/
        │   ├── overview/
        │   └── user-collect-data/
        ├── components/          # Reusable UI components
        │   ├── costum-ook/      # Custom hook(s) shared across pages
        │   ├── landing/         # Hero / marketing section pieces
        │   ├── main-navbar/     # Authenticated app top-bar
        │   ├── main-sidebar/    # Authenticated app side navigation
        │   ├── navbar/          # Public / landing page nav
        │   ├── sidebar/         # Secondary sidebar variant
        │   ├── table-all-transactions/  # Full transaction history table
        │   ├── table-expense/   # Filtered expense table
        │   ├── table-income/    # Filtered income table
        │   ├── transactions-item/       # Single transaction row/card
        │   └── wait-card/       # "Waiting for card scan" UI overlay
        └── pages/               # One folder = one route
            ├── accounts/        # Manage linked RFID cards & balances
            ├── landing/         # Public marketing / home page
            ├── login/           # Email + password sign-in
            ├── overview/        # Dashboard – balance, recent activity
            ├── register/        # New user registration
            ├── send-money/      # Initiate a transfer to another account
            ├── setting/         # Profile & app preferences
            ├── splash/          # Initial loading / branding screen
            ├── transactions/    # Full transaction history & filters
            └── user-collect-data/  # Onboarding – collect user details
```

---

## Web Application

The React app runs alongside the hardware and acts as the "bank" side of the system. It listens to Firebase RTDB for card scans from the terminal and responds with the authentication and balance decisions the firmware needs.

### Page overview

| Page | Purpose |
|------|---------|
| **Splash** | Branded loading screen shown on first launch |
| **Landing** | Public marketing page with product overview |
| **Register / Login** | Firebase Auth-backed account creation and sign-in |
| **User Collect Data** | Onboarding flow that gathers name, phone, etc. after registration |
| **Overview** | Main dashboard: current balance, quick stats, recent transactions |
| **Accounts** | List and manage RFID cards linked to the user's account |
| **Send Money** | Transfer funds to another PayPer account |
| **Transactions** | Full paginated history with income / expense filter tabs |
| **Settings** | Profile editing and app preferences |

### Component overview

| Component | Purpose |
|-----------|---------|
| `main-navbar` / `main-sidebar` | Shell navigation shown on all authenticated pages |
| `navbar` / `sidebar` | Lightweight nav used on public (pre-login) pages |
| `wait-card` | Overlay that appears while the terminal is scanning a card |
| `table-all-transactions` | Complete transaction list with sorting |
| `table-expense` / `table-income` | Filtered views of the same data |
| `transactions-item` | Individual transaction row rendered inside any of the tables |
| `costum-ook` | Shared custom React hook(s) (e.g. Firebase listener, auth guard) |

### Setup

```bash
cd website
npm install
npm run dev      # Vite dev server  (or: npm start for CRA)
```

Create a `.env` file in `website/` with your Firebase config:

```env
VITE_API_KEY=your_firebase_api_key
VITE_AUTH_DOMAIN=your-project.firebaseapp.com
VITE_DATABASE_URL=https://your-project-default-rtdb.firebaseio.com
VITE_PROJECT_ID=your-project-id
VITE_STORAGE_BUCKET=your-project.appspot.com
VITE_MESSAGING_SENDER_ID=your_sender_id
VITE_APP_ID=your_app_id
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
