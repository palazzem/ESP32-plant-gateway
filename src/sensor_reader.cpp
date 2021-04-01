#include "sensor_reader.h"

#include "config.h"

// Module configuration
const char *DEVICE_NAME = "esp32-controller";

// Service identifier the system connects to
const BLEUUID serviceUUID("00001204-0000-1000-8000-00805f9b34fb");

// Characteristics of the service
const BLEUUID uuid_version_battery("00001a02-0000-1000-8000-00805f9b34fb");
const BLEUUID uuid_sensor_data("00001a01-0000-1000-8000-00805f9b34fb");
const BLEUUID uuid_write_mode("00001a00-0000-1000-8000-00805f9b34fb");

// Bytes needed to put the sensor in data mode
uint8_t payloadDataMode[2] = {0xA0, 0x1F};

SensorReader::SensorReader(AppConfig config) {
  BLEDevice::init(DEVICE_NAME);
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  this->mBLEClient = BLEDevice::createClient();
}

SensorReader::~SensorReader() {
  this->mBLEClient->disconnect();
  BLEDevice::deinit();
}

bool SensorReader::setService(const BLEUUID uuid) {
  try {
    this->mService = this->mBLEClient->getService(serviceUUID);
    if (this->mService == nullptr) {
      // Failed to find the service, bailing out the entire operation
      this->mBLEClient->disconnect();
      return false;
    }
  } catch (...) {
    // TODO: handle exceptions and log
    this->mBLEClient->disconnect();
    return false;
  }

  return true;
}

bool SensorReader::enableDataMode(const BLEUUID uuid) {
  // Safe-guard: bailout if connection or service are not available
  if (!this->mBLEClient->isConnected() || this->mService == nullptr) {
    return false;
  }

  // Set service in data mode (required to collect data)
  BLERemoteCharacteristic *sensor;
  try {
    sensor = this->mService->getCharacteristic(uuid);
    sensor->writeValue(payloadDataMode, 2, true);
  } catch (...) {
    // TODO: handle exceptions and log
    return false;
  }

  return true;
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

bool SensorReader::query(Plant plant, PlantMetrics &metrics) {
  // Connect using Bluetooth LE
  BLEAddress macAddr = BLEAddress(plant.macAddr);
  if (!this->mBLEClient->connect(macAddr)) {
    return false;
  }

  // Set sensor service
  if (!this->setService(serviceUUID)) {
    return false;
  }

  // Put the sensor in data mode (write bytes over bluetooth)
  if (!this->enableDataMode(uuid_write_mode)) {
    return false;
  }

  // Retrieve sensors data
  const char *rawSensorData = this->readRawData(uuid_sensor_data);
  if (rawSensorData == nullptr) {
    return false;
  }

  metrics.temperature = this->parseTemperature(rawSensorData);
  metrics.moisture = this->parseMoisture(rawSensorData);
  metrics.light = this->parseLight(rawSensorData);
  metrics.conductivity = this->parseConductivity(rawSensorData);

  // Retrieve battery data: it requires a different characteristic
  rawSensorData = this->readRawData(uuid_version_battery);
  if (rawSensorData == nullptr) {
    return false;
  }

  metrics.battery = this->parseBattery(rawSensorData);

  // Disconnect bluetooth client
  // TODO: check client deconstructor and see if it implements RAII
  this->mBLEClient->disconnect();

  return true;
}
