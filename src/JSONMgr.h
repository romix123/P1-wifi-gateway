/*
 * Copyright (c) 2022 Ronald Leenes
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

#ifndef JSONMGR_H
#define JSONMGR_H

#include <Arduino.h>
#include "GlobalVar.h"
#include <ESP8266HTTPClient.h>
#include "P1Reader.h"

class JSONMgr
{
public:
  explicit JSONMgr(settings &configuration, P1Reader &currentP1);
  void DoMe();

private:
  settings &conf;
  P1Reader &P1Captor;

  char prevGAS[12] = {}; // not an P1 protocol var, but holds gas value
  /// @brief sends the gas usage to server
  void UpdateGas();
  /// @brief sends the electricity usage to server
  void UpdateElectricity();
  /// @brief Send to Domoticz data
  /// @param idx
  /// @param nValue
  /// @param sValue
  void DomoticzJson(unsigned int idx, int nValue, char *sValue);
};
#endif