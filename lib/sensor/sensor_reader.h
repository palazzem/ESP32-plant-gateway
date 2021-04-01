#ifndef SENSOR_SENSOR_READER_H_
#define SENSOR_SENSOR_READER_H_

#include <Arduino.h>
#include <BLEDevice.h>

#include "config.h"
#include "plant.h"

class SensorReader {
private:
  BLEClient *ble_client_;
  BLERemoteService *ble_service_;

  bool setService(const BLEUUID uuid);
  bool enableDataMode(const BLEUUID uuid);
  const char *readRawData(const BLEUUID uuid);
  float parseTemperature(const char *raw_data);
  int parseMoisture(const char *raw_data);
  int parseLight(const char *raw_data);
  int parseConductivity(const char *raw_data);
  int parseBattery(const char *raw_data);

public:
  SensorReader(AppConfig config);
  ~SensorReader();
  bool query(Plant plant, PlantMetrics &metrics);
};

#endif  // SENSOR_SENSOR_READER_H_
