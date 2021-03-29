#include <WiFi.h>

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
