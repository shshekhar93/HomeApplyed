#ifndef __HOME_APPLYED_CONSTANTS__
#define __HOME_APPLYED_CONSTANTS__

#include "pgmspace.h"
#include "stdint.h"
#include "WString.h"

#define MAX_LEADS_COUNT       8
#define ENABLED_LEADS_COUNT   4
#define KEY_SIZE              16
#define ENCODED_KEY_SIZE      32
#define SOFTWARE_VERSION      "2.0.0"
#define HARDWARE_MODEL        "ESP32S3"

namespace HomeApplyed {
  // Magic constants
  const char EMPTY_STR[] = "";
  const char STATE_FILE[] = "/state.json";
  const char SETTINGS_FILE[] = "/settings.json";
  const char BACKUP_FILE[] = "/backup.json";
  const char READ_MODE[2] = "r";
  const char WRITE_MODE[2] = "w";
  const char LEAD_PREFIX[] = "lead";
  const char DEFAULT_HOST_ADDRESS[] = "home.applyed.in";
  const int BaudRate = 115200;
  const int RESTART_DELAY = 2500;
  const int LOOP_DELAY = 100;
  extern String FailureResponse;
  extern String SuccessResponse;

  // Protocol constants
  extern String ChipID;
  const uint16_t WS_PORT = 80;
  const char WS_PATH[] = "/v1/ws";
  const char WS_PROTOCOL[] = "myhomenew-device";
  const char DEVICE_ID_PREFIX[] = "myhomenew-";
  const char KEY_SEPARATOR[] = "|";
  const char AUTHORIZATION_HEADER[] = "Authorization";
  const uint32_t WS_RECONNECT_INTERVAL = 5000;
  const char JSON_KEY_ACTION[] = "action";
  const char JSON_KEY_DATA[] = "data";
  const char JSON_KEY_FRAME[] = "frame-num";
  const char ACTION_CONFIRM_SESSION[] = "confirm-session";
  const char ACTION_UPDATE_KEY[] = "update-key";
  const char ACTION_UPDATE_USERNAME[] = "update-username";
  const char ACTION_SET_STATE[] = "set-state";
  const char ACTION_GET_STATE[] = "get-state";
  const char ACTION_UPDATE_FIRMWARE[] = "firmware-update";
  const char ACTION_UPDATE_FS[] = "spiffs-update";

  
  // Config keys
  const char AccessPointMac[] PROGMEM = "ap_mac";
  const char AccessPointPassword[] PROGMEM = "ap_pass";
  const char StationMac[] PROGMEM = "sta_mac";
  const char UserId[] PROGMEM = "user_id";
  const char EncryptionKey[] PROGMEM = "enc_key";
  const char Hostname[] PROGMEM = "hostname";
  const char DeviceType[] PROGMEM = "dev_type";
  const char ActiveState[] PROGMEM = "active_state";

  // Logging messages
  const char MountSuccess[] PROGMEM = "fs_sucs";
  const char MountFailure[] PROGMEM = "fs_fail";
  const char FileOpenFail[] PROGMEM = "fo_fail";
  const char FileParseFail[] PROGMEM = "fp_fail";

#ifdef ESP8266
  const __FlashStringHelper* FStr(const char*);
#else
  const char* FStr(const char*);
#endif
}

#endif