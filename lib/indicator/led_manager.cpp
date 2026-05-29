#include "indicator/led_manager.h"

LedManager::LedManager(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin)
    : red_pin_(red_pin), green_pin_(green_pin), blue_pin_(blue_pin) {}

void LedManager::Begin() {
  pinMode(red_pin_, OUTPUT);
  pinMode(green_pin_, OUTPUT);
  pinMode(blue_pin_, OUTPUT);
  SetColor(false, false, false);
}

void LedManager::SetStatus(HumidityStatus status) {
  switch (status) {
    case HumidityStatus::LOW:
      SetColor(false, false, true);
      break;
    case HumidityStatus::NORMAL:
      SetColor(false, true, false);
      break;
    case HumidityStatus::HIGH:
      SetColor(true, false, false);
      break;
    case HumidityStatus::SENSOR_ERROR:
      SetColor(true, false, true);
      break;
  }
}

void LedManager::SetColor(bool red_on, bool green_on, bool blue_on) {
  digitalWrite(red_pin_, red_on ? HIGH : LOW);
  digitalWrite(green_pin_, green_on ? HIGH : LOW);
  digitalWrite(blue_pin_, blue_on ? HIGH : LOW);
}
