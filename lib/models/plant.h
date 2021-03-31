#ifndef PLANT_H
#define PLANT_H

struct PlantMetrics {
  float temperature;
  int moisture;
  int light;
  int conductivity;
  int battery;
};

struct Plant {
  const char *macAddr;
  PlantMetrics metrics;
};

#endif
