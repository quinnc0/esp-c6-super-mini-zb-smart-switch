#pragma once
#include "config.h"
#include <Arduino.h>

// Blink LED with configurable pattern
void blinkLed(uint8_t pin, uint8_t count, uint16_t onMs, uint16_t offMs) {
  for(uint8_t i = 0; i < count; i++) {
    digitalWrite(pin, HIGH);
    delay(onMs);
    digitalWrite(pin, LOW);
    if (i < count - 1) { // Don't delay after last blink
      delay(offMs);
    }
  }
}

// Control RGB LED
void rgbLed(bool on) {
  uint8_t r = on ? 255 : 0;
  uint8_t g = on ? 0 : 0;
  uint8_t b = on ? 10 : 0;
  uint8_t brightness = 255; // Adjust brightness (0-255)
  rgbLedWrite(RGB_BUILTIN, r, g, b);
}