#ifndef MODELS_PLANT_H_
#define MODELS_PLANT_H_

struct PlantMetrics {
  float temperature;
  int moisture;
  int light;
  int conductivity;
  int battery;
};

struct Plant {
  const char *mac_addr;
  PlantMetrics metrics;
};

#endif  // MODELS_PLANT_H_
