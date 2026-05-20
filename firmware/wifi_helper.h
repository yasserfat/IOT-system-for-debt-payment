#pragma once

#include <Arduino.h>

// Connect to Wi-Fi.
// Blinks redLedPin while waiting; turns on greenLedPin on success.
// Returns true on success, false if WIFI_TIMEOUT_MS is reached.
bool wifiConnect(int redLedPin, int greenLedPin);
