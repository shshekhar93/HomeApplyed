#include "wifi.h"
#include "Arduino.h"
#include "config/config.h"
#include "common/crypto.h"
#include "common/constants.h"
#include "WiFiManager.h"

#ifdef ESP8266
#include "ESP8266WiFi.h"
#else
#include "WiFi.h"
#include "esp_wifi.h"
#endif

HomeApplyed::WifiController* HomeApplyed::WifiController::_instance = NULL;
WiFiManager wifiManager;

HomeApplyed::WifiController::WifiController()
  :buttonPressStart(0) {}

HomeApplyed::WifiController* HomeApplyed::WifiController::getInstance() {
  if(_instance == NULL) {
    _instance = new WifiController();
  }
  return _instance;
}

void HomeApplyed::WifiController::clearAndRestart() {
  Serial.println("CLR_N_RESRT");
  wifiManager.resetSettings();
  delay(RESTART_DELAY);
  ESP.restart();
}

bool HomeApplyed::WifiController::initialize() {
  pinMode(D8, INPUT);
  const char* APMacStr = Config::getInstance()->getStrValue(ACCESS_POINT_MAC);
  const char* STMacStr = Config::getInstance()->getStrValue(STATION_MAC);

  uint8_t APMac[6];
  uint8_t STAMac[6];
  decodeHex(APMacStr, APMac, 6);
  decodeHex(STMacStr, STAMac, 6);
  
  WiFi.mode(WIFI_AP_STA);
  yield();
  #ifdef ESP8266
  if(!wifi_set_macaddr(SOFTAP_IF, APMac)) {
    Serial.println("SET_AP_MAC_FAIL");
    return false;
  }
  #else
  if(esp_wifi_set_mac(WIFI_IF_AP, APMac) != ESP_OK) {
    Serial.println("SET_AP_MAC_FAIL");
    return false;
  } 
  #endif
  yield();
  #ifdef ESP8266
  if(!wifi_set_macaddr(STATION_IF, STAMac)) {
    Serial.println("SET_STA_MAC_FAIL");
    return false;
  }
  #else
  if(esp_wifi_set_mac(WIFI_IF_STA, STAMac) != ESP_OK) {
    Serial.println("SET_STA_MAC_FAIL");
    return false;
  }
  #endif
  yield();

  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());

  String hostname = DEVICE_ID_PREFIX + ChipID;
  WiFi.hostname(hostname);

  WiFi.mode(WIFI_STA);
  if(WiFi.SSID() != "") {
    WiFi.begin();

    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
      setupAccessPoint(hostname);
    }
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  else {
    setupAccessPoint(hostname);
  }
  return true;
}

void HomeApplyed::WifiController::setupAccessPoint(const String& ssid) {
  #ifdef ESP8266
    wifiManager.setTitle("Home Applyed");
  #else
    wifiManager.setTitle("\
<p style=\"text-align:center\">\
<svg style=\"height:100px\" viewBox=\"0 0 500 500\" stroke=\"currentColor\" stroke-width=\"25\" \
stroke-linecap=\"round\" stroke-linejoin=\"round\">\
<path style=\"fill: transparent;\" d=\"M 220 245 A50,50 0 1 0 280,245 M 250 270 L 250 225\"/>\
<path style=\"fill: transparent;\" d=\"M250,400 L 400 400 A50,50 0 0 0 400,300 C500,300 400,150 350,200 A105,105 0 0 0 250,125\"/>\
<path style=\"fill: transparent;\" d=\"M250,400 L 400 400 A50,50 0 0 0 400,300 C500,300 400,150 350,200 A105,105 0 0 0 250,125\"\
transform=\"scale(-1,1) translate(-500,0)\"/>\
</svg>\
</p>\
<p>Home Applyed");
  #endif
  WiFiManagerParameter userIdParam("user", "Registered username", "", 32);
  WiFiManagerParameter passwordParam("pass", "Temporary password", "", 32);
  WiFiManagerParameter hostnameParam("host", "Hub address (optional)", "", 32);

  wifiManager.addParameter(&userIdParam);
  wifiManager.addParameter(&passwordParam);
  wifiManager.addParameter(&hostnameParam);
  wifiManager.setConfigPortalTimeout(300);
  const char* apPassword = Config::getInstance()->getStrValue(ACCESS_POINT_PASSWORD);
  Serial.print("AP Password: ");
  Serial.println(apPassword);
  wifiManager.autoConnect(ssid.c_str(), apPassword);
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("WIFI_CNCT_FAIL");
    ESP.restart();
  }

  const char* userId = userIdParam.getValue();
  const char* password = passwordParam.getValue();
  const char* hostname = hostnameParam.getValue();

  Config* _config = Config::getInstance();
  if(userId[0] == '\0') {
    userId = _config->getStrValue(USER_ID);
  }
  if(password[0] == '\0') {
    password = _config->getStrValue(ENCRYPTION_KEY);
  }
  if(hostname[0] == '\0'){
    hostname = _config->getStrValue(HOSTNAME);
  }

  if(userId[0] == '\0' || password[0] == '\0' || hostname[0] == '\0') {
    // User did not provide a mandatory parameter.
    clearAndRestart();
  }

  // Save in config.json
  _config->setStrValue(HOSTNAME, hostname);
  _config->setStrValue(ENCRYPTION_KEY, password);
  _config->setStrValue(USER_ID, userId);
  if(!_config->save()) {
    clearAndRestart();
  }
}

void HomeApplyed::WifiController::loop() {
  if(digitalRead(D8) == 1) {
    if(buttonPressStart == 0) {
      buttonPressStart = millis();
    }
    else if(millis() - buttonPressStart > 3000) {
      this->clearAndRestart();
    }
  }
  else {
    buttonPressStart = 0;
  }
}