#ifndef __HOME_APPLYED_UPDATES__
#define __HOME_APPLYED_UPDATES__
#include "WString.h"
#ifdef ESP8266
#include "ESP8266httpUpdate.h"
#else
#include "HTTPUpdate.h"
#endif

namespace HomeApplyed {
  class Updates {
    private:
    static void handleUpdateResult(HTTPUpdateResult);
    public:
    static char softwareVersion[6];
    static char hardwareModel[12];

    static String getFullVersion();
    static void update(const char*);
    static void updateFS(const char*);
  };
}

#endif
