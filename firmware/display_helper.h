#pragma once

#include <Adafruit_SSD1306.h>
#include "config.h"

// Initialise the OLED; halts on failure.
void displayInit(Adafruit_SSD1306 &display);

// Clear screen and print a single line of text at (x=0, y=10).
void displayMessage(Adafruit_SSD1306 &display, const char *msg);

// Print a second line (x=20, y=35) without clearing – used to show live input.
void displayInputLine(Adafruit_SSD1306 &display, const String &text);
