/**
 * Defines configuration constants that are used inside the application.
 */
#ifndef MODELS_CONFIG_H_
#define MODELS_CONFIG_H_

struct AppConfig {
  // List of sensor(s) queried by the system
  const char *sensors_mac_addr[0] = {};
  // Duration (seconds) between runs (deep sleep mode)
  const int deep_sleep_duration = 30 * 60;
  // Max duration (seconds) before forcing a deep sleep
  const int emergency_sleep_duration = 3 * 60;
  // Number of retries in reading sensor data
  const int sensor_reading_retries = 3;
  // WiFi Credentials
  const char *wifi_ssid = "";
  const char *wifi_password = "";
  // MQTT broker configuration
  const int mqtt_port = 1883;
  const char *mqtt_host = "";
  const char *mqtt_username = "";
  const char *mqtt_password = "";
  const char *mqtt_client_id = "";
  // MQTT topic are defined as "<MQTT_BASE_TOPIC>/<MAC_ADDRESS>/<property>"
  // where MAC_ADDRESS is one of the values from FLORA_DEVICES array
  // property is either temperature, moisture, conductivity, light or battery
  const char *mqtt_base_topic = "";
  // Duration (milliseconds) before retry in sending MQTT messages
  const int mqtt_retry_wait = 5 * 1000;
};

#endif  // MODELS_CONFIG_H_
