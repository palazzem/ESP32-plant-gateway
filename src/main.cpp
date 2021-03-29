/**
   A BLE client for the Xiaomi Mi Plant Sensor, pushing measurements to an MQTT
   server.
*/

#include <BLEDevice.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "octopus.h"

// boot count used to check if battery status should be read
RTC_DATA_ATTR int bootCount = 0;

// device count
static int deviceCount = sizeof FLORA_DEVICES / sizeof FLORA_DEVICES[0];

// the remote service we wish to connect to
static BLEUUID serviceUUID("00001204-0000-1000-8000-00805f9b34fb");

// the characteristic of the remote service we are interested in
static BLEUUID uuid_version_battery("00001a02-0000-1000-8000-00805f9b34fb");
static BLEUUID uuid_sensor_data("00001a01-0000-1000-8000-00805f9b34fb");
static BLEUUID uuid_write_mode("00001a00-0000-1000-8000-00805f9b34fb");

// ESP32 MAC address
char macAddr[18];

TaskHandle_t hibernateTaskHandle = NULL;

Octopus octo = Octopus();

BLEClient *getFloraClient(BLEAddress floraAddress) {
  BLEClient *floraClient = BLEDevice::createClient();

  if (!floraClient->connect(floraAddress)) {
    Serial.println("- Connection failed, skipping");
    return nullptr;
  }

  Serial.println("- Connection successful");
  return floraClient;
}

BLERemoteService *getFloraService(BLEClient *floraClient) {
  BLERemoteService *floraService = nullptr;

  try {
    floraService = floraClient->getService(serviceUUID);
  } catch (...) {
    // something went wrong
  }
  if (floraService == nullptr) {
    Serial.println("- Failed to find data service");
    Serial.println(serviceUUID.toString().c_str());
  } else {
    Serial.println("- Found data service");
  }

  return floraService;
}

bool forceFloraServiceDataMode(BLERemoteService *floraService) {
  BLERemoteCharacteristic *floraCharacteristic;

  // get device mode characteristic, needs to be changed to read data
  Serial.println("- Force device in data mode");
  floraCharacteristic = nullptr;
  try {
    floraCharacteristic = floraService->getCharacteristic(uuid_write_mode);
  } catch (...) {
    // something went wrong
  }
  if (floraCharacteristic == nullptr) {
    Serial.println("-- Failed, skipping device");
    return false;
  }

  // write the magic data
  uint8_t buf[2] = {0xA0, 0x1F};
  floraCharacteristic->writeValue(buf, 2, true);

  delay(500);
  return true;
}

bool readFloraDataCharacteristic(BLERemoteService *floraService,
                                 String baseTopic) {
  BLERemoteCharacteristic *floraCharacteristic = nullptr;

  // get the main device data characteristic
  Serial.println("- Access characteristic from device");
  try {
    floraCharacteristic = floraService->getCharacteristic(uuid_sensor_data);
  } catch (...) {
    // something went wrong
  }
  if (floraCharacteristic == nullptr) {
    Serial.println("-- Failed, skipping device");
    return false;
  }

  // read characteristic value
  Serial.println("- Read value from characteristic");
  std::string value;
  try {
    value = floraCharacteristic->readValue();
  } catch (...) {
    // something went wrong
    Serial.println("-- Failed, skipping device");
    return false;
  }
  const char *val = value.c_str();

  Serial.print("Hex: ");
  for (int i = 0; i < 16; i++) {
    Serial.print((int)val[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");

  char buffer[64];

  int16_t *temp_raw = (int16_t *)val;
  float temperature = (*temp_raw) / ((float)10.0);
  Serial.print("-- Temperature:  ");
  Serial.print(temperature);
  Serial.print("°C");
  if (temperature != 0 && temperature > -20 && temperature < 40) {
    snprintf(buffer, 64, "%2.1f", temperature);
    if (octo.getMQTTClient().publish((baseTopic + "temperature").c_str(), buffer)) {
      Serial.println("   >> Published");
    }
  } else {
    Serial.println("   >> Skip");
  }

  int moisture = val[7];
  Serial.print("-- Moisture:     ");
  Serial.print(moisture);
  Serial.print(" %");
  if (moisture <= 100 && moisture >= 0) {
    snprintf(buffer, 64, "%d", moisture);
    if (octo.getMQTTClient().publish((baseTopic + "moisture").c_str(), buffer)) {
      Serial.println("   >> Published");
    }
  } else {
    Serial.println("   >> Skip");
  }

  int light = val[3] + val[4] * 256;
  Serial.print("-- Light:        ");
  Serial.print(light);
  Serial.print(" lux");
  if (light >= 0) {
    snprintf(buffer, 64, "%d", light);
    if (octo.getMQTTClient().publish((baseTopic + "light").c_str(), buffer)) {
      Serial.println("   >> Published");
    }
  } else {
    Serial.println("   >> Skip");
  }

  int conductivity = val[8] + val[9] * 256;
  Serial.print("-- Conductivity: ");
  Serial.print(conductivity);
  Serial.print(" uS/cm");
  if (conductivity >= 0 && conductivity < 5000) {
    snprintf(buffer, 64, "%d", conductivity);
    if (octo.getMQTTClient().publish((baseTopic + "conductivity").c_str(), buffer)) {
      Serial.println("   >> Published");
    }
  } else {
    Serial.println("   >> Skip");
  }

  return true;
}

bool readFloraBatteryCharacteristic(BLERemoteService *floraService,
                                    String baseTopic) {
  BLERemoteCharacteristic *floraCharacteristic = nullptr;

  // get the device battery characteristic
  Serial.println("- Access battery characteristic from device");
  try {
    floraCharacteristic = floraService->getCharacteristic(uuid_version_battery);
  } catch (...) {
    // something went wrong
  }
  if (floraCharacteristic == nullptr) {
    Serial.println("-- Failed, skipping battery level");
    return false;
  }

  // read characteristic value
  Serial.println("- Read value from characteristic");
  std::string value;
  try {
    value = floraCharacteristic->readValue();
  } catch (...) {
    // something went wrong
    Serial.println("-- Failed, skipping battery level");
    return false;
  }
  const char *val2 = value.c_str();
  int battery = val2[0];

  char buffer[64];
  Serial.print("-- Battery:      ");
  Serial.print(battery);
  Serial.println(" %");
  snprintf(buffer, 64, "%d", battery);
  octo.getMQTTClient().publish((baseTopic + "battery").c_str(), buffer);
  Serial.println("   >> Published");

  return true;
}

bool processFloraService(BLERemoteService *floraService,
                         const char *deviceMacAddress, bool readBattery) {
  // set device in data mode
  if (!forceFloraServiceDataMode(floraService)) {
    return false;
  }

  String baseTopic = MQTT_BASE_TOPIC + "/" + deviceMacAddress + "/";
  bool dataSuccess = readFloraDataCharacteristic(floraService, baseTopic);

  bool batterySuccess = true;
  if (readBattery) {
    batterySuccess = readFloraBatteryCharacteristic(floraService, baseTopic);
  }

  return dataSuccess && batterySuccess;
}

bool processFloraDevice(BLEAddress floraAddress, const char *deviceMacAddress,
                        bool getBattery, int tryCount) {
  Serial.print("Processing Flora device at ");
  Serial.print(floraAddress.toString().c_str());
  Serial.print(" (try ");
  Serial.print(tryCount);
  Serial.println(")");

  // connect to flora ble server
  BLEClient *floraClient = getFloraClient(floraAddress);
  if (floraClient == nullptr) {
    return false;
  }

  // connect data service
  BLERemoteService *floraService = getFloraService(floraClient);
  if (floraService == nullptr) {
    floraClient->disconnect();
    return false;
  }

  // process devices data
  bool success =
      processFloraService(floraService, deviceMacAddress, getBattery);

  // disconnect from device
  floraClient->disconnect();

  return success;
}

void hibernate() {
  Serial.println("Going to sleep now.");
  delay(100);
  esp_deep_sleep(SLEEP_DURATION * 1000000ll);
}

void delayedHibernate(void *parameter) {
  delay(EMERGENCY_HIBERNATE * 1000); // delay for five minutes
  Serial.println("Something got stuck, entering emergency hibernate...");
  hibernate();
}

void setup() {
  // all action is done when device is woken up
  Serial.begin(115200);
  delay(1000);

  // increase boot count
  bootCount++;

  // create a hibernate task in case something gets stuck
  xTaskCreate(delayedHibernate, "hibernate", 4096, NULL, 1,
              &hibernateTaskHandle);

  // Octopus activates the entire stack
  octo.initBluetooth(_DEVICE_NAME);
  octo.initWifi(WIFI_SSID, WIFI_PASSWORD);
  octo.initMQTT(MQTT_CLIENTID, MQTT_HOST, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

  Serial.println("");
  // check if battery status should be read - based on boot count
  bool readBattery = ((bootCount % BATTERY_INTERVAL) == 0);

  // process devices
  for (int i = 0; i < deviceCount; i++) {
    int tryCount = 0;
    BLEAddress floraAddress(FLORA_DEVICES[i]);

    while (tryCount < RETRY) {
      tryCount++;
      if (processFloraDevice(floraAddress, FLORA_DEVICES[i], readBattery,
                             tryCount)) {
        break;
      }
      delay(1000);
    }
    delay(1500);
  }

  // disconnect wifi and mqtt
  octo.deinitWiFi();
  octo.deinitBluetooth();
  octo.deinitMQTT();

  // delete emergency hibernate task
  vTaskDelete(hibernateTaskHandle);

  // go to sleep now
  hibernate();
}

void loop() {
  /// we're not doing anything in the loop, only on device wakeup
  delay(10000);
}
