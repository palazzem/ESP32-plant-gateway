#include <dispatcher.h>

// Both clients must stay in the outer scope, otherwise the destructor
// is called, causing a NULL pointer access crash.
// TODO: maybe Octopus must not declare this and inject from the outside for
// simplified testing.
WiFiClient wifi_client;
PubSubClient mqtt_client;

Dispatcher::Dispatcher(AppConfig config) {
  // Initialize WiFi
  WiFi.begin(config.wifi_ssid, config.wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  mqtt_client_.setClient(wifi_client);

  // Initialize MQTT client
  this->mqtt_client_.setServer(config.mqtt_host, config.mqtt_port);

  while (!this->mqtt_client_.connected()) {
    if (!this->mqtt_client_.connect(config.mqtt_client_id, config.mqtt_username,
                                     config.mqtt_password)) {
      delay(config.mqtt_retry_wait);
    }
  }
  this->mqtt_client_ = mqtt_client;
}

Dispatcher::~Dispatcher() {
  // MQTT disconnection
  this->mqtt_client_.disconnect();

  // WiFi disconnection
  WiFi.disconnect(true);
}

bool Dispatcher::publish(const char *topic, const char *payload) {
  return this->mqtt_client_.publish(topic, payload);
}
