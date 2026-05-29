#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include "app_types.h"
#include "config.h"
#include "display_manager.h"
#include "hardware_probe.h"
#include "led_manager.h"
#include "logger.h"
#include "dht_manager.h"

namespace {
DhtManager g_dht(Config::Pins::DHT11, Config::Dht::TYPE);
DisplayManager g_display(Config::Display::WIDTH, Config::Display::HEIGHT, &Wire,
                         Config::Display::RESET_PIN);
LedManager g_leds(Config::Pins::WS2812B);

unsigned long g_last_read_ms = 0;
unsigned long g_last_upload_ms = 0;
unsigned long g_last_history_ms = 0;
bool g_display_ready = false;

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void InitWiFi() {
  WiFi.begin(Config::Network::SSID, Config::Network::PASSWORD);
  Logger::Info("Connecting to WiFi...");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Logger::Info("\nWiFi connected. IP: " + WiFi.localIP().toString());
  } else {
    Logger::Warning("\nWiFi connection failed.");
  }
}

void InitFirebase() {
  config.api_key = Config::Firebase::API_KEY;
  config.database_url = Config::Firebase::HOST;
  
  if (Firebase.ready()) return;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  Logger::Info("Firebase initialized");
}

void UploadToFirebase(const DhtReading& reading, bool isHistory) {
  if (!Firebase.ready()) return;

  String basePath = "/devices/" + String(Config::Firebase::DEVICE_ID);
  FirebaseJson json;
  json.add("t", reading.temperature_c);
  json.add("h", reading.humidity);
  // Use Firebase Server Value for timestamp
  FirebaseJson timestamp;
  timestamp.set(".sv", "timestamp");
  json.set("ts", timestamp);

  if (isHistory) {
    String historyPath = basePath + "/history";
    if (Firebase.RTDB.pushJSON(&fbdo, historyPath.c_str(), &json)) {
      Logger::Info("History pushed to Firebase");
    } else {
      Logger::Error("Firebase history push failed: " + fbdo.errorReason());
    }
  } else {
    String currentPath = basePath + "/current";
    if (Firebase.RTDB.setJSON(&fbdo, currentPath.c_str(), &json)) {
      Logger::Info("Current data set in Firebase");
    } else {
      Logger::Error("Firebase set failed: " + fbdo.errorReason());
    }
  }
}

HumidityStatus EvaluateHumidity(float humidity) {
  if (humidity > Config::Humidity::HIGH_THRESHOLD) {
    return HumidityStatus::STATUS_HIGH;
  }
  if (humidity < Config::Humidity::LOW_THRESHOLD) {
    return HumidityStatus::STATUS_LOW;
  }
  return HumidityStatus::STATUS_NORMAL;
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
  
  InitWiFi();
  InitFirebase();

  Logger::Info("Setup complete");
}

void loop() {
  const unsigned long now = millis();
  
  // 1. Read sensor
  if (now - g_last_read_ms >= Config::Dht::READ_INTERVAL_MS) {
    g_last_read_ms = now;

    DhtReading reading{};
    if (g_dht.Read(reading)) {
      HumidityStatus status = EvaluateHumidity(reading.humidity);
      g_leds.SetStatus(status);

      if (g_display_ready) {
        g_display.ShowReading(reading, status, (WiFi.status() == WL_CONNECTED), Firebase.ready());
      }

      Logger::Info(String("T=") + String(reading.temperature_c, 1) + "C, H=" +
                   String(reading.humidity, 1) + "%");

      // 2. Upload to Firebase (Current)
      if (now - g_last_upload_ms >= Config::Firebase::UPLOAD_INTERVAL_MS) {
        g_last_upload_ms = now;
        UploadToFirebase(reading, false);
      }

      // 3. Upload to Firebase (History)
      if (now - g_last_history_ms >= Config::Firebase::HISTORY_INTERVAL_MS) {
        g_last_history_ms = now;
        UploadToFirebase(reading, true);
      }
    } else {
      Logger::Error("DHT read failed");
      g_leds.SetStatus(HumidityStatus::STATUS_ERROR);
      if (g_display_ready) {
        g_display.ShowError("DHT read failed");
      }
    }
  }

  delay(Config::Timing::LOOP_DELAY_MS);
}
