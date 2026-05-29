#include "display_manager.h"

#include "logger.h"

DisplayManager::DisplayManager(uint16_t width, uint16_t height, TwoWire* wire, int8_t reset_pin)
    : display_(width, height, wire, reset_pin), is_ready_(false) {}

bool DisplayManager::Begin(uint8_t i2c_address) {
  is_ready_ = display_.begin(SSD1306_SWITCHCAPVCC, i2c_address);
  if (!is_ready_) {
    Logger::Error(String("OLED init failed at address 0x") + String(i2c_address, HEX));
    return false;
  }

  display_.clearDisplay();
  display_.setTextColor(SSD1306_WHITE);
  display_.setTextSize(1);
  display_.setCursor(0, 0);
  display_.println("OLED Ready");
  display_.display();
  return true;
}

void DisplayManager::ShowReading(const DhtReading& reading, HumidityStatus status, bool wifi_ok, bool firebase_ok) {
  if (!is_ready_) {
    return;
  }

  display_.clearDisplay();
  
  // Header / Temperature label
  display_.setTextSize(1);
  display_.setCursor(0, 0);
  display_.print("TEMPERATURE");

  // Connection Status Icons (Top Right)
  display_.setCursor(85, 0);
  display_.print(wifi_ok ? "W+" : "W-");
  display_.setCursor(110, 0);
  display_.print(firebase_ok ? "F+" : "F-");
  
  // Temperature value (Size 2)
  display_.setTextSize(2);
  display_.setCursor(0, 10);
  display_.printf("%.1f C", reading.temperature_c);

  // Humidity label
  display_.setTextSize(1);
  display_.setCursor(0, 30);
  display_.print("HUMIDITY");
  
  // Humidity value (Size 2)
  display_.setTextSize(2);
  display_.setCursor(0, 40);
  display_.printf("%.1f %%", reading.humidity);

  // Status footer
  display_.setTextSize(1);
  display_.setCursor(0, 57);
  display_.printf("STATUS: %s", StatusText(status));
  
  display_.display();
}

void DisplayManager::ShowError(const String& message) {
  if (!is_ready_) {
    return;
  }

  display_.clearDisplay();
  display_.setTextSize(2);
  display_.setCursor(0, 0);
  display_.println("SENSOR");
  display_.println("ERROR");
  
  display_.setTextSize(1);
  display_.setCursor(0, 40);
  display_.println(message);
  display_.display();
}

const char* DisplayManager::StatusText(HumidityStatus status) const {
  switch (status) {
    case HumidityStatus::STATUS_LOW:
      return "LOW";
    case HumidityStatus::STATUS_NORMAL:
      return "NORMAL";
    case HumidityStatus::STATUS_HIGH:
      return "HIGH";
    case HumidityStatus::STATUS_ERROR:
      return "ERROR";
  }
  return "UNKNOWN";
}
