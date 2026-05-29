#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
namespace Pins {
constexpr uint8_t DHT11 = 4;
constexpr uint8_t OLED_SDA = 21;
constexpr uint8_t OLED_SCL = 22;
constexpr uint8_t LED_RED = 25;
constexpr uint8_t LED_GREEN = 26;
constexpr uint8_t LED_BLUE = 27;
}  // namespace Pins

namespace Display {
constexpr uint16_t WIDTH = 128;
constexpr uint16_t HEIGHT = 64;
constexpr uint8_t RESET_PIN = 255;
constexpr uint8_t PREFERRED_I2C_ADDRESS = 0x3C;
}  // namespace Display

namespace Dht {
constexpr uint8_t TYPE = 11;
constexpr uint32_t READ_INTERVAL_MS = 2000;
}  // namespace Dht

namespace Humidity {
constexpr float LOW_THRESHOLD = 40.0f;
constexpr float HIGH_THRESHOLD = 70.0f;
}  // namespace Humidity

namespace Timing {
constexpr uint32_t LOOP_DELAY_MS = 100;
}  // namespace Timing
}  // namespace Config

#endif
