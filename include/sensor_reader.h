#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <BLEDevice.h>

#include "plant.h"

class SensorReader {
private:
  BLEClient *mBLEClient;

public:
  SensorReader();
  PlantMetrics query(Plant plant);
};

#endif
