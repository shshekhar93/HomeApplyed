#include "ws-client.h"
#include <cstddef>
#include "config/config.h"
#include "common/crypto.h"
#include "common/mappers.h"
#include "common/logger.h"
#include "lib/actions.h"
#include "lib/updates.h"
#include "ArduinoJson.h"

using namespace websockets;

HomeApplyed::WSClient* HomeApplyed::WSClient::_instance = NULL;
uint32_t HomeApplyed::WSClient::disconnectTs = 0;

/**
 * This assumes we are trying to send a JSON object.
 * Isn't really a problem right now since we only send
 * objects. Refactor this, if that changes in future.
 */
String addFrameNumToJSON(String resp, int frameNum) {
  return resp.substring(0, resp.length() - 1) +
    ",\"frame-num\":" + String(frameNum) + "}";
}

HomeApplyed::WSClient::WSClient()
  : client(NULL)
  , disconnected(false)
  , active(false)
  , response("")
  , frameCount(0) {}

HomeApplyed::WSClient* HomeApplyed::WSClient::getInstance() {
  if(_instance == NULL) {
    _instance = new WSClient();
  }
  return _instance;
}

void HomeApplyed::WSClient::initialize() {
  Config* config = Config::getInstance();
  String encryptionKeyStr = config->getStrValue(ENCRYPTION_KEY);
  uint8_t encryptionKey[KEY_SIZE];
  decodeHex(encryptionKeyStr, encryptionKey, KEY_SIZE);
  
  active = 0;
  frameCount = 0;
  randomBytes(sessionKey);
  String sessionKeyEncoded = encodeHex(sessionKey, KEY_SIZE);
  String payload = DEVICE_ID_PREFIX + HomeApplyed::ChipID + KEY_SEPARATOR + sessionKeyEncoded;
  String password = encrypt(payload, encryptionKey);

  if(client) {
    delete client;
  }
  client = new WebsocketsClient();
  client->addHeader(AUTHORIZATION_HEADER, String(config->getStrValue(USER_ID)) + ":" + password);
  client->addHeader("Sec-WebSocket-Protocol", WS_PROTOCOL);
}

void HomeApplyed::WSClient::onWSEvent(WebsocketsEvent event, String data) {
  switch (event)
  {
    case WebsocketsEvent::ConnectionOpened:
      logE(LogWebSocketConnected);
      disconnectTs = 0;
      break;
    case WebsocketsEvent::ConnectionClosed:
      logE(LogWebSocketDisconnected);
      _instance->disconnected = true;

      if(disconnectTs == 0){
        disconnectTs = millis();
      }
    default:
      return;
  }
}

void HomeApplyed::WSClient::handleMessage(WebsocketsMessage msg) {
  Config* config = Config::getInstance();
  DynamicJsonDocument doc(512);
  String json = decrypt(msg.data(), _instance->sessionKey);
  
  auto error = deserializeJson(doc, json);
  if(error) {
    _instance->sendEncrypted(FailureResponse);
    return ;
  }

  String action = doc[JSON_KEY_ACTION];
  String data = doc[JSON_KEY_DATA];

  uint32_t frameNum = 0; 
  if(doc[JSON_KEY_FRAME].is<uint32_t>()) {
    frameNum = doc[JSON_KEY_FRAME].as<uint32_t>();
  }

  if(frameNum == 0 || frameNum <= _instance->frameCount) {
    return _instance->sendEncrypted(FailureResponse);
  }

  // Ideally we should also check if frameNum is about to overflow and restart.
  // However, 2 billion seems like a high enough number to be safe.
  _instance->frameCount = frameNum;

  String sucessWithFrameNum = addFrameNumToJSON(SuccessResponse, frameNum);
  String failureWithFrameNum = addFrameNumToJSON(FailureResponse, frameNum);

  if(action == ACTION_CONFIRM_SESSION) {
    logE(LogWebSocketKeyConfirm);
    String storedSessKey = encodeHex(_instance->sessionKey, 16);
    if(storedSessKey == data) {
      logE(LogWebSocketKeyConfirmSuccess);
      _instance->active = true;
      _instance->sendEncrypted(sucessWithFrameNum);
    }
    else {
      _instance->sendEncrypted(failureWithFrameNum);
    }
    return;
  }

  // The server hasn't proven itself yet. don't process anything else.
  if(!_instance->active) {
    logE(LogWebSocketConnectionInactive);
    return _instance->sendEncrypted(failureWithFrameNum);
  }

  // Update the key!
  if(action == ACTION_UPDATE_KEY) {
    logE(LogWebSocketSetKey);
    config->setStrValue(ENCRYPTION_KEY, data.c_str());
    _instance->shouldSaveConfig = true;
    _instance->sendEncrypted(sucessWithFrameNum);
    return;
  }

  // Update the username!
  if(action == ACTION_UPDATE_USERNAME) {
    logE(LogWebSocketSetUser);
    config->setStrValue(USER_ID, data.c_str());
    _instance->shouldSaveConfig = true;
    _instance->sendEncrypted(sucessWithFrameNum);
    return;
  }

  // Set state
  if(action == ACTION_SET_STATE) {
    logE(LogWebSocketSetState);
    uint8_t eqIdx = data.indexOf('=');
    uint8_t devId = data.substring(0, eqIdx).toInt();
    uint8_t brightness = data.substring(eqIdx + 1).toInt();
    Actions::getInstance()->setLeadState(devId, brightness, true);
    _instance->shouldSaveConfig = true;
    _instance->sendEncrypted(sucessWithFrameNum);
    return;
  }

  // Get state
  if(action == ACTION_GET_STATE) {
    logE(LogWebSocketGetState);
    String state = SuccessResponse.substring(0, SuccessResponse.length() - 1) + 
      String(",\"frame-num\":") + String(frameNum) +
      String(",\"type\":\"") + config->getStrValue(DEVICE_TYPE) + "\"" +
      String(",\"version\":\"") + Updates::getFullVersion() + "\"";
    for(uint8_t i = 0; i < ENABLED_LEADS_COUNT; i++) {
      state += ",\"lead" + String(i) + "\":" + String(config->getIntValue(toConfigItem(i)));
    }
    state += "}";
    _instance->sendEncrypted(state);
    return;
  }

  // Update Firmware
  if(action == ACTION_UPDATE_FIRMWARE) {
    logE(LogWebSocketUpdateReq);
    Updates::update(data.c_str());
  }

  // Update FS
  if(action == ACTION_UPDATE_FS) {
    logE(LogWebSocketFSUpdateReq);
    Updates::updateFS(data.c_str());
  }
}

void HomeApplyed::WSClient::sendEncrypted() {
  if(response == "") {
    logE(LogWebSocketBufEmpty);
    return;
  }

  String encrypted = encrypt(response.c_str(), sessionKey);
  client->send(encrypted);
  response = "";
}

void HomeApplyed::WSClient::sendEncrypted(const String& resp) {
  if(response != "") { // first clear old buffer
    logE(LogWebSocketOldSend);
    sendEncrypted();
  }
  response = resp;
}

bool HomeApplyed::WSClient::connect() {
  initialize();
  const char* host = Config::getInstance()->getStrValue(HOSTNAME);
  if(host[0] == 0) {
    host = DEFAULT_HOST_ADDRESS;
  }
  
  client->onEvent(onWSEvent);
  client->onMessage(handleMessage);
  Serial.printf("Will connect to %s\n", host);
  return client->connect(host, WS_PORT, WS_PATH);
}

void HomeApplyed::WSClient::loop() {
  if(disconnected) {
    // We have not been able to connect for last 2.5 minute.
    // Simply restart.
    if((millis() - disconnectTs) > 150000ul) {
      logE(LogServerConnectFailed);
      ESP.restart();
    }

    delay(HomeApplyed::RESTART_DELAY);
    disconnected = !connect();
    return;
  }

  if(response != "") {
    logE(LogWebSocketFlush);
    yield();
    sendEncrypted();
    return;
  }

  if(shouldSaveConfig) {
    logE(LogWebSocketConfigSave);
    yield();
    Config::getInstance()->save();
    shouldSaveConfig = false;
    return;
  }

  if(client->available()) {
    client->poll();
  }
}
