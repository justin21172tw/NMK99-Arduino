#include "hardware_probe.h"

#include <Wire.h>

#include "logger.h"

namespace HardwareProbe {
std::vector<uint8_t> ScanI2CDevices(TwoWire& wire) {
  Logger::Info("Scanning I2C...");
  std::vector<uint8_t> devices;

  for (uint8_t address = 0x08; address <= 0x77; ++address) {
    wire.beginTransmission(address);
    if (wire.endTransmission() == 0) {
      devices.push_back(address);
      Logger::Info(String("Found device at 0x") + String(address, HEX));
    }
  }

  if (devices.empty()) {
    Logger::Warning("No I2C device found");
  }
  return devices;
}

int FindDisplayAddress(const std::vector<uint8_t>& devices, uint8_t preferred_address) {
  for (const auto& address : devices) {
    if (address == preferred_address) {
      return preferred_address;
    }
  }

  if (!devices.empty()) {
    return devices.front();
  }

  return -1;
}

bool ProbeDht(DhtManager& dht_manager) {
  DhtReading reading{};
  if (!dht_manager.Read(reading)) {
    Logger::Warning("DHT sensor timeout or read failed");
    return false;
  }

  Logger::Info(String("DHT probe OK (T=") + String(reading.temperature_c, 1) + "C, H=" +
               String(reading.humidity, 1) + "%)");
  return true;
}
}  // namespace HardwareProbe
