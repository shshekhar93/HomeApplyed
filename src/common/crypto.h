#ifndef __HOME_APPLYED_CRYPTO__
#define __HOME_APPLYED_CRYPTO__

#include "WString.h"
#include "stdint.h"

namespace HomeApplyed {
  void randomBytes(uint8_t output[]);
  uint32_t decodeHex(String hexStr, uint8_t output[], uint32_t maxLen);
  String encodeHex(const uint8_t* bytes, uint8_t len);

  String encrypt(String plainText, uint8_t key[]);
  String decrypt(String cipherText, uint8_t key[]);
}

#endif
