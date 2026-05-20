#pragma once

// ─── Wi-Fi ───────────────────────────────────────────────────────────────────
#define WIFI_SSID     "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// ─── Firebase ────────────────────────────────────────────────────────────────
#define API_KEY      "your_firebase_api_key"
#define DATABASE_URL "https://your-project-default-rtdb.firebaseio.com/"

// ─── RFID (RC522) ────────────────────────────────────────────────────────────
#define SS_PIN  5
#define RST_PIN 0

// ─── OLED ────────────────────────────────────────────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_I2C_ADDR 0x3C

// ─── GPIO ────────────────────────────────────────────────────────────────────
#define PIN_LED_GREEN 12
#define PIN_LED_RED   2
#define PIN_BUZZER    16

// ─── Keypad (4×4) ────────────────────────────────────────────────────────────
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// ─── Timing ──────────────────────────────────────────────────────────────────
#define WIFI_TIMEOUT_MS      60000UL   // 60 s Wi-Fi connection timeout
#define BEEP_DURATION_MS     400       // RFID scan beep length
#define BEEP_FREQUENCY_HZ    1000      // RFID scan beep pitch
#define FIREBASE_POLL_DELAY  1000      // ms to wait after pushing amount
