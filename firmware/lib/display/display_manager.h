#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "app_types.h"
#include "dht_manager.h"

class DisplayManager {
 public:
  DisplayManager(uint16_t width, uint16_t height, TwoWire* wire, int8_t reset_pin);
  bool Begin(uint8_t i2c_address);
  void ShowStatus(const String& message);
  void ShowReading(const DhtReading& reading, HumidityStatus status, bool wifi_ok, bool firebase_ok);
  void ShowError(const String& message);

 private:
  Adafruit_SSD1306 display_;
  bool is_ready_;
  const char* StatusText(HumidityStatus status) const;
};

#endif
