/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "LittleFS.h"

#include "common/constants.h"
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

  // Setup config
  HomeApplyed::Config* config = HomeApplyed::Config::getInstance();
  bool configInitSuccess = config->initialize();
  if(!configInitSuccess) {
    Serial.println("CONF_INIT_FAIL");
    delay(2500);
    ESP.restart();
  }

  // Setup Actions
  HomeApplyed::Actions* actions = HomeApplyed::Actions::getInstance();
  bool actionsInitSuccess = actions->initialize();
  if(!actionsInitSuccess) {
    Serial.println("ACT_INIT_FAIL");
    delay(2500);
    ESP.restart();
  }

  // Setup Wifi
  bool wifiInitSuccess = wifiController->initialize();
  if(!wifiInitSuccess) {
    Serial.println("WIFI_INIT_FAIL");
    delay(2500);
    ESP.restart();
  }

  // Setup Web Socket Client
  wsClient->connect();
}

void loop()
{
  wifiController->loop();
  wsClient->loop();
}