#include <WiFi.h>
#include <BLEDevice.h>
#include <PubSubClient.h>

#include "config.h"
#include "octopus.h"

Octopus::Octopus() {}

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

void Octopus::deinitBluetooth() {
  BLEDevice::deinit();
}

PubSubClient Octopus::initMQTT(const char *clientId, const char *host, const int port, const char *username, const char *password) {
  WiFiClient espClient;
  PubSubClient client(espClient);
  m_mqtt_client = client;

  Serial.println("Connecting to MQTT...");
  client.setServer(host, port);

  while (!client.connected()) {
    if (!client.connect(clientId, username, password)) {
      Serial.print("MQTT connection failed:");
      Serial.print(client.state());
      Serial.println("Retrying...");
      delay(_MQTT_RETRY_WAIT);
    }
  }

  Serial.println("MQTT connected");
  Serial.println("");
}

PubSubClient Octopus::getMQTTClient() {
  return m_mqtt_client;
}

void Octopus::deinitMQTT() {
  m_mqtt_client.disconnect();
  Serial.println("MQTT disconnected");
}
