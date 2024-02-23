#pragma once

#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <include/WiFiState.h> // WiFiState structure details

// van ESP8266WiFi/examples/WiFiShutdown/WiFiShutdown.ino
#ifndef RTC_config_data_SLOT_WIFI_STATE
#define RTC_config_data_SLOT_WIFI_STATE 33u
#endif

// We make a structure to store connection information
// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods
// read and write 4 bytes at a time, so the RTC data structure should be padded
// to a 4-byte multiple.
struct RtcData {
  uint32_t crc32;    // 4 bytes
  uint8_t channel;   // 1 byte,   5 in total
  uint8_t ap_mac[6]; // 6 bytes, 11 in total
  uint8_t padding;   // 1 byte,  12 in total
};

class WiFiManager {
public:
  void setup();
  void connect(const char *ssid, const char *password, const char *apName);
  void loop();

  String getWifiStatusText() {
    String wifiStatus = "";
    switch (WiFi.status()) {
    case WL_NO_SSID_AVAIL:
      wifiStatus = "SSID onbereikbaar: ";
      break;
    case WL_CONNECTED:
      wifiStatus = "SSID connected: ";
      break;
    case WL_CONNECT_FAILED:
      wifiStatus = "Connection failed: ";
      break;
    case WL_NO_SHIELD:
      wifiStatus = "No WiFi shield is present: ";
      break;
    case WL_IDLE_STATUS:
      wifiStatus = "WiFi connecting...: ";
      break;
    case WL_SCAN_COMPLETED:
      wifiStatus = "Scan networks is completed: ";
      break;
    case WL_CONNECTION_LOST:
      wifiStatus = "Connection lost: ";
      break;
    case WL_WRONG_PASSWORD:
      wifiStatus = "Wrong WiFi password: ";
      break;
    case WL_DISCONNECTED:
      wifiStatus = "WiFi disconnected: ";
      break;
    }
    return wifiStatus;
  }

protected:
  char mSSID[32];
  char mPassword[64];
  unsigned long mAPTimer = 0; // time we went into AP mode.
  WiFiState mWiFistate;

  void startConnection();
  RtcData loadRtcData();
  void saveRtcData();
  bool isRtcValid(RtcData rtcData);
  uint32_t calculateRTCDataCRC32(RtcData rtcData);
  void checkConnectionOrStartAP(const char* apName);
  bool setupAccessPoint(const char* apName);
  void onWifiConnect(const WiFiEventStationModeGotIP &event);
  void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
};
