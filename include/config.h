/**
 * Defines configuration constants that are used inside the application.
 */
#ifndef PLANT_CONFIGURATION_H
#define PLANT_CONFIGURATION_H

struct PlantConfig {
  // List of sensor(s) queried by the system
  const char *sensorsMacAddr[0] = {};
  // Duration (seconds) between runs (deep sleep mode)
  const int deepSleepDuration = 30 * 60;
  // Max duration (seconds) before forcing a deep sleep
  const int emergencySleepDuration = 3 * 60;
  // Number of retries in reading sensor data
  const int sensorReadingRetries = 3;
  // WiFi Credentials
  const char *wifiSSID = "";
  const char *wifiPassword = "";
  // MQTT broker configuration
  const int mqttPort = 1883;
  const char *mqttHost = "";
  const char *mqttUsername = "";
  const char *mqttPassword = "";
  const char *mqttClientID = "";
  // MQTT topic are defined as "<MQTT_BASE_TOPIC>/<MAC_ADDRESS>/<property>"
  // where MAC_ADDRESS is one of the values from FLORA_DEVICES array
  // property is either temperature, moisture, conductivity, light or battery
  const char *mqttBaseTopic = "";
  // Duration (milliseconds) before retry in sending MQTT messages
  const int mqttRetryWait = 5 * 1000;
};

#endif
