#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <BLEDevice.h>

#include "config.h"
#include "plant.h"

class SensorReader {
private:
  BLEClient *mBLEClient;
  BLERemoteService *mService;
  bool setService(const BLEUUID uuid);
  bool enableDataMode(const BLEUUID uuid);
  const char *readRawData(const BLEUUID uuid);
  float parseTemperature(const char *rawData);
  int parseMoisture(const char *rawData);
  int parseLight(const char *rawData);
  int parseConductivity(const char *rawData);
  int parseBattery(const char *rawData);

public:
  SensorReader(AppConfig config);
  ~SensorReader();
  bool query(Plant plant, PlantMetrics &metrics);
};

#endif
