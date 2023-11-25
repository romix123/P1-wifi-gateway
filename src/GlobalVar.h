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

#ifndef VARMGR_H
#define VARMGR_H

#include <Arduino.h>

#define VERSION "0.95"
#define HOSTNAME "p1meter"

#define OE 16 // IO16 OE on the 74AHCT1G125
#define DR 4  // IO4 is Data Request

#define SETTINGVERSIONNULL 0 //= no config
#define SETTINGVERSION 1

struct settings
{
  byte ConfigVersion;
  bool NeedConfig = true;
  char ssid[33];
  char password[65];
  char domoticzIP[30];
  unsigned int domoticzPort;
  unsigned int domoticzEnergyIdx;
  unsigned int domoticzGasIdx;
  char mqttTopic[50];
  char mqttIP[30];
  unsigned int mqttPort;
  char mqttUser[32];
  char mqttPass[32];
  unsigned int interval;
  bool domo = true;
  bool mqtt = false;
  bool watt = false;
  bool telnet = false;
  bool debugToMqtt = false;
  bool debugToTelnet = false;
  char adminPassword[33];
  char adminUser[33];
};

// Swedish specific
#ifdef SWEDISH
char cumulativeActiveImport[12];    // 1.8.0
char cumulativeActiveExport[12];    // 2.8.0
char cumulativeReactiveImport[12];  // 3.8.0
char cumulativeReactiveExport[12];  // 4.8.0
char momentaryActiveImport[12];     // 1.7.0
char momentaryActiveExport[12];     // 2.7.0
char momentaryReactiveImport[12];   // 3.7.0
char momentaryReactiveExport[12];   // 4.7.0
char momentaryReactiveImportL1[12]; // 23.7.0
char momentaryReactiveImportL2[12]; // 43.7.0
char momentaryReactiveImportL3[12]; // 63.7.0
char momentaryReactiveExportL1[12]; // 24.7.0
char momentaryReactiveExportL2[12]; // 44.7.0
char momentaryReactiveExportL3[12]; // 64.7.0

char reactivePowerL1P[9]; // Sweden uses these 6
char reactivePowerL2P[9];
char reactivePowerL3P[9];
char reactivePowerL1NP[9];
char reactivePowerL2NP[9];
char reactivePowerL3NP[9];
#endif
#endif