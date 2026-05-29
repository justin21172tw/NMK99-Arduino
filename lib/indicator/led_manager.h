#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "app_types.h"

class LedManager {
 public:
  LedManager(uint8_t pin, uint8_t num_pixels = 1);
  void Begin();
  void SetStatus(HumidityStatus status);

 private:
  Adafruit_NeoPixel strip_;
  void SetPixelColor(uint8_t r, uint8_t g, uint8_t b);
};

#endif
