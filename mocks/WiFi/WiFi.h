#ifndef MOCK_WIFI_H
#define MOCK_WIFI_H

typedef enum {
  WL_NO_SHIELD = 255,
  WL_IDLE_STATUS = 0,
  WL_NO_SSID_AVAIL = 1,
  WL_SCAN_COMPLETED = 2,
  WL_CONNECTED = 3,
  WL_CONNECT_FAILED = 4,
  WL_CONNECTION_LOST = 5,
  WL_DISCONNECTED = 6
} wl_status_t;

class WiFiClass {
public:
  WiFiClass();

  wl_status_t begin(const char *ssid, const char *passphrase);
  wl_status_t status();
  bool disconnect(bool wifioff);
};

extern WiFiClass WiFi;

#endif
