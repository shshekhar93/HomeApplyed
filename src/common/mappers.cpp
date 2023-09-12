#include "mappers.h"
#include "MD5Builder.h"


HomeApplyed::ConfigItem HomeApplyed::toConfigItem(uint8_t leadId) {
  return (HomeApplyed::ConfigItem)(HomeApplyed::LEAD1_STATE << leadId);
} 

uint8_t HomeApplyed::toLeadId(HomeApplyed::ConfigItem key) {
  if(key < HomeApplyed::LEAD1_STATE) {
    return 0;
  }

  auto lsbs = key / HomeApplyed::LEAD1_STATE;
  uint8_t leadId = 0;

  while(lsbs > 0) {
    lsbs = lsbs >> 1;
    leadId++;
  }
  return leadId - 1;
}

char HomeApplyed::toHexDigit(uint8_t nibble) {
  nibble &= 0x0f;

  if(nibble >= 0 && nibble <= 9) {
    return '0' + nibble;
  }
  else if (nibble >= 10 && nibble <= 15) {
    return 'a' + nibble - 10;
  }
  return '0';
}

uint8_t HomeApplyed::toNumber(char hexDigit) {
  if(hexDigit >= '0' && hexDigit <= '9') {
    return hexDigit - '0';
  }
  else if (hexDigit >= 'a' && hexDigit <= 'f') {
    return hexDigit - 'a' + 10;
  }
  else if (hexDigit >= 'A' && hexDigit <= 'F') {
    return hexDigit - 'A' + 10;
  }
  return 0;
}

String HomeApplyed::md5(const char* str) {
  MD5Builder md5Builder;
  md5Builder.begin();
  md5Builder.add(str);
  md5Builder.calculate();
  return md5Builder.toString();
}
