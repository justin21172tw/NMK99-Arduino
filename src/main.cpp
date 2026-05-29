#include <Arduino.h>
#include <Wire.h>

#include "app_types.h"
#include "config.h"
#include "display/display_manager.h"
#include "hardware/hardware_probe.h"
#include "indicator/led_manager.h"
#include "logger.h"
#include "sensor/dht_manager.h"

namespace {
DhtManager g_dht(Config::Pins::DHT11, Config::Dht::TYPE);
DisplayManager g_display(Config::Display::WIDTH, Config::Display::HEIGHT, &Wire,
                         Config::Display::RESET_PIN);
LedManager g_leds(Config::Pins::LED_RED, Config::Pins::LED_GREEN, Config::Pins::LED_BLUE);

unsigned long g_last_read_ms = 0;
bool g_display_ready = false;

HumidityStatus EvaluateHumidity(float humidity) {
  if (humidity > Config::Humidity::HIGH_THRESHOLD) {
    return HumidityStatus::HIGH;
  }
  if (humidity < Config::Humidity::LOW_THRESHOLD) {
    return HumidityStatus::LOW;
  }
  return HumidityStatus::NORMAL;
}
}  // namespace

void setup() {
  Logger::Begin(115200);
  delay(500);
  Logger::Info("System booting...");

  g_leds.Begin();

  Wire.begin(Config::Pins::OLED_SDA, Config::Pins::OLED_SCL);
  Logger::Info(String("I2C started (SDA=") + Config::Pins::OLED_SDA + ", SCL=" +
               Config::Pins::OLED_SCL + ")");

  g_dht.Begin();

  auto devices = HardwareProbe::ScanI2CDevices(Wire);
  int display_address =
      HardwareProbe::FindDisplayAddress(devices, Config::Display::PREFERRED_I2C_ADDRESS);

  if (display_address >= 0) {
    g_display_ready = g_display.Begin(static_cast<uint8_t>(display_address));
  } else {
    Logger::Warning("OLED address not found during I2C scan");
  }

  HardwareProbe::ProbeDht(g_dht);
  Logger::Info("Setup complete");
}

void loop() {
  const unsigned long now = millis();
  if (now - g_last_read_ms < Config::Dht::READ_INTERVAL_MS) {
    delay(Config::Timing::LOOP_DELAY_MS);
    return;
  }
  g_last_read_ms = now;

  DhtReading reading{};
  if (!g_dht.Read(reading)) {
    Logger::Error("DHT read failed");
    g_leds.SetStatus(HumidityStatus::SENSOR_ERROR);
    if (g_display_ready) {
      g_display.ShowError("DHT read failed");
    }
    return;
  }

  HumidityStatus status = EvaluateHumidity(reading.humidity);
  g_leds.SetStatus(status);

  if (g_display_ready) {
    g_display.ShowReading(reading, status);
  }

  Logger::Info(String("T=") + String(reading.temperature_c, 1) + "C, H=" +
               String(reading.humidity, 1) + "%");
}
