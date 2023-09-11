#include "updates.h"
#include "common/constants.h"
#include "config/config.h"
#include "WiFiClient.h"
#include "Arduino.h"

#ifdef ESP8266 
  ESP8266HTTPUpdate& httpUpdate = ESPhttpUpdate;
#endif

char HomeApplyed::Updates::softwareVersion[6] = SOFTWARE_VERSION;
char HomeApplyed::Updates::hardwareModel[12] = HOME_APPLYED_BOARD;

String HomeApplyed::Updates::getFullVersion() {
  hardwareModel[11] = 0;
  return String(softwareVersion) + "-" + String(hardwareModel);
}

void HomeApplyed::Updates::handleUpdateResult(HTTPUpdateResult updateResp) {
  switch (updateResp)
  {
    case HTTP_UPDATE_OK:
      Serial.println("UPDATE_INSTALLED");
      ESP.restart();
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("NO_UPDATE_AVLBL");
      break;
    case HTTP_UPDATE_FAILED:
      Serial.println(F("UPDATE_FAILED"));
      Serial.println(httpUpdate.getLastErrorString());
      break;
    default:
      Serial.println(F("UNSUP_RET_CODE"));
      break;
  }
}

void HomeApplyed::Updates::update(const char* url) {
  WiFiClient wifiClient;
  Config* config = Config::getInstance();

  httpUpdate.rebootOnUpdate(true);
  HTTPUpdateResult updateResp = httpUpdate.update(
    wifiClient,
    config->getStrValue(HOSTNAME),
    80,
    url,
    getFullVersion()
  );

  handleUpdateResult(updateResp);
}



void HomeApplyed::Updates::updateFS(const char* url) {
  WiFiClient wifiClient;
  Config* config = Config::getInstance();
  httpUpdate.rebootOnUpdate(true);
  #ifdef ESP8266
  HTTPUpdateResult updateResp = httpUpdate.updateFS(
  #else
  HTTPUpdateResult updateResp = httpUpdate.updateSpiffs(
  #endif
    wifiClient,
    String(config->getStrValue(HOSTNAME)) + String(80) + String(url),
    getFullVersion()
  );
  handleUpdateResult(updateResp);
}
