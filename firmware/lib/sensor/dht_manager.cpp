#include "dht_manager.h"

#include <math.h>

DhtManager::DhtManager(uint8_t pin, uint8_t type) : dht_(pin, type) {}

void DhtManager::Begin() { dht_.begin(); }

bool DhtManager::Read(DhtReading& out_reading) {
  float humidity = dht_.readHumidity();
  float temperature = dht_.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    return false;
  }

  out_reading.humidity = humidity;
  out_reading.temperature_c = temperature;
  return true;
}
