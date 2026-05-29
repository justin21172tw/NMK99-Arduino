#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>

#include "app_types.h"

class LedManager {
 public:
  LedManager(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin);
  void Begin();
  void SetStatus(HumidityStatus status);

 private:
  uint8_t red_pin_;
  uint8_t green_pin_;
  uint8_t blue_pin_;
  void SetColor(bool red_on, bool green_on, bool blue_on);
};

#endif
