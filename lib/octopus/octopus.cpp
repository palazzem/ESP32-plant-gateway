#include <octopus.h>

// Both clients must stay in the outer scope, otherwise the destructor
// is called, causing a NULL pointer access crash.
// TODO: maybe Octopus must not declare this and inject from the outside for
// simplified testing.
WiFiClient wifiClient;
PubSubClient mqttClient;

Octopus::Octopus(PlantConfig config) {
  // Initialize WiFi
  WiFi.begin(config.wifiSSID, config.wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  mqttClient.setClient(wifiClient);

  // Initialize MQTT client
  this->m_mqtt_client.setServer(config.mqttHost, config.mqttPort);

  while (!this->m_mqtt_client.connected()) {
    if (!this->m_mqtt_client.connect(config.mqttClientID, config.mqttUsername,
                                     config.mqttPassword)) {
      delay(config.mqttRetryWait);
    }
  }
  this->m_mqtt_client = mqttClient;
}

Octopus::~Octopus() {
  // MQTT disconnection
  this->m_mqtt_client.disconnect();

  // WiFi disconnection
  WiFi.disconnect(true);
}

bool Octopus::publish(const char *topic, const char *payload) {
  return this->m_mqtt_client.publish(topic, payload);
}
