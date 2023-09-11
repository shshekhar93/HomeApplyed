#ifndef __HOME_APPLYED_ACTIONS__
#define __HOME_APPLYED_ACTIONS__

#include "Arduino.h"
#include "common/constants.h"

namespace HomeApplyed {
  const uint8_t LeadPins[MAX_LEADS_COUNT] = { D0, D1, D2, D3, D4, D5, D6, D7 };

  class Actions {
    private:
      Actions();

      static  Actions* _instance;
      /**
       * Calling digitalWrite after analogWrite doesn't work on esp32
       * This is most likely due the timer being run in background, which
       * immediately overwrites the digitalWrite's value.
       * 
       * The workaround is to set pinMode OUTPUT one more time when swit-
       * ching from analog mode to digital. This set of flag remembers
       * whenever we used analogWrite for a pin so that we can reset the
       * pinMode when using digitalWrite the next time.
      */
      uint32_t pwmPins;

      void switchOn(uint8_t leadId);
      void switchOff(uint8_t leadId);
      void switchPWM(uint8_t leadId, uint8_t dutyPercent);

    public:
      static Actions* getInstance();
      bool initialize();
      bool setLeadState(uint8_t leadId, uint8_t dutyPercent, bool persist = true);
  };
}

#endif