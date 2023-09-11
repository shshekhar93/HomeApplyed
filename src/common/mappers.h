#ifndef __HOME_APPLYED_MAPPERS__
#define __HOME_APPLYED_MAPPEDS__

#include "config/config.h"

namespace HomeApplyed {
  ConfigItem toConfigItem(uint8_t);
  uint8_t toLeadId(ConfigItem);

  char toHexDigit(uint8_t nibble);
  uint8_t toNumber(char hexDigit);
}

#endif
