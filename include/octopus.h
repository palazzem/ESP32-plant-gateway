/**
 * Octopus definition that provides connection capabilities.
 * This definition is used to simplify testing and mocking.
 */
#ifndef OCTOPUS_H
#define OCTOPUS_H

class Octopus {
    public:
        Octopus();
        void initWifi(const char *ssid, const char *password);
        void deinitWiFi();
        void initBluetooth(const char *deviceName);
        void deinitBluetooth();
        void initMQTT(const char *clientId, const char *host, const int port, const char *username, const char *password);
        void deinitMQTT();
        PubSubClient getMQTTClient();
    private:
        PubSubClient m_mqtt_client;
};

#endif
