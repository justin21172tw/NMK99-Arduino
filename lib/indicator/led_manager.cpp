#include "led_manager.h"
#include "config.h"

LedManager::LedManager(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin)
    : red_pin_(red_pin), green_pin_(green_pin), blue_pin_(blue_pin) {}

void LedManager::Begin() {
  pinMode(red_pin_, OUTPUT);
  pinMode(green_pin_, OUTPUT);
  pinMode(blue_pin_, OUTPUT);
  SetColor(false, false, false);
}

void LedManager::SetStatus(HumidityStatus status) {
  if (!Config::Features::ENABLE_LED) {
    SetColor(false, false, false);
    return;
  }

  switch (status) {
    case HumidityStatus::STATUS_LOW:
      SetColor(false, false, true);
      break;
    case HumidityStatus::STATUS_NORMAL:
      SetColor(false, true, false);
      break;
    case HumidityStatus::STATUS_HIGH:
      SetColor(true, false, false);
      break;
    case HumidityStatus::STATUS_ERROR:
      SetColor(true, false, true);
      break;
  }
}

void LedManager::SetColor(bool red_on, bool green_on, bool blue_on) {
  uint8_t brightness = Config::Features::LED_BRIGHTNESS;
  analogWrite(red_pin_, red_on ? brightness : 0);
  analogWrite(green_pin_, green_on ? brightness : 0);
  analogWrite(blue_pin_, blue_on ? brightness : 0);
}
