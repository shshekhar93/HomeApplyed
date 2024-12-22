#ifndef __HOME_APPLYED_CONFIG__
#define __HOME_APPLYED_CONFIG__

#include "stddef.h"
#include "stdint.h"
#include "common/constants.h"
#include "ArduinoJson.h"

namespace HomeApplyed {
  enum ConfigItem {
    DEVICE_TYPE = 0x01,
    ACTIVE_STATE = 0x02,
    HOSTNAME = 0x04,
    USER_ID = 0x08,
    ENCRYPTION_KEY = 0x10,
    STATION_MAC = 0x20,
    ACCESS_POINT_MAC = 0x40,
    ACCESS_POINT_PASSWORD = 0x80,
    LEAD1_STATE = 0x0100,
    LEAD2_STATE = 0x0200,
    LEAD3_STATE = 0x0400,
    LEAD4_STATE = 0x0800,
    LEAD5_STATE = 0x1000,
    LEAD6_STATE = 0x2000,
    LEAD7_STATE = 0x4000,
    LEAD8_STATE = 0x8000,
  };

  const uint32_t SETTINGS_CONFIG_KEYS = 0x00FF;
  const uint32_t LEAD_CONFIG_KEYS = 0xFF00;
  const uint32_t MAX_LEAD_KEY = (LEAD1_STATE << ENABLED_LEADS_COUNT);

  class Config {
    private:
      static Config* _instance;

      Config();

      uint32_t syncPendingItems = 0;
      DynamicJsonDocument* jsonDoc;

      char deviceType[17];
      char accessPointMac[13];
      char accessPointPassword[25];
      char stationMac[13];
      char userId[33];
      char encryptionKey[33];
      char hostname[33];
      bool activeStateLow;
      uint8_t leadState[ENABLED_LEADS_COUNT];

      bool isValidConfigItem(ConfigItem key);
      bool loadJSONFile(const char* filename);
      bool loadState();
      bool loadSettings();

    public: 
      static Config* getInstance();
      bool initialize();
      const char* getStrValue(ConfigItem key);
      uint8_t getIntValue(ConfigItem key);
      bool getBoolValue(ConfigItem key);

      void setStrValue(ConfigItem key, const char* value);
      void setIntValue(ConfigItem key, uint8_t value);
      /* Boolean values are not mutable yet */
      // void setBoolValue(ConfigItem key, bool value);
      bool save();
  };
}

#endif