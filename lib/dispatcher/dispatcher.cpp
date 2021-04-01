#include "dispatcher.h"

// Maximum size of MQTT buffer, used to send metrics over the wire
const int kMQTTBufferSize = 4;

// Both clients must stay in the outer scope, otherwise the destructor
// is called, causing a NULL pointer access crash.
// TODO: maybe Octopus must not declare this and inject from the outside for
// simplified testing.

Dispatcher::Dispatcher(AppConfig config) {
  // Config initialization
  this->base_topic_ = config.mqtt_base_topic;

  // Initialize WiFi
  WiFi.begin(config.wifi_ssid, config.wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  mqtt_client_.setClient(wifi_client_);

  // Initialize MQTT client
  this->mqtt_client_.setServer(config.mqtt_host, config.mqtt_port);

  while (!this->mqtt_client_.connected()) {
    if (!this->mqtt_client_.connect(config.mqtt_client_id, config.mqtt_username,
                                    config.mqtt_password)) {
      delay(config.mqtt_retry_wait);
    }
  }
}

Dispatcher::~Dispatcher() {
  // MQTT disconnection
  this->mqtt_client_.disconnect();

  // WiFi disconnection
  WiFi.disconnect(true);
}

void Dispatcher::sendMetric(const std::string prefix,
                            const std::string metric_name, const char *mac_addr,
                            const float value) {
  const char *topic = (prefix + metric_name).c_str();
  char buffer[kMQTTBufferSize];
  snprintf(buffer, sizeof(buffer), "%f", value);
  this->mqtt_client_.publish(topic, buffer);
}

void Dispatcher::sendMetric(const std::string prefix,
                            const std::string metric_name, const char *mac_addr,
                            const int value) {
  const char *topic = (prefix + metric_name).c_str();
  char buffer[kMQTTBufferSize];
  snprintf(buffer, sizeof(buffer), "%d", value);
  this->mqtt_client_.publish(topic, buffer);
}

bool Dispatcher::sendPlant(Plant plant) {
  // MQTT topic are defined as "<MQTT_BASE_TOPIC>/<MAC_ADDRESS>/<property>"
  const std::string prefix = this->base_topic_ + "/" + plant.mac_addr + "/";

  this->sendMetric(prefix, "temperature", plant.mac_addr,
                   plant.metrics.temperature);
  this->sendMetric(prefix, "conductivity", plant.mac_addr,
                   plant.metrics.conductivity);
  this->sendMetric(prefix, "moisture", plant.mac_addr, plant.metrics.moisture);
  this->sendMetric(prefix, "light", plant.mac_addr, plant.metrics.light);
  this->sendMetric(prefix, "battery", plant.mac_addr, plant.metrics.battery);

  return true;
}
