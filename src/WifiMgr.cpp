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

#include "WifiMgr.h"

void WifiMgr::DoMe()
{
  if (WiFi.status() != LastStatusEvent)
  {
    wl_status_t tmp = LastStatusEvent;
    LastStatusEvent = WiFi.status();
    if (DelegateWifiChange != nullptr)
    { // Que si quelqu'un ecoute l'event
      DelegateWifiChange(tmp, LastStatusEvent);
    }

    if (LastStatusEvent == WL_CONNECTED && !WiFi.isConnected())
    {
      Reconnect();
    }
  }
}

/// @brief Traduit l'id de status en string
/// @param status
/// @return
String WifiMgr::StatusIdToString(wl_status_t status)
{
  switch (status)
  {
  case WL_NO_SHIELD:
    return "NO_SHIELD";
    break;
  case WL_IDLE_STATUS:
    return "IDLE_STATUS";
    break;
  case WL_NO_SSID_AVAIL:
    return "NO_SSID_AVAIL";
    break;
  case WL_SCAN_COMPLETED:
    return "SCAN_COMPLETED";
    break;
  case WL_CONNECTED:
    return "CONNECTED";
    break;
  case WL_CONNECT_FAILED:
    return "CONNECT_FAILED";
    break;
  case WL_CONNECTION_LOST:
    return "CONNECTION_LOST";
    break;
  case WL_WRONG_PASSWORD:
    return "WRONG_PASSWORD";
    break;
  case WL_DISCONNECTED:
    return "DISCONNECTED";
    break;
  default:
    return "UNKNOW";
    break;
  }
}

WifiMgr::WifiMgr(settings &currentConf) : conf(currentConf)
{
  LastStatusEvent = WL_IDLE_STATUS;
  WiFi.disconnect(true);
  WiFi.setAutoConnect(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  /*const uint32_t sleepTime = 15000; // sleep sleepTime millisecs

  WiFi.forceSleepBegin(sleepTime * 1000000L); // In uS. Must be same length as your delay
  delay(10);                                  // it doesn't always go to sleep unless you delay(10); yield() wasn't reliable
  delay(sleepTime);                           // Hang out at 15mA for (sleeptime) seconds
  WiFi.forceSleepWake();                      // Wifi on
  */
}

void WifiMgr::OnWifiEvent(void (*CallBack)(wl_status_t, wl_status_t))
{
  DelegateWifiChange = CallBack;
}

void WifiMgr::Connect()
{
  if (strcmp(conf.ssid, "") != 0)
  {
    // Wifi configured to connect to one wifi
    SendDebugPrintf("[WIFI] Trying to connect to '%s' wifi network", conf.ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(conf.ssid, conf.password);
    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(300);

      if (tries++ > 30)
      {
        SetAPMod();
        break;
      }
    }

    // if connected to wifi's user
    if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
    {
      WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
                                     {
        SendDebugPrintf("[WIFI] Perte de communication : %s", event.reason);
        Serial.println("Déconnexion du réseau");
        Serial.print("Raison : ");
        Serial.println(event.reason); });
      WiFi.setAutoReconnect(true);
      SendDebugPrintf("[WIFI] Running, IP : %s", WiFi.localIP().toString());
      setRFPower();
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  else
  {
    SetAPMod();
  }
}
void WifiMgr::Reconnect()
{
  SendDebugPrintf("[WIFI] Trying to Reconnect to '%s' wifi network", conf.ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(conf.ssid, conf.password);
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(500);
    if (tries++ > 30)
    {
      SendDebug("[WIFI][FATAL] Something is terribly wrong, can't connect to wifi (anymore).");
      digitalWrite(LED_BUILTIN, LOW);
      delay(60000);
      ESP.reset();
    }
  }
}

void WifiMgr::SetAPMod()
{
  SendDebugPrintf("[WIFI] Setting up Captive Portal by the name '%s'", SSID_SETUP);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID_SETUP, "");
  SendDebugPrintf("[WIFI] Captive Portal IP : %s", WiFi.softAPIP().toString());
  APtimer = millis();
}

/// @brief Si il diffuse son propre AP et non connecté a un Wifi
/// @return True si c'est en mode AP
bool WifiMgr::AsAP()
{
  return (WiFi.getMode() == WIFI_AP);
}

/// @brief
/// @return Return the quality (Received Signal Strength Indicator) of the WiFi network.
int WifiMgr::getQuality()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    return -1;
  }

  int dBm = WiFi.RSSI();
  if (dBm <= -100)
  {
    return 0;
  }
  if (dBm >= -50)
  {
    return 100;
  }

  return 2 * (dBm + 100);
}

void WifiMgr::setRFPower()
{
  float RFpower = roundf(20 - (getQuality() / 5)) + 2;
  if (RFpower >= 21)
  {
    RFpower = 20.5;
  }

  if (RFpower < 0)
  {
    RFpower = 1;
  }

  WiFi.setOutputPower(RFpower);
}