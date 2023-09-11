#include "crypto.h"
#include "Arduino.h"
#include "Crypto.h"
#include "AES.h"
#include "CTR.h"
#include "common/mappers.h"

#ifdef ESP8266
  uint32_t esp_random() {
    return RANDOM_REG32;
  }
#endif

CTR<AES128> cipher;

void HomeApplyed::randomBytes(uint8_t output[]) {
  for(uint8_t i = 0; i < 4; i++) {
    uint32_t randomNum = esp_random();

    uint8_t offset = i * 4;
    for(uint8_t j = 0; j < 4; j++) {
      output[offset + j] = randomNum & 0xFF;
      randomNum = randomNum >> 8;
    }
  }
}

uint32_t HomeApplyed::decodeHex(String hexStr, uint8_t output[], uint32_t maxLen) {
  uint32_t byteLen = hexStr.length() / 2;

  if(byteLen > maxLen) {
    return 0;
  }

  for(uint32_t i = 0; i < byteLen; i++) {
    uint32_t hexIdx = i * 2;
    output[i] = (toNumber(hexStr[hexIdx]) << 4) + toNumber(hexStr[hexIdx + 1]);
  }

  return byteLen;
}

String HomeApplyed::encodeHex(const uint8_t* bytes, uint8_t len) {
  char hex[len * 2 + 1];
  hex[len * 2] = '\0';
  for(uint32_t i = 0; i < len; i++) {
    uint32_t hexIdx = i * 2;
    hex[hexIdx] = toHexDigit(bytes[i] >> 4);
    hex[hexIdx + 1] = toHexDigit(bytes[i]);
  }

  return (String)hex;
}

String HomeApplyed::encrypt(String plainText, uint8_t key[]) {
  uint8_t iv[KEY_SIZE];
  randomBytes(iv);

  cipher.clear();
  cipher.setIV(iv, KEY_SIZE);
  cipher.setKey(key, KEY_SIZE);

  uint32_t len = plainText.length();
  uint8_t paddingLen = KEY_SIZE - (len % KEY_SIZE);
  char padding[paddingLen];
  memset(padding, ' ', paddingLen);
  plainText = plainText + padding;
  len += paddingLen;

  uint8_t output[len];
  cipher.encrypt(output, (uint8_t*)plainText.c_str(), len);
  return encodeHex(iv, KEY_SIZE) + '-' + encodeHex(output, len);
}

/**
 * `input`: encrypted information in form of iv(32) + hyphen(1) + cipherText(....)
 * `key`: Byte array of key to be used for decryption
*/
String HomeApplyed::decrypt(String input, uint8_t key[]) {
  // Extract the IV 
  uint8_t iv[33];
  memcpy(iv, input.c_str(), ENCODED_KEY_SIZE);
  iv[32] = 0;
  decodeHex((char*)iv, iv, KEY_SIZE);

  // Extract the cipher text
  uint32_t cipherLen = input.length() - ENCODED_KEY_SIZE - 1;
  uint8_t cipherText[cipherLen + 1];
  memcpy(cipherText, input.c_str() + ENCODED_KEY_SIZE + 1, cipherLen);
  cipherText[cipherLen] = 0;
  cipherLen = decodeHex((char*)cipherText, cipherText, cipherLen);

  // Cipher setup
  cipher.clear();
  cipher.setIV(iv, KEY_SIZE);
  cipher.setKey(key, KEY_SIZE);

  char plainText[cipherLen + 1];
  cipher.decrypt((uint8_t*)plainText, cipherText, cipherLen);
  plainText[cipherLen] = 0;
  return plainText;
}
