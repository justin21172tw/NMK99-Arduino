#ifndef HARDWARE_PROBE_H
#define HARDWARE_PROBE_H

#include <Arduino.h>
#include <Wire.h>

#include <vector>

#include "dht_manager.h"

namespace HardwareProbe {
std::vector<uint8_t> ScanI2CDevices(TwoWire& wire);
int FindDisplayAddress(const std::vector<uint8_t>& devices, uint8_t preferred_address);
bool ProbeDht(DhtManager& dht_manager);
}  // namespace HardwareProbe

#endif
