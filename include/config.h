#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
namespace Features {
constexpr bool ENABLE_LED = true;  // enable or disable LED control
}  // namespace Features

namespace Pins {
constexpr uint8_t DHT11 = 13;
constexpr uint8_t OLED_SDA = 21;
constexpr uint8_t OLED_SCL = 22;
constexpr uint8_t LED_RED = 16;
constexpr uint8_t LED_GREEN = 17;
constexpr uint8_t LED_BLUE = 5;
}  // namespace Pins

namespace Display {
constexpr uint16_t WIDTH = 128;
constexpr uint16_t HEIGHT = 64;
constexpr uint8_t RESET_PIN = 255;  // whenever set to 225 or -1, it means not connected
constexpr uint8_t PREFERRED_I2C_ADDRESS = 0x3C;
}  // namespace Display

namespace Dht {
constexpr uint8_t TYPE = 11; //which means DHT11
constexpr uint32_t READ_INTERVAL_MS = 3000;  // read DHT11 every 3 seconds
}  // namespace Dht

namespace Humidity {
constexpr float LOW_THRESHOLD = 40.0f; // humudity lower then 40%
constexpr float HIGH_THRESHOLD = 70.0f;  // humudity lower then 70%
}  // namespace Humidity

namespace Timing {
constexpr uint32_t LOOP_DELAY_MS = 100;
}  // namespace Timing
}  // namespace Config

#endif
