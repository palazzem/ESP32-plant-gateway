/**
 * Octopus definition that provides connection capabilities.
 * This definition is used to simplify testing and mocking.
 */
#ifndef OCTOPUS_H
#define OCTOPUS_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <config.h>

class Octopus {
private:
  PubSubClient m_mqtt_client;

public:
  Octopus();
  void initWifi(const char *ssid, const char *password);
  void deinitWiFi();
  void initMQTT(const char *clientId, const char *host, const int port,
                const char *username, const char *password);
  void deinitMQTT();
  bool publish(const char *topic, const char *payload);
};

#endif
