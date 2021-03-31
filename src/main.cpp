/**
 * Application that retrieves data from compatible MiFlora devices and publishes
 * collected data over the wire via MQTT.
 */
#include <BLEDevice.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "octopus.h"
#include "plant.h"
#include "sensor_reader.h"

TaskHandle_t hibernateTaskHandle = NULL;

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

  // create a hibernate task in case something gets stuck
  xTaskCreate(delayedHibernate, "hibernate", 4096, NULL, 1,
              &hibernateTaskHandle);

  // Initializing these components, brings up all the network stack
  Octopus octo = Octopus();
  SensorReader sensorReader = SensorReader();

  // Octopus activates the entire stack
  octo.initWifi(WIFI_SSID, WIFI_PASSWORD);
  octo.initMQTT(MQTT_CLIENTID, MQTT_HOST, MQTT_PORT, MQTT_USERNAME,
                MQTT_PASSWORD);

  // process devices
  for (int i = 0; i < DEVICE_COUNT; i++) {
    int retry = 0;
    PlantMetrics metrics;
    Plant plant = {FLORA_DEVICES[i], {}};

    while (retry < RETRY) {
      ++retry;
      bool result = sensorReader.query(plant, plant.metrics);
      if (result) {
        Serial.println("Sensor read with success!");
        break;
      }
    }

    // TODO: needs to send data via Octopus (aka: Dispatcher)
    plant.metrics = metrics;
  }

  // disconnect wifi and mqtt
  octo.deinitMQTT();
  octo.deinitWiFi();

  // delete emergency hibernate task
  vTaskDelete(hibernateTaskHandle);
}

// Loop is not used. When this code path is reached, the system goes in deep
// sleep mode.
void loop() { hibernate(); }
