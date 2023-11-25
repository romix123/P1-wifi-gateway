/*
 * Copyright (c) 2023 Jean-Pierre sneyers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Additionally, please note that the original source code of this file
 * may contain portions of code derived from (or inspired by)
 * previous works by:
 *
 * Ronald Leenes (https://github.com/romix123/P1-wifi-gateway and http://esp8266thingies.nl)
 */

#ifndef WIFIMGR
#define WIFIMGR

#define SSID_SETUP "P1_setup"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "GlobalVar.h"

// van ESP8266WiFi/examples/WiFiShutdown/WiFiShutdown.ino
#ifndef RTC_config_data_SLOT_WIFI_STATE
#define RTC_config_data_SLOT_WIFI_STATE 33u
#endif

/// @brief everything related to WIFI
class WifiMgr
{
  private:
  settings& conf;
  void (*DelegateWifiChange)(wl_status_t, wl_status_t) = nullptr;
  wl_status_t LastStatusEvent;
  void SetAPMod();

  public:
  explicit WifiMgr(settings& currentConf);
  unsigned long APtimer = 0; // time we went into AP mode. Restart module if in AP for 10 mins as we might have gotten in AP due to router wifi issue
  WiFiClient WifiCom;
  void DoMe();
  String StatusIdToString(wl_status_t status);
  void OnWifiEvent(void (*CallBack)(wl_status_t, wl_status_t));
  void Connect();

  /// @brief Si il diffuse son propre AP et non connecté a un Wifi
  /// @return True si c'est en mode AP
  bool AsAP();

  /// @brief
  /// @return Return the quality (Received Signal Strength Indicator) of the WiFi network.
  int getQuality();
  void setRFPower();
  void Reconnect();
};
#endif