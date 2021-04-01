#ifndef MOCK_BLE_DEVICE_H_
#define MOCK_BLE_DEVICE_H_

#include <BLEClient.h>

#include <string>

typedef enum {
  ESP_PWR_LVL_N12 = 0,
  ESP_PWR_LVL_N9 = 1,
  ESP_PWR_LVL_N6 = 2,
  ESP_PWR_LVL_N3 = 3,
  ESP_PWR_LVL_N0 = 4,
  ESP_PWR_LVL_P3 = 5,
  ESP_PWR_LVL_P6 = 6,
  ESP_PWR_LVL_P9 = 7,
  ESP_PWR_LVL_N14 = ESP_PWR_LVL_N12,
  ESP_PWR_LVL_N11 = ESP_PWR_LVL_N9,
  ESP_PWR_LVL_N8 = ESP_PWR_LVL_N6,
  ESP_PWR_LVL_N5 = ESP_PWR_LVL_N3,
  ESP_PWR_LVL_N2 = ESP_PWR_LVL_N0,
  ESP_PWR_LVL_P1 = ESP_PWR_LVL_P3,
  ESP_PWR_LVL_P4 = ESP_PWR_LVL_P6,
  ESP_PWR_LVL_P7 = ESP_PWR_LVL_P9,
} esp_power_level_t;

class BLEDevice {
 public:
  static BLEClient* createClient();
  static void init(std::string deviceName);
  static void setPower(esp_power_level_t powerLevel);
  static void deinit(bool release_memory = false);
};

#endif  // MOCK_BLE_DEVICE_H_
