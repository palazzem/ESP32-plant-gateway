/**
 * Defines configuration constants that are used inside the application.
 */
#ifndef PLANT_CONFIG_H
#define PLANT_CONFIG_H

// List compatible MiFlora sensors MAC address(es)
const char *FLORA_DEVICES[] = {};

// Sleep between runs (seconds)
const int SLEEP_DURATION = 30 * 60;

// Emergency hibernate countdown triggered when the device is stuck (seconds)
const int EMERGENCY_HIBERNATE = 3 * 60;

// Number of runs after which sensors battery info is collected
const int BATTERY_INTERVAL = 6;

// Number of retries in reading a sensor data before bailing out
const int RETRY = 3;

// WiFi (AP) credentials
const char *WIFI_SSID = "";
const char *WIFI_PASSWORD = "";

// Adafruit HUZZAH32 ESP32 Feather Battery is connected to ADC pin A13 (or 35)
// TODO: remove because it can't realistically be collect in % manner
const int ADC_Pin = A13;

// MQTT configuration to connect to a MQTT server. Authentication is required.
const int MQTT_PORT = 1883;
const char *MQTT_HOST = "";
const char *MQTT_USERNAME = "";
const char *MQTT_PASSWORD = "";

// MQTT Client ID used by the board to register in the MQTT server.
// Example: `miflora-client-1`.
const char *MQTT_CLIENTID = "";

// MQTT topic are defined as "<MQTT_BASE_TOPIC>/<MAC_ADDRESS>/<property>"
// where MAC_ADDRESS is one of the values from FLORA_DEVICES array
// property is either temperature, moisture, conductivity, light or battery
const String MQTT_BASE_TOPIC = "";

// Retry before trying to publish data again over MQTT (in milliseconds)
const int MQTT_RETRY_WAIT = 5000;

#endif
