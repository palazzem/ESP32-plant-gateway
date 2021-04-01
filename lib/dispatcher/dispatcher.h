/**
 * Dispatcher definition that provides connection capabilities.
 * The responsibility of this class is to bring up the network stack and
 * send messages to a destination.
 *
 * In the current implementation, MQTT is used to deliver messages.
 */
#ifndef LIB_DISPATCHER_DISPATCHER_H_
#define LIB_DISPATCHER_DISPATCHER_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include <string>

#include "config.h"
#include "plant.h"

class Dispatcher {
 private:
  std::string base_topic_;
  WiFiClient wifi_client_;
  PubSubClient mqtt_client_;

  void sendMetric(const std::string prefix, const std::string metric_name,
                  const char *mac_addr, const float value);
  void sendMetric(const std::string prefix, const std::string metric_name,
                  const char *mac_addr, const int value);

 public:
  Dispatcher(AppConfig config);
  ~Dispatcher();

  bool sendPlant(Plant plant);
};

#endif  // LIB_DISPATCHER_DISPATCHER_H_
