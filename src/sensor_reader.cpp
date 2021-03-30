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

void SensorReader::setService(const BLEUUID uuid) {
  try {
    this->mService = this->mBLEClient->getService(serviceUUID);
    if (this->mService == nullptr) {
      // Failed to find the service, bailing out the entire operation
      this->mBLEClient->disconnect();
    }
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
  }
}

const char *SensorReader::readRawData(const BLEUUID uuid) {
  // Safe-guard: bailout if connection or service are not available
  if (!this->mBLEClient->isConnected() || this->mService == nullptr) {
    return nullptr;
  }

  // Retrieve sensors data
  std::string sensorData;
  BLERemoteCharacteristic *sensor;
  try {
    sensor = this->mService->getCharacteristic(uuid);
    sensorData = sensor->readValue();
    return sensorData.c_str();
  } catch (...) {
    // TODO: handle exceptions and log
    return nullptr;
  }
}

float SensorReader::parseTemperature(const char *rawData) {
  int16_t *temp_raw = (int16_t *)rawData;
  return (*temp_raw) / ((float)10.0);
}

int SensorReader::parseMoisture(const char *rawData) { return rawData[7]; }

int SensorReader::parseLight(const char *rawData) {
  return rawData[3] + rawData[4] * 256;
}

int SensorReader::parseConductivity(const char *rawData) {
  return rawData[8] + rawData[9] * 256;
}

int SensorReader::parseBattery(const char *rawData) { return rawData[0]; }

PlantMetrics SensorReader::query(Plant plant) {
  // Connect using Bluetooth LE
  BLEAddress macAddr = BLEAddress(plant.macAddr);
  this->mBLEClient->connect(macAddr);

  // Retrieve sensors characteristics
  this->setService(serviceUUID);

  // Retrieve sensors data
  PlantMetrics metrics{};
  const char *rawSensorData = this->readRawData(uuid_sensor_data);
  metrics.temperature = this->parseTemperature(rawSensorData);
  metrics.moisture = this->parseMoisture(rawSensorData);
  metrics.light = this->parseLight(rawSensorData);
  metrics.conductivity = this->parseConductivity(rawSensorData);

  // Retrieve battery data: it requires a different characteristic
  rawSensorData = this->readRawData(uuid_version_battery);
  metrics.battery = this->parseBattery(rawSensorData);

  // Disconnect bluetooth client
  // TODO: check client deconstructor and see if it implements RAII
  this->mBLEClient->disconnect();

  return metrics;
}
