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

bool LooksConfigured(const char* value) {
  if (value == nullptr) {
    return false;
  }
  String text(value);
  text.trim();
  if (text.length() == 0) {
    return false;
  }
  if (text.startsWith("YOUR_")) {
    return false;
  }
  if (text.indexOf("...") >= 0) {
    return false;
  }
  return true;
}

void InitWiFi() {
  if (g_display_ready) g_display.ShowStatus("Connecting WiFi...");
  WiFi.begin(Config::Network::SSID, Config::Network::PASSWORD);
  Logger::Info("Connecting to WiFi...");
  
  // Wait shorter time in setup, continue in loop
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) { 
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Logger::Info("\nWiFi connected");
    if (g_display_ready) g_display.ShowStatus("WiFi OK");
  } else {
    Logger::Warning("\nWiFi failed (will retry)");
    if (g_display_ready) g_display.ShowStatus("WiFi Skip...");
  }
}

void InitFirebase() {
  if (WiFi.status() != WL_CONNECTED) {
    Logger::Warning("Skipping Firebase init: No WiFi");
    return;
  }

  if (g_display_ready) g_display.ShowStatus("Init Firebase...");
  Logger::Info("Initializing Firebase...");
  
  config.api_key = Config::Firebase::API_KEY;
  config.database_url = Config::Firebase::HOST;
  config.token_status_callback = tokenStatusCallback; 

  // Sign up as anonymous user
  Logger::Info("Signing up anonymously...");
  if (Firebase.signUp(&config, &auth, "", "")) {
    Logger::Info("Firebase anonymous signup success");
    if (g_display_ready) g_display.ShowStatus("Firebase OK");
  } else {
    Logger::Error("Firebase signup failed");
    if (g_display_ready) g_display.ShowStatus("Firebase Skip");
  }

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  
  Logger::Info("Firebase begin called.");
}

void UploadToFirebase(const DhtReading& reading, bool isHistory) {
  if (!Firebase.ready()) {
    static unsigned long last_not_ready_log = 0;
    if (millis() - last_not_ready_log > 15000) { // Check every 15s
      Logger::Warning("Firebase not ready. HTTP code: " + String(fbdo.httpCode()));
      last_not_ready_log = millis();
    }
    return;
  }

  String basePath = "/devices/" + String(Config::Firebase::DEVICE_ID);
  FirebaseJson json;
  json.add("t", reading.temperature_c);
  json.add("h", reading.humidity);
  
  // Correct syntax for Firebase Server Value timestamp
  json.set("ts/.sv", "timestamp");

  if (isHistory) {
    String historyPath = basePath + "/history";
    if (Firebase.RTDB.pushJSON(&fbdo, historyPath.c_str(), &json)) {
      Logger::Info("History pushed to Firebase");
    } else {
      Logger::Error("Firebase history push failed. HTTP code: " + String(fbdo.httpCode()));
    }
  } else {
    String currentPath = basePath + "/current";
    if (Firebase.RTDB.setJSON(&fbdo, currentPath.c_str(), &json)) {
      Logger::Info("Current data set in Firebase");
    } else {
      Logger::Error("Firebase set failed. HTTP code: " + String(fbdo.httpCode()));
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
  static bool first_run = true;
  
  // 1. Read sensor (Immediately on first run or after interval)
  if (first_run || (now - g_last_read_ms >= Config::Dht::READ_INTERVAL_MS)) {
    g_last_read_ms = now;
    first_run = false;

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
