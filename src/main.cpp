#include "Arduino.h"
#include "LittleFS.h"

#include "common/constants.h"
#include "common/logger.h"
#include "config/config.h"
#include "lib/actions.h"
#include "lib/wifi.h"
#include "lib/ws-client.h"

HomeApplyed::WifiController* wifiController = HomeApplyed::WifiController::getInstance();
HomeApplyed::WSClient* wsClient = HomeApplyed::WSClient::getInstance();

void setup()
{
  // Setup Serial Console
  Serial.begin(HomeApplyed::BaudRate);

  // debugging delay
  delay(HomeApplyed::RESTART_DELAY);

  // Setup config
  HomeApplyed::Config* config = HomeApplyed::Config::getInstance();
  bool configInitSuccess = config->initialize();
  if(!configInitSuccess) {
    HomeApplyed::logE(HomeApplyed::LogConfigInitFail);
    delay(HomeApplyed::RESTART_DELAY);
    ESP.restart();
  }

  // Setup Actions
  HomeApplyed::Actions* actions = HomeApplyed::Actions::getInstance();
  bool actionsInitSuccess = actions->initialize();
  if(!actionsInitSuccess) {
    HomeApplyed::logE(HomeApplyed::LogActionsInitFail);
    delay(HomeApplyed::RESTART_DELAY);
    ESP.restart();
  }

  // Setup Wifi
  bool wifiInitSuccess = wifiController->initialize();
  if(!wifiInitSuccess) {
    HomeApplyed::logE(HomeApplyed::LogWifiInitFail);
    delay(HomeApplyed::RESTART_DELAY);
    ESP.restart();
  }

  // Setup Web Socket Client
  bool wsConnectSuccess = wsClient->connect();
  if(!wsConnectSuccess) {
    HomeApplyed::logE(HomeApplyed::LogWebSocketConnectFail);
    delay(HomeApplyed::RESTART_DELAY);
    ESP.restart();
  }
}

void loop()
{
  wifiController->loop();
  wsClient->loop();
  delay(HomeApplyed::LOOP_DELAY);
}