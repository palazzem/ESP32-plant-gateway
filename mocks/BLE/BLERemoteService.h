#ifndef MOCK_BLE_REMOTE_SERVICE_H_
#define MOCK_BLE_REMOTE_SERVICE_H_

#include <BLERemoteCharacteristic.h>
#include <BLEUUID.h>

class BLERemoteService {
 public:
  BLERemoteService();

  BLERemoteCharacteristic* getCharacteristic(BLEUUID uuid);
};

#endif  // MOCK_BLE_REMOTE_SERVICE_H_
