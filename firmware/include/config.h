#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace Config {
namespace Features {
constexpr bool ENABLE_LED = true;  // enable or disable LED control
constexpr uint8_t LED_BRIGHTNESS = 51; // 1/5 of 255 for WS2812B
}  // namespace Features

namespace Pins {
constexpr uint8_t DHT11 = 33;
constexpr uint8_t OLED_SDA = 21;
constexpr uint8_t OLED_SCL = 22;
constexpr uint8_t WS2812B = 32;
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

namespace Network {
#ifdef WIFI_SSID
constexpr char SSID[] = WIFI_SSID;
#else
constexpr char SSID[] = "YOUR_WIFI_SSID";
#endif

#ifdef WIFI_PASSWORD
constexpr char PASSWORD[] = WIFI_PASSWORD;
#else
constexpr char PASSWORD[] = "YOUR_WIFI_PASSWORD";
#endif
} // namespace Network

namespace Firebase {
#ifdef FIREBASE_DATABASE_URL
constexpr char HOST[] = FIREBASE_DATABASE_URL;
#else
constexpr char HOST[] = "tah-monitor-default-rtdb.firebaseio.com";
#endif

#ifdef FIREBASE_API_KEY
constexpr char API_KEY[] = FIREBASE_API_KEY;
#else
constexpr char API_KEY[] = "AIzaSy...";
#endif
constexpr char DEVICE_ID[] = "NMK99-Node-01";
constexpr uint32_t UPLOAD_INTERVAL_MS = 10000; // Upload every 10 seconds
constexpr uint32_t HISTORY_INTERVAL_MS = 600000; // History every 10 minutes
} // namespace Firebase
}  // namespace Config

#endif
