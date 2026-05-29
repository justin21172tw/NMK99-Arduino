#ifndef DHT_MANAGER_H
#define DHT_MANAGER_H

#include <Arduino.h>
#include <DHT.h>

struct DhtReading {
  float temperature_c;
  float humidity;
};

class DhtManager {
 public:
  DhtManager(uint8_t pin, uint8_t type);
  void Begin();
  bool Read(DhtReading& out_reading);

 private:
  DHT dht_;
};

#endif
