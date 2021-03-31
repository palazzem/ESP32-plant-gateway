/**
 * Defines configuration constants that are used inside the application.
 */
#ifndef PLANT_CONFIG_H
#define PLANT_CONFIG_H

#include <WString.h>

// List compatible MiFlora sensors MAC address(es)
extern const char *FLORA_DEVICES[];
extern const int DEVICE_COUNT;

// Sleep between runs (seconds)
extern const int SLEEP_DURATION;

// Emergency hibernate countdown triggered when the device is stuck (seconds)
extern const int EMERGENCY_HIBERNATE;

// Number of runs after which sensors battery info is collected
extern const int BATTERY_INTERVAL;

// Number of retries in reading a sensor data before bailing out
extern const int RETRY;

// WiFi (AP) credentials
extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;

// MQTT configuration to connect to a MQTT server. Authentication is required.
extern const int MQTT_PORT;
extern const char *MQTT_HOST;
extern const char *MQTT_USERNAME;
extern const char *MQTT_PASSWORD;

// MQTT Client ID used by the board to register in the MQTT server.
// Example: `miflora-client-1`.
extern const char *MQTT_CLIENTID;

// MQTT topic are defined as "<MQTT_BASE_TOPIC>/<MAC_ADDRESS>/<property>"
// where MAC_ADDRESS is one of the values from FLORA_DEVICES array
// property is either temperature, moisture, conductivity, light or battery
extern const String MQTT_BASE_TOPIC;

// List of internal configurations
extern const int _MQTT_RETRY_WAIT;

#endif
