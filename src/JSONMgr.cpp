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


#include "JSONMgr.h"

void JSONMgr::UpdateGas()
{
  // if we have a new value, report
  if(strncmp(P1Captor.DataReaded.gasReceived5min, prevGAS, sizeof(P1Captor.DataReaded.gasReceived5min)) != 0)
  {
    SendDebugPrintf("New value for gas : %s", P1Captor.DataReaded.gasDomoticz);
    DomoticzJson(conf.domoticzGasIdx, 0, P1Captor.DataReaded.gasDomoticz); // gasReceived5min);
    strcpy(prevGAS, P1Captor.DataReaded.gasReceived5min); // retain current value for future reference
  }
}

/// @brief sends the electricity usage to server
void JSONMgr::UpdateElectricity()
{
  char sValue[300];
  SendDebugPrintf("New value for Energy : %s", P1Captor.DataReaded.actualElectricityPowerRet);
  sprintf(sValue, "%s;%s;%s;%s;%s;%s", P1Captor.DataReaded.electricityUsedTariff1, P1Captor.DataReaded.electricityUsedTariff2, P1Captor.DataReaded.electricityReturnedTariff1, P1Captor.DataReaded.electricityReturnedTariff2, P1Captor.DataReaded.actualElectricityPowerDeli, P1Captor.DataReaded.actualElectricityPowerRet);
  DomoticzJson(conf.domoticzEnergyIdx, 0, sValue);
}

/// @brief Send to Domoticz data
/// @param idx 
/// @param nValue 
/// @param sValue 
void JSONMgr::DomoticzJson(unsigned int idx, int nValue, char* sValue)
{
  WiFiClient client;
  HTTPClient http;
  
  if (conf.domo)
  {
    char url[255];
    sprintf(url, "http://%s:%u/json.htm?type=command&param=udevice&idx=%u&nvalue=%d&svalue=%s", conf.domoticzIP, conf.domoticzPort, idx, nValue, sValue);
    SendDebugPrintf("[HTTP] Send GET : %s", url);
    
    http.begin(client, url);
    int httpCode = http.GET();
    
    // httpCode will be negative on error
    if (httpCode > 0)
    { // HTTP header has been sent and Server response header has been handled
      SendDebugPrintf("[HTTP] Send GET return : %d", httpCode);

      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString();
      }
    }
    else
    {
      SendDebugPrintf("[HTTP] GET failed, error: %s", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

JSONMgr::JSONMgr(settings &configuration, P1Reader &currentP1) : conf(configuration), P1Captor(currentP1)
{
}

void JSONMgr::DoMe()
{
  UpdateElectricity();
  UpdateGas();
}