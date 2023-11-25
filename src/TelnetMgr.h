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

#ifndef TelnetMgr_H
#define TelnetMgr_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Debug.h"
#include "GlobalVar.h"

#define MAX_SRV_CLIENTS 5
#define STACK_PROTECTOR 1512  // bytes
#define TELNETPORT 23
#define TELNET_REPPORT_INTERVAL_SEC 10

class TelnetMgr
{
  private:
  settings& conf;
  WiFiServer telnet;
  WiFiClient telnetClients[MAX_SRV_CLIENTS];
  unsigned long NextReportTime = millis();
  public:
  explicit TelnetMgr(settings& currentConf);
  void DoMe(String Diagram);
  void TelnetReporter(String Diagram);
  void SendDebug(String payload);
};
#endif