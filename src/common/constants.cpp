#include "constants.h"
#include "Arduino.h"

#ifdef ESP8266
    String HomeApplyed::ChipID = String(ESP.getChipId(), HEX);

    const __FlashStringHelper* HomeApplyed::FStr(const char* str) {
      return FPSTR(str);
    }
#else
  uint64_t fuseMAC = ESP.getEfuseMac();
  String HomeApplyed::ChipID = String(fuseMAC >> 32, HEX) + String(fuseMAC & 0xFFFFFFFF, HEX);

  const char* HomeApplyed::FStr(const char* str) {
    return str;
  }
#endif

String HomeApplyed::FailureResponse = "{\"status\":\"FAIL\"}";
String HomeApplyed::SuccessResponse = "{\"status\":\"OK\"}";
