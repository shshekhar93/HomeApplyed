#include "Arduino.h"
#include "actions.h"
#include "config/config.h"
#include "common/mappers.h"

HomeApplyed::Actions* HomeApplyed::Actions::_instance = NULL;

HomeApplyed::Actions::Actions() {
  pwmPins = 0;
}

HomeApplyed::Actions* HomeApplyed::Actions::getInstance() {
  if(_instance == NULL) {
    _instance = new Actions();
  }
  return _instance;
}

bool HomeApplyed::Actions::initialize() {
  Config* config = Config::getInstance();

  for(uint8_t i = 0; i < ENABLED_LEADS_COUNT; i++) {
    uint8_t dutyPercent = config->getIntValue(toConfigItem(i));
    pinMode(LeadPins[i], OUTPUT);
    setLeadState(i, dutyPercent, false);
  }

  return true;
}

bool HomeApplyed::Actions::setLeadState(uint8_t leadId, uint8_t dutyPercent, bool persist) {
  if(leadId >= ENABLED_LEADS_COUNT) {
    return false;
  }

  if(dutyPercent <= 0) {
    switchOff(leadId);
    dutyPercent = 0;
  }
  else if(dutyPercent >= 100) {
    switchOn(leadId);
    dutyPercent = 100;
  }
  else {
    switchPWM(leadId, dutyPercent);
  }

  if(persist) {
    Config* config = Config::getInstance();
    config->setIntValue(toConfigItem(leadId), dutyPercent);
  }

  return true;
}

void HomeApplyed::Actions::switchOn(uint8_t leadId) {
  Config* config = Config::getInstance();
  bool activeStateLow = config->getBoolValue(ACTIVE_STATE);

  uint32_t pwmFlag = 1 << leadId;
  if(pwmPins & pwmFlag) {
    pinMode(LeadPins[leadId], OUTPUT);
    pwmPins &= ~pwmFlag;
  }
  digitalWrite(LeadPins[leadId], activeStateLow ? LOW : HIGH);
}

void HomeApplyed::Actions::switchOff(uint8_t leadId) {
  Config* config = Config::getInstance();
  bool activeStateLow = config->getBoolValue(ACTIVE_STATE);

  uint32_t pwmFlag = 1 << leadId;
  if(pwmPins & pwmFlag) {
    pinMode(LeadPins[leadId], OUTPUT);
    pwmPins &= ~pwmFlag;
  }
  digitalWrite(LeadPins[leadId], activeStateLow ? HIGH : LOW);
}

void HomeApplyed::Actions::switchPWM(uint8_t leadId, uint8_t dutyPercent) {
  Config* config = Config::getInstance();
  bool activeStateLow = config->getBoolValue(ACTIVE_STATE);
  if(activeStateLow) {
    dutyPercent = 100 - dutyPercent;
  }

  analogWrite(LeadPins[leadId], (int)(10.23 * dutyPercent));
  // Set a flag that pwm is being used for this pin.
  pwmPins |= (1 << leadId);
}