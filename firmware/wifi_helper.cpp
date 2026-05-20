#include "wifi_helper.h"
#include "config.h"

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

bool wifiConnect(int redLedPin, int greenLedPin) {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("[WiFi] Connecting");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < WIFI_TIMEOUT_MS) {
    digitalWrite(redLedPin, HIGH);
    delay(200);
    digitalWrite(redLedPin, LOW);
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Connection failed");
    return false;
  }

  Serial.print("[WiFi] Connected – IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(greenLedPin, HIGH);
  return true;
}
