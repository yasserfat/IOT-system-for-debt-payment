#include "display_helper.h"

void displayInit(Adafruit_SSD1306 &display) {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) { /* halt */ }
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void displayMessage(Adafruit_SSD1306 &display, const char *msg) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(msg);
  display.display();
}

void displayInputLine(Adafruit_SSD1306 &display, const String &text) {
  display.setCursor(20, 35);
  display.println(text);
  display.display();
}
