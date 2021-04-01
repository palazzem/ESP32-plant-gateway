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

#include "config.h"

class Dispatcher {
 private:
  PubSubClient mqtt_client_;

 public:
  Dispatcher(AppConfig config);
  ~Dispatcher();
  bool publish(const char *topic, const char *payload);
};

#endif  // LIB_DISPATCHER_DISPATCHER_H_
