#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <include/WiFiState.h> // WiFiState structure details

#include "CRC32.h"
#include "WifiManager.h"
#include "led.h"

void WiFiManager::setup() {
  Log.traceln("WiFiManager::setup");
  // https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html
  // Starting from version 3 of this core, persistence is disabled by default
  //  and WiFi does not start automatically at boot (see PR #7902).
}

void WiFiManager::connect(const char *ssid, const char *password, const char *apName) {
  // Save the ssid and password
  strncpy(mSSID, ssid, sizeof mSSID);
  strncpy(mPassword, password, sizeof mPassword);

  startConnection();

  checkConnectionOrStartAP(apName);
  // WiFi.printDiag(Serial);

  if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED) {
    saveRtcData();
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  } else if (WiFi.getMode() != WIFI_AP) {
    Log.errorln("Wifi in unknown state %s", getWifiStatusText());
  }
}

void WiFiManager::loop() {
  // Check we have been in AP mode for 600 sec.
  // Restart module if in AP for 10 mins as we
  // might have gotten in AP due to router wifi issue
  if (WiFi.getMode() == WIFI_AP && (millis() - mAPTimer > 600000)) {
    ESP.reset();
  }
}

void WiFiManager::startConnection() {
  Log.verboseln("Connecting to WiFi \"%s\" ", mSSID);
  WiFi.mode(WIFI_STA); // Station mode

  //-----------Now we replace the normally used "WiFi.begin();"
  // with a procedure using connection data stored by us
  RtcData rtcData = loadRtcData();
  if (isRtcValid(rtcData)) {
    // The RTC data was good, make a quick connection
    WiFi.begin(mSSID, mPassword, rtcData.channel, rtcData.ap_mac, true);
    Log.verboseln("RTC wifi quick start.");
  } else {
    // The RTC data was not valid, so make a regular connection
    WiFi.begin(mSSID, mPassword);
    Log.verboseln("Regular wifi start.");
  }
}

RtcData WiFiManager::loadRtcData() {
  // Try to read WiFi settings from RTC memory
  RtcData rtcData;
  ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(rtcData));
  return rtcData;
}

void WiFiManager::saveRtcData() {
  RtcData rtcData;
  //-----
  // Write current connection info back to RTC
  rtcData.channel = WiFi.channel();
  // Copy 6 bytes of BSSID (AP's MAC address)
  memcpy(rtcData.ap_mac, WiFi.BSSID(), 6);
  rtcData.crc32 = calculateRTCDataCRC32(rtcData);
  ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData));
}

bool WiFiManager::isRtcValid(RtcData rtcData) {
  return calculateRTCDataCRC32(rtcData) == rtcData.crc32;
}

uint32_t WiFiManager::calculateRTCDataCRC32(RtcData rtcData) {
  // Calculate the CRC of what we just read from RTC memory, but skip the
  // first 4 bytes as that's the checksum itself.
  return calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
}

void WiFiManager::checkConnectionOrStartAP(const char* apName) {
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    ToggleLED;
    delay(300);
    Log.verbose(".");
    if (tries++ > 30) {
      Log.verboseln("");
      setupAccessPoint(apName);
      break;
    }
  }
  LEDoff;
  Log.verboseln("");
}

bool WiFiManager::setupAccessPoint(const char* apName) {
  Log.verboseln("Setting up Captive Portal by the name '%s'", apName);
  LEDon;
  WiFi.mode(WIFI_AP);
  Log.verboseln("Before activating AccessPoint");
  bool connected = WiFi.softAP(apName, "");
  Log.verboseln("After activating AccessPoint");
  if (connected) {
    mAPTimer = millis();
  }
  return connected;
}
