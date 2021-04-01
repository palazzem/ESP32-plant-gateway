#ifndef MOCK_BLU_REMOTE_CHARACTERISTIC_H_
#define MOCK_BLU_REMOTE_CHARACTERISTIC_H_

#include <string>

class BLERemoteCharacteristic {
 public:
  void writeValue(uint8_t* data, size_t length, bool response = false);
  std::string readValue();
};

#endif  // MOCK_BLU_REMOTE_CHARACTERISTIC_H_
