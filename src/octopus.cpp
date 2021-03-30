#include <BLEDevice.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "octopus.h"

// Both clients must stay in the outer scope, otherwise the destructor
// is called, causing a NULL pointer access crash.
// TODO: maybe Octopus must not declare this and inject from the outside for
// simplified testing.
WiFiClient wifiClient;
PubSubClient mqttClient;

Octopus::Octopus() {
  mqttClient.setClient(wifiClient);
  this->m_mqtt_client = mqttClient;
}

void Octopus::initWifi(const char *ssid, const char *password) {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("");
}

void Octopus::deinitWiFi() {
  WiFi.disconnect(true);
  Serial.println("WiFi disonnected");
}

void Octopus::initBluetooth(const char *deviceName) {
  Serial.println("Initialize BLE client...");
  BLEDevice::init(deviceName);
  BLEDevice::setPower(ESP_PWR_LVL_P7);
}

void Octopus::deinitBluetooth() { BLEDevice::deinit(); }

void Octopus::initMQTT(const char *clientId, const char *host, const int port,
                       const char *username, const char *password) {
  Serial.println("Connecting to MQTT...");
  this->m_mqtt_client.setServer(host, port);

  while (!this->m_mqtt_client.connected()) {
    if (!this->m_mqtt_client.connect(clientId, username, password)) {
      Serial.print("MQTT connection failed:");
      Serial.print(this->m_mqtt_client.state());
      Serial.println("Retrying...");
      delay(_MQTT_RETRY_WAIT);
    }
  }

  Serial.println("MQTT connected");
  Serial.println("");
}

bool Octopus::publish(const char *topic, const char *payload) {
  return this->m_mqtt_client.publish(topic, payload);
}

void Octopus::deinitMQTT() {
  this->m_mqtt_client.disconnect();
  Serial.println("MQTT disconnected");
}
