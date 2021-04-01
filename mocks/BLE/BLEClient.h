#ifndef MOCK_BLE_CLIENT_H_
#define MOCK_BLE_CLIENT_H_

#include <BLEUUID.h>
#include <BLEAddress.h>
#include <BLERemoteService.h>

class BLEClient {
public:
  BLEClient();
  
  bool connect(BLEAddress address);
  void disconnect();
  BLERemoteService* getService(BLEUUID uuid);
  bool isConnected();
};

#endif  // MOCK_BLE_CLIENT_H_
