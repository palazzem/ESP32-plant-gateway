#include <Arduino.h>
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

bool SensorReader::setService(const BLEUUID uuid) {
  try {
    this->mService = this->mBLEClient->getService(serviceUUID);
    Serial.println("Service detected");
    if (this->mService == nullptr) {
      // Failed to find the service, bailing out the entire operation
      Serial.println("Failure (nullptr)");
      this->mBLEClient->disconnect();
      return false;
    }
  } catch (...) {
    // TODO: handle exceptions and log
    Serial.println("Failure (exception)");
    this->mBLEClient->disconnect();
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
  Serial.println("Start reading sensors...");
  // Connect using Bluetooth LE
  BLEAddress macAddr = BLEAddress(plant.macAddr);
  if (!this->mBLEClient->connect(macAddr)) {
    Serial.println("Connection via bluetooth failed");
    return false;
  }
  Serial.println("Connected to device");

  // Retrieve sensors characteristics
  if (!this->setService(serviceUUID)) {
    Serial.println("Service retrieval failed - bailing out");
    return false;
  }
  Serial.println("Service retrieved...");

  // Retrieve sensors data
  const char *rawSensorData = this->readRawData(uuid_sensor_data);
  if (rawSensorData == nullptr) {
    Serial.println("Reading sensor data failed - bailing out");
    return false;
  }

  metrics.temperature = this->parseTemperature(rawSensorData);
  metrics.moisture = this->parseMoisture(rawSensorData);
  metrics.light = this->parseLight(rawSensorData);
  metrics.conductivity = this->parseConductivity(rawSensorData);
  Serial.println("Readings:");
  Serial.print("- Temperature: ");
  Serial.println(metrics.temperature);
  Serial.print("- Moisture: ");
  Serial.println(metrics.moisture);
  Serial.print("- Light: ");
  Serial.println(metrics.light);
  Serial.print("- Conductivity: ");
  Serial.println(metrics.conductivity);

  // Retrieve battery data: it requires a different characteristic
  rawSensorData = this->readRawData(uuid_version_battery);
  if (rawSensorData == nullptr) {
    Serial.println("Reading sensor battery failed - bailing out");
    return false;
  }

  metrics.battery = this->parseBattery(rawSensorData);
  Serial.print("- Battery: ");
  Serial.println(metrics.battery);

  // Disconnect bluetooth client
  // TODO: check client deconstructor and see if it implements RAII
  this->mBLEClient->disconnect();
  Serial.println("Bluetooth disconnected");

  return true;
}
