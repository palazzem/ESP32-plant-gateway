/**
 * Application that retrieves data from compatible MiFlora devices and publishes
 * collected data over the wire via MQTT.
 */
#include <BLEDevice.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "plant.h"
#include "dispatcher.h"
#include "sensor_reader.h"

AppConfig config{};

TaskHandle_t hibernate_task_handle = NULL;

void hibernate() {
  Serial.println("Going to sleep now.");
  delay(100);
  esp_deep_sleep(config.deep_sleep_duration * 1000000ll);
}

void delayedHibernate(void *parameter) {
  delay(config.emergency_sleep_duration * 1000); // delay for five minutes
  Serial.println("Something got stuck, entering emergency hibernate...");
  hibernate();
}

void setup() {
  // all action is done when device is woken up
  Serial.begin(115200);
  delay(1000);

  // create a hibernate task in case something gets stuck
  xTaskCreate(delayedHibernate, "hibernate", 4096, NULL, 1,
              &hibernate_task_handle);

  // Initializing these components, brings up all the network stack
  Dispatcher dispatcher = Dispatcher(config);
  SensorReader sensorReader = SensorReader(config);

  // process devices
  for (int i = 0; i < sizeof(config.sensors_mac_addr); i++) {
    int retry = 0;
    PlantMetrics metrics;
    Plant plant = {config.sensors_mac_addr[i]};

    while (retry < config.sensor_reading_retries) {
      ++retry;
      bool result = sensorReader.query(plant, plant.metrics);
      if (result) {
        Serial.println("Sensor read with success!");
        break;
      }
    }

    // TODO: needs to send data via Dispatcher
    plant.metrics = metrics;
  }

  // delete emergency hibernate task
  vTaskDelete(hibernate_task_handle);
}

// Loop is not used. When this code path is reached, the system goes in deep
// sleep mode.
void loop() { hibernate(); }
