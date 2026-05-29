#include "led_manager.h"
#include "config.h"

LedManager::LedManager(uint8_t pin, uint8_t num_pixels)
    : strip_(num_pixels, pin, NEO_GRB + NEO_KHZ800) {}

void LedManager::Begin() {
  strip_.begin();
  strip_.setBrightness(Config::Features::LED_BRIGHTNESS);
  strip_.show(); // Initialize all pixels to 'off'
}

void LedManager::SetStatus(HumidityStatus status) {
  if (!Config::Features::ENABLE_LED) {
    SetPixelColor(0, 0, 0);
    return;
  }

  switch (status) {
    case HumidityStatus::STATUS_LOW:
      SetPixelColor(0, 0, 255); // Blue
      break;
    case HumidityStatus::STATUS_NORMAL:
      SetPixelColor(0, 255, 0); // Green
      break;
    case HumidityStatus::STATUS_HIGH:
      SetPixelColor(255, 0, 0); // Red
      break;
    case HumidityStatus::STATUS_ERROR:
      SetPixelColor(255, 0, 255); // Purple
      break;
  }
}

void LedManager::SetPixelColor(uint8_t r, uint8_t g, uint8_t b) {
  strip_.setPixelColor(0, strip_.Color(r, g, b));
  strip_.show();
}
