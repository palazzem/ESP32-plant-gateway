#ifndef PLANT_H
#define PLANT_H

struct Plant {
  const char *macAddr;
  float temperature;
  int moisture;
  int light;
  int conductivity;
  int battery;
};

#endif
