#include <BLEDevice.h>

#include "config.h"
#include "plant.h"
#include "sensor_reader.h"

// Service identifier the system connects to
const BLEUUID serviceUUID("00001204-0000-1000-8000-00805f9b34fb");

// Characteristics of the service
const BLEUUID uuid_version_battery("00001a02-0000-1000-8000-00805f9b34fb");
const BLEUUID uuid_sensor_data("00001a01-0000-1000-8000-00805f9b34fb");
const BLEUUID uuid_write_mode("00001a00-0000-1000-8000-00805f9b34fb");

// Bytes needed to put the sensor in data mode
uint8_t payloadDataMode[2] = {0xA0, 0x1F};

SensorReader::SensorReader() { this->mBLEClient = BLEDevice::createClient(); }

PlantMetrics SensorReader::query(Plant plant) {
  PlantMetrics metrics{};

  // Connect over Bluetooth
  BLEAddress macAddr = BLEAddress(plant.macAddr);
  this->mBLEClient->connect(macAddr);

  // Retrieve service data
  BLERemoteService *bleService;
  try {
    bleService = this->mBLEClient->getService(serviceUUID);
    if (bleService == nullptr) {
      // Failed to find data
      this->mBLEClient->disconnect();
      return metrics;
    }
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return metrics;
  }

  // Set service in data mode (required to collect data)
  BLERemoteCharacteristic *characteristic;
  try {
    characteristic = bleService->getCharacteristic(uuid_write_mode);
    if (characteristic == nullptr) {
      // Failed to retrieve sensor characteristics
      this->mBLEClient->disconnect();
      return metrics;
    }
    characteristic->writeValue(payloadDataMode, 2, true);
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return metrics;
  }

  // Retrieve sensors data
  try {
    characteristic = bleService->getCharacteristic(uuid_sensor_data);
    if (characteristic == nullptr) {
      // Failed to retrieve sensor characteristics
      this->mBLEClient->disconnect();
      return metrics;
    }
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return metrics;
  }

  // Read values
  std::string value;
  try {
    value = characteristic->readValue();
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return metrics;
  }
  const char *val = value.c_str();

  // Parse temperature
  int16_t *temp_raw = (int16_t *)val;
  metrics.temperature = (*temp_raw) / ((float)10.0);

  // Parse moisture
  metrics.moisture = val[7];

  // Parse light
  metrics.light = val[3] + val[4] * 256;

  // Parse conductivity
  metrics.conductivity = val[8] + val[9] * 256;

  // Retrieve battery data
  try {
    characteristic = bleService->getCharacteristic(uuid_version_battery);
    if (characteristic == nullptr) {
      // Failed to retrieve sensor characteristics
      this->mBLEClient->disconnect();
      return metrics;
    }
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return metrics;
  }

  // Read battery levels
  std::string value;
  try {
    value = characteristic->readValue();
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return metrics;
  }
  const char *val2 = value.c_str();
  metrics.battery = val2[0];

  // Disconnect bluetooth client
  // TODO: check client deconstructor and see if it implements RAII
  this->mBLEClient->disconnect();

  return metrics;
}
