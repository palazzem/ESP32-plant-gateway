#include "sensor_reader.h"

// Module configuration
const char *kDeviceName = "esp32-controller";

// Service identifier the system connects to
const BLEUUID kUUIDService("00001204-0000-1000-8000-00805f9b34fb");

// Characteristics of the service
const BLEUUID kUUIDVersionBattery("00001a02-0000-1000-8000-00805f9b34fb");
const BLEUUID kUUIDSensorData("00001a01-0000-1000-8000-00805f9b34fb");
const BLEUUID kUUIDWriteMode("00001a00-0000-1000-8000-00805f9b34fb");

// Bytes needed to put the sensor in data mode
uint8_t payload_data_mode[2] = {0xA0, 0x1F};

SensorReader::SensorReader(AppConfig config) {
  BLEDevice::init(kDeviceName);
  BLEDevice::setPower(ESP_PWR_LVL_P7);
  this->ble_client_ = BLEDevice::createClient();
}

SensorReader::~SensorReader() {
  this->ble_client_->disconnect();
  BLEDevice::deinit();
}

bool SensorReader::setService(const BLEUUID uuid) {
  try {
    this->ble_service_ = this->ble_client_->getService(kUUIDService);
    if (this->ble_service_ == nullptr) {
      // Failed to find the service, bailing out the entire operation
      this->ble_client_->disconnect();
      return false;
    }
  } catch (...) {
    // TODO: handle exceptions and log
    this->ble_client_->disconnect();
    return false;
  }

  return true;
}

bool SensorReader::enableDataMode(const BLEUUID uuid) {
  // Safe-guard: bailout if connection or service are not available
  if (!this->ble_client_->isConnected() || this->ble_service_ == nullptr) {
    return false;
  }

  // Set service in data mode (required to collect data)
  BLERemoteCharacteristic *sensor;
  try {
    sensor = this->ble_service_->getCharacteristic(uuid);
    sensor->writeValue(payload_data_mode, 2, true);
  } catch (...) {
    // TODO: handle exceptions and log
    return false;
  }

  return true;
}

const char *SensorReader::readRawData(const BLEUUID uuid) {
  // Safe-guard: bailout if connection or service are not available
  if (!this->ble_client_->isConnected() || this->ble_service_ == nullptr) {
    return nullptr;
  }

  // Retrieve sensors data
  std::string sensor_data;
  BLERemoteCharacteristic *sensor;
  try {
    sensor = this->ble_service_->getCharacteristic(uuid);
    sensor_data = sensor->readValue();
    return sensor_data.c_str();
  } catch (...) {
    // TODO: handle exceptions and log
    return nullptr;
  }
}

float SensorReader::parseTemperature(const char *raw_data) {
  int16_t *temp_raw = (int16_t *)raw_data;
  return (*temp_raw) / ((float)10.0);
}

int SensorReader::parseMoisture(const char *raw_data) { return raw_data[7]; }

int SensorReader::parseLight(const char *raw_data) {
  return raw_data[3] + raw_data[4] * 256;
}

int SensorReader::parseConductivity(const char *raw_data) {
  return raw_data[8] + raw_data[9] * 256;
}

int SensorReader::parseBattery(const char *raw_data) { return raw_data[0]; }

bool SensorReader::query(Plant plant, PlantMetrics &metrics) {
  // Connect using Bluetooth LE
  BLEAddress mac_addr = BLEAddress(plant.mac_addr);
  if (!this->ble_client_->connect(mac_addr)) {
    return false;
  }

  // Set sensor service
  if (!this->setService(kUUIDService)) {
    return false;
  }

  // Put the sensor in data mode (write bytes over bluetooth)
  if (!this->enableDataMode(kUUIDWriteMode)) {
    return false;
  }

  // Retrieve sensors data
  const char *raw_sensor_data = this->readRawData(kUUIDSensorData);
  if (raw_sensor_data == nullptr) {
    return false;
  }

  metrics.temperature = this->parseTemperature(raw_sensor_data);
  metrics.moisture = this->parseMoisture(raw_sensor_data);
  metrics.light = this->parseLight(raw_sensor_data);
  metrics.conductivity = this->parseConductivity(raw_sensor_data);

  // Retrieve battery data: it requires a different characteristic
  raw_sensor_data = this->readRawData(kUUIDVersionBattery);
  if (raw_sensor_data == nullptr) {
    return false;
  }

  metrics.battery = this->parseBattery(raw_sensor_data);

  // Disconnect bluetooth client
  // TODO: check client deconstructor and see if it implements RAII
  this->ble_client_->disconnect();

  return true;
}
