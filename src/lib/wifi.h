#ifndef __HOME_APPLYED_WIFI__
#define __HOME_APPLYED_WIFI__
#include "WString.h"

namespace HomeApplyed {
  class WifiController {
    private:
      WifiController();
      static WifiController* _instance;
      uint32_t buttonPressStart;
    
    public:
      static WifiController* getInstance();
      bool initialize();
      bool connect();
      void loop();
      void setupAccessPoint(const String& hostname);
      void clearAndRestart();
  };
}
#endif
