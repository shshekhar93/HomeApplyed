#ifndef __HOME_APPLYED_WS_CLIENT__
#define __HOME_APPLYED_WS_CLIENT__
#include "stdint.h"
#include "WString.h"
#include "ArduinoWebsockets.h"
using namespace websockets;

namespace HomeApplyed {
  class WSClient {
    private:
      WSClient();
      static WSClient* _instance;
      static uint32_t disconnectTs;
      WebsocketsClient* client;
      bool disconnected;
      bool active;
      bool shouldSaveConfig;
      uint8_t sessionKey[16];
      String response;
      uint32_t frameCount;

      void initialize();
      static void onWSEvent(WebsocketsEvent event, String data);
      static void handleMessage(WebsocketsMessage msg);
      void sendEncrypted();
      void sendEncrypted(const String& resp);
    
    public: 
      static WSClient* getInstance();
      void connect();
      void loop();
  };
}

#endif
