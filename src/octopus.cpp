#include <octopus.h>

// Both clients must stay in the outer scope, otherwise the destructor
// is called, causing a NULL pointer access crash.
// TODO: maybe Octopus must not declare this and inject from the outside for
// simplified testing.
WiFiClient wifiClient;
PubSubClient mqttClient;

Octopus::Octopus(PlantConfig config) {
  // Initialize WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(config.wifiSSID, config.wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("");
  mqttClient.setClient(wifiClient);

  // Initialize MQTT client
  Serial.println("Connecting to MQTT...");
  this->m_mqtt_client.setServer(config.mqttHost, config.mqttPort);

  while (!this->m_mqtt_client.connected()) {
    if (!this->m_mqtt_client.connect(config.mqttClientID, config.mqttUsername,
                                     config.mqttPassword)) {
      Serial.print("MQTT connection failed:");
      Serial.print(this->m_mqtt_client.state());
      Serial.println("Retrying...");
      delay(config.mqttRetryWait);
    }
  }
  Serial.println("MQTT connected!");
  Serial.println("");
  this->m_mqtt_client = mqttClient;
}

Octopus::~Octopus() {
  // MQTT disconnection
  this->m_mqtt_client.disconnect();
  Serial.println("MQTT disconnected");

  // WiFi disconnection
  WiFi.disconnect(true);
  Serial.println("WiFi disonnected");
}

bool Octopus::publish(const char *topic, const char *payload) {
  return this->m_mqtt_client.publish(topic, payload);
}
