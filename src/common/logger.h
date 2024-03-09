#ifndef __HOME_APPLYED_LOGGER__
#define __HOME_APPLYED_LOGGER__

namespace HomeApplyed {
  enum LogEnum {
    // config.cpp
    LogMountSuccess = 1000, // fs_sucs
    LogMountFailure, // fs_fail
    LogFileOpenFail, // fo_fail
    LogFileParseFail, // fp_fail
    LogConfigInitFail, // CONF_INIT_FAIL
    LogActionsInitFail, // ACT_INIT_FAIL

    // wifi.cpp 
    LogWifiInitFail = 2000, // WIFI_INIT_FAIL
    LogFactoryReset, // CLR_N_RESRT
    LogSetAPMacFail, // SET_AP_MAC_FAIL
    LogSetSTAMacFail, // SET_STA_MAC_FAIL
    LogWiFiConnectFail, // WIFI_CNCT_FAIL
    LogWebSocketConnectFail, // 2005
    LogWiFiDisconnected, // 2006

    // updates.cpp
    LogUpdateInstalled = 3000, // UPDATE_INSTALLED
    LogNoUpdateAvailable, // NO_UPDATE_AVAILABLE
    LogUpdateFailed, // UPDATE_FAILED
    LogUnsupportedReturnCode,

    // ws-client.cpp
    LogWebSocketConnected = 4000, // WSC_CON
    LogWebSocketDisconnected, // WSC_DIS
    LogServerConnectFailed, // SRVR_CNCT_FAIL
    LogWebSocketKeyConfirm, // CNF_KEY
    LogWebSocketKeyConfirmSuccess, // CNF_KEY_SCS
    LogWebSocketConnectionInactive, // WS_CONN_INACTV
    LogWebSocketSetKey, // SET_KEY
    LogWebSocketSetUser, // SET_USR
    LogWebSocketSetState, // SET_STATE
    LogWebSocketGetState, // GET_STATE
    LogWebSocketUpdateReq, // UPDT_REQ
    LogWebSocketFSUpdateReq, // UPDT_FS_REQ
    LogWebSocketBufEmpty, // BUF_EMPTY
    LogWebSocketOldSend, //OLD_SND
    LogWebSocketFlush, // WS_FLUSH
    LogWebSocketConfigSave, // CNF_SAVE
  };

  void logE(LogEnum);
}

#endif
