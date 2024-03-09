#include "logger.h"
#include "common/constants.h"
#include "Arduino.h"

void HomeApplyed::logE(LogEnum e) {
  Serial.print(LOG_ENUM_PREFIX);
  Serial.println(e);
}