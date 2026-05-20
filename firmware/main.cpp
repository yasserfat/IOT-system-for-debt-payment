/*
 * PayPer – RFID Payment Terminal
 * ESP32  |  RC522  |  4×4 Keypad  |  SSD1306 OLED  |  Firebase RTDB
 *
 * Flow:
 *   1. Scan RFID card  →  UID sent to Firebase
 *   2. Firebase sets rfid/data/isExist = true/false
 *   3. User enters PIN on keypad  →  sent to Firebase
 *   4. Firebase sets rfid/password/isCorrect = true/false
 *   5. User enters amount  →  sent to Firebase
 *   6. Firebase sets rfid/amount/isDoable = true/false
 *   7. Transaction result shown on OLED
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include <Keypad.h>

#include "config.h"
#include "wifi_helper.h"
#include "firebase_helper.h"
#include "display_helper.h"

// ─── Hardware objects ─────────────────────────────────────────────────────────
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MFRC522          rfid(SS_PIN, RST_PIN);

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
uint8_t rowPins[KEYPAD_ROWS] = {14, 27, 26, 25};
uint8_t colPins[KEYPAD_COLS] = { 4, 32, 13, 15};
Keypad  keypad = Keypad(makeKeymap(keys), rowPins, colPins,
                        KEYPAD_ROWS, KEYPAD_COLS);

// ─── Session state ────────────────────────────────────────────────────────────
static bool   cardIsValid  = false;   // rfid/data/isExist
static bool   pinIsCorrect = false;   // rfid/password/isCorrect
static bool   txIsDoable   = false;   // rfid/amount/isDoable
static bool   amountSent   = false;   // true after 'A' pressed in amount step
static String pinBuffer    = "";
static String amountBuffer = "";

// ─── Helpers ──────────────────────────────────────────────────────────────────
static void beep() {
  tone(PIN_BUZZER, BEEP_FREQUENCY_HZ);
  delay(BEEP_DURATION_MS);
  noTone(PIN_BUZZER);
}

static void resetSession() {
  cardIsValid  = false;
  pinIsCorrect = false;
  txIsDoable   = false;
  amountSent   = false;
  pinBuffer    = "";
  amountBuffer = "";
}

// Clear Firebase fields so the web-app starts fresh for the next session.
static void clearFirebaseSession() {
  firebaseSetString("rfid/data",     "uid",      "");
  firebaseSetString("rfid/password", "key",      "");
  firebaseSetString("rfid/amount",   "amount",   "");
}

// ─── Step handlers ────────────────────────────────────────────────────────────
static void handleCardScan() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  beep();
  resetSession();
  clearFirebaseSession();

  // Build UID string.
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  Serial.println("[RFID] UID: " + uid);

  // Push UID to Firebase.
  firebaseSetString("rfid/data", "uid", uid);

  // Check whether the card exists in the database.
  cardIsValid = firebaseGetBool("/rfid/data/isExist");

  if (cardIsValid) {
    displayMessage(display, "Card valid.\nEnter your PIN:");
  } else {
    displayMessage(display, "Card not recognised.\nPlease try again.");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

static void handlePinEntry() {
  // Refresh the live input on the second line.
  display.setCursor(20, 35);
  display.println(pinBuffer);
  display.display();

  char key = keypad.getKey();
  if (key == NO_KEY) return;

  if (key == 'A') {
    // Submit PIN.
    Serial.println("[PIN] Submitted: " + pinBuffer);
    firebaseSetString("rfid/password", "key", pinBuffer);
    pinBuffer = "";

    pinIsCorrect = firebaseGetBool("/rfid/password/isCorrect");
    if (pinIsCorrect) {
      displayMessage(display, "PIN correct.\nEnter amount:");
    } else {
      displayMessage(display, "Wrong PIN.\nTry again:");
    }
  } else {
    pinBuffer += key;
    Serial.println("[PIN] Key: " + String(key));
  }
}

static void handleAmountEntry() {
  if (!amountSent) {
    // Show prompt + live input once per frame.
    display.setCursor(20, 35);
    display.println(amountBuffer);
    display.display();
  }

  char key = keypad.getKey();
  if (key == NO_KEY) return;

  if (key == 'A') {
    amountSent = true;
    Serial.println("[AMT] Submitted: " + amountBuffer);
    firebaseSetString("rfid/amount", "amount", amountBuffer);
    delay(FIREBASE_POLL_DELAY);

    txIsDoable   = firebaseGetBool("/rfid/amount/isDoable");
    amountBuffer = "";

    if (txIsDoable) {
      displayMessage(display, "Transaction\nsuccessful!");
      clearFirebaseSession();
      delay(3000);
      displayMessage(display, "Scan your card...");
      resetSession();
    } else {
      displayMessage(display, "Insufficient\nfunds or error.");
    }
  } else {
    amountBuffer += key;
    Serial.println("[AMT] Key: " + String(key));
  }
}

// ─── Arduino entry points ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED,   OUTPUT);

  displayInit(display);
  displayMessage(display, "Connecting to Wi-Fi...");

  if (!wifiConnect(PIN_LED_RED, PIN_LED_GREEN)) {
    displayMessage(display, "Wi-Fi failed.\nCheck credentials.");
    return;
  }

  displayMessage(display, "Connected!\nScan your card...");

  firebaseInit();

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("[RFID] Reader ready.");
}

void loop() {
  // Step 1 – always watching for a new card.
  handleCardScan();

  // Step 2 – PIN entry (card valid, PIN not yet confirmed).
  if (cardIsValid && !pinIsCorrect) {
    handlePinEntry();
  }

  // Step 3 – Amount entry (card valid, PIN confirmed, tx not yet sent).
  if (cardIsValid && pinIsCorrect && !txIsDoable && !amountSent) {
    if (!amountSent) {
      // Print the prompt once when first entering this state.
      static bool promptShown = false;
      if (!promptShown) {
        displayMessage(display, "Enter amount:");
        promptShown = true;
      }
    }
    handleAmountEntry();
  }

  // Reset the "amount prompt shown" flag when leaving this state.
  // (Handled implicitly by resetSession clearing amountSent.)
}
