#include "config.h"
#include "common/constants.h"
#include "LittleFS.h"
#include "ArduinoJson.h"

HomeApplyed::Config* HomeApplyed::Config::_instance = NULL;

HomeApplyed::Config::Config() {
  syncPendingItems = 0;

  strcpy(deviceType, EMPTY_STR);
  strcpy(accessPointMac, EMPTY_STR);
  strcpy(accessPointPassword, EMPTY_STR);
  strcpy(stationMac, EMPTY_STR);
  strcpy(userId, EMPTY_STR);
  strcpy(encryptionKey, EMPTY_STR);
  strcpy(hostname, EMPTY_STR);

  // Low level trigger by default
  activeStateLow = true;

  for(uint8_t i = 0; i < ENABLED_LEADS_COUNT; i++) {
    leadState[i] = 0;
  }
}

HomeApplyed::Config* HomeApplyed::Config::getInstance() {
  if(_instance == NULL) {
    _instance = new Config();
  }

  return _instance;
}

bool HomeApplyed::Config::isValidConfigItem(HomeApplyed::ConfigItem key) {
  if(key < LEAD1_STATE) {
    return true;
  }

  return key < MAX_LEAD_KEY;
}

bool HomeApplyed::Config::initialize() {
  if(!LittleFS.begin()) {
    Serial.println(MountFailure);
    return false;
  }
  Serial.println(MountSuccess);

  DynamicJsonDocument doc(1024);
  // Load state file
  File stateFile = LittleFS.open(STATE_FILE, READ_MODE);
  if(!stateFile) {
    Serial.println(FileOpenFail);
    Serial.println(STATE_FILE);
    return false;
  }

  auto err = deserializeJson(doc, stateFile);
  if(err) {
    Serial.println(FileParseFail);
    Serial.println(STATE_FILE);
    return false;
  }

  for(uint8_t i = 0; i < ENABLED_LEADS_COUNT; i++) {
    String leadKey = String(LEAD_PREFIX) + i;
    this->leadState[i] = doc.containsKey(leadKey) ? doc[leadKey] : 0;
  }
  // End of state file loading

  // Load settings file
  File settingsFile = LittleFS.open(SETTINGS_FILE, READ_MODE);
  if(!settingsFile) {
    Serial.println(FileOpenFail);
    Serial.println(SETTINGS_FILE);

    settingsFile = LittleFS.open(BACKUP_FILE, READ_MODE);

    if(!settingsFile) {
      Serial.println(FileOpenFail);
      Serial.println(BACKUP_FILE);

      return false;
    }
  }

  err = deserializeJson(doc, settingsFile);
  if(err) {
    Serial.println(FileParseFail);
    Serial.println(SETTINGS_FILE);
    return false;
  }

  if(doc.containsKey(FStr(AccessPointMac))) {
    strcpy(accessPointMac, doc[FStr(AccessPointMac)]);
  }
  if(doc.containsKey(FStr(AccessPointPassword))) {
    strcpy(accessPointPassword, doc[FStr(AccessPointPassword)]);
  }
  if(doc.containsKey(FStr(StationMac))) {
    strcpy(stationMac, doc[FStr(StationMac)]);
  }
  if(doc.containsKey(FStr(UserId))) {
    strcpy(userId, doc[FStr(UserId)]);
  }
  if(doc.containsKey(FStr(EncryptionKey))) {
    strcpy(encryptionKey, doc[FStr(EncryptionKey)]);
  }
  if(doc.containsKey(FStr(Hostname))) {
    strcpy(hostname, doc[FStr(Hostname)]);
  }
  if(doc.containsKey(FStr(DeviceType))) {
    strcpy(deviceType, doc[FStr(DeviceType)]);
  }
  if(doc.containsKey(FStr(ActiveState))) {
    activeStateLow = doc[FStr(ActiveState)] == "low";
  }
  // End of loading settings file.

  return true;
}

const char* HomeApplyed::Config::getStrValue(HomeApplyed::ConfigItem key) {
  switch(key) {
    case DEVICE_TYPE:
      return deviceType; 
    case HOSTNAME:
      return hostname;
    case USER_ID:
      return userId;
    case ENCRYPTION_KEY:
      return encryptionKey;
    case STATION_MAC:
      return stationMac;
    case ACCESS_POINT_MAC:
      return accessPointMac;
    case ACCESS_POINT_PASSWORD:
      return accessPointPassword;

    // Invalid keys
    default:
      return EMPTY_STR;
  }
}

uint8_t HomeApplyed::Config::getIntValue(HomeApplyed::ConfigItem key) {
  if(!isValidConfigItem(key)) {
    return 0;
  }

  switch(key) {
    case LEAD1_STATE:
      return this->leadState[0];
    case LEAD2_STATE:
      return this->leadState[1];
    case LEAD3_STATE:
      return this->leadState[2];
    case LEAD4_STATE:
      return this->leadState[3];
    case LEAD5_STATE:
      return this->leadState[4];
    case LEAD6_STATE:
      return this->leadState[5];
    case LEAD7_STATE:
      return this->leadState[6];
    case LEAD8_STATE:
      return this->leadState[7];

    // Invalid keys
    default:
      return 0;
  }
}

bool HomeApplyed::Config::getBoolValue(HomeApplyed::ConfigItem key) {
  if(!isValidConfigItem(key)) {
    return false;
  }

  if(key == ACTIVE_STATE) {
    return activeStateLow;
  }

  return false;
}

void HomeApplyed::Config::setStrValue(HomeApplyed::ConfigItem key, const char* value) {
  if(!isValidConfigItem(key)) {
    return;
  }

  char* ptr = NULL;
  switch(key) {
    case DEVICE_TYPE:
      ptr = deviceType;
      break; 
    case HOSTNAME:
      ptr = hostname;
      break;
    case USER_ID:
      ptr = userId;
      break;
    case ENCRYPTION_KEY:
      ptr = encryptionKey;
      break;
    case STATION_MAC:
      ptr = stationMac;
      break;
    case ACCESS_POINT_MAC:
      ptr = accessPointMac;
      break;
    case ACCESS_POINT_PASSWORD:
      ptr = accessPointPassword;
      break;

    // Invalid keys
    default:
      break;
  }
  if(ptr != NULL) {
    strcpy(ptr, value);
  }
  syncPendingItems |= key;
}

void HomeApplyed::Config::setIntValue(HomeApplyed::ConfigItem key, uint8_t value) {
  if(!isValidConfigItem(key)) {
    return;
  }
  
  switch(key) {
    case LEAD1_STATE:
      this->leadState[0] = value;
      break;
    case LEAD2_STATE:
      this->leadState[1] = value;
      break;
    case LEAD3_STATE:
      this->leadState[2] = value;
      break;
    case LEAD4_STATE:
      this->leadState[3] = value;
      break;
    case LEAD5_STATE:
      this->leadState[4] = value;
      break;
    case LEAD6_STATE:
      this->leadState[5] = value;
      break;
    case LEAD7_STATE:
      this->leadState[6] = value;
      break;
    case LEAD8_STATE:
      this->leadState[7] = value;
      break;

    // Invalid keys
    default:
      return;
  }
  syncPendingItems |= key;
}

bool HomeApplyed::Config::save() {
  bool result = true;

  if(syncPendingItems & LEAD_CONFIG_KEYS) {
    DynamicJsonDocument stateDoc(1024);
    JsonObject stateObj = stateDoc.to<JsonObject>();
    for(uint8_t i = 0; i < 4; i++) {
      stateObj[(String)LEAD_PREFIX + i] = leadState[i];
    }

    File stateFile = LittleFS.open(STATE_FILE, WRITE_MODE);
    if(stateFile) {
      serializeJson(stateDoc, stateFile);
      stateFile.close();
    }
    else 
      result = false;
  }

  if(syncPendingItems & SETTINGS_CONFIG_KEYS) {
    DynamicJsonDocument settingsDoc(1024);
    JsonObject settingsObj = settingsDoc.to<JsonObject>();

    settingsObj[FStr(AccessPointMac)] = accessPointMac;
    settingsObj[FStr(AccessPointPassword)] = accessPointPassword;
    settingsObj[FStr(StationMac)] = stationMac;
    settingsObj[FStr(UserId)] = userId;
    settingsObj[FStr(EncryptionKey)] = encryptionKey;
    settingsObj[FStr(Hostname)] = hostname;
    settingsObj[FStr(DeviceType)] = deviceType;
    settingsObj[FStr(ActiveState)] = activeStateLow ? "low": "high";

    File settingsFile = LittleFS.open(SETTINGS_FILE, WRITE_MODE);
    if(settingsFile) {
      serializeJson(settingsDoc, settingsFile);
      settingsFile.close();
    }
    else
      result = false;

    File backupFile = LittleFS.open(BACKUP_FILE, WRITE_MODE);
    if(backupFile) {
      serializeJson(settingsDoc, backupFile);
      backupFile.close();
    }
    else
      result = false;
  }

  syncPendingItems = 0;
  return result;
}
