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
 */

/**
 * @file JSON.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 *
 * @brief This file contains the JSON functions for communicating with Domoticz 
 *
 * @see http://esp8266thingies.nl
 */
 
void doJSON(){
  UpdateElectricity();
  UpdateGas();
}

bool DomoticzJson(char* idx, int nValue, char* sValue){
  WiFiClient client;
  HTTPClient http;
  bool retVal = false;
  char url[255];
 
  if (config_data.domoticzIP[0] != '-') {
    sprintf(url, "http://%s:%s/json.htm?type=command&param=udevice&idx=%s&nvalue=%d&svalue=%s", config_data.domoticzIP, config_data.domoticzPort, idx, nValue, sValue);
    debugff("[HTTP] GET... URL: %s\n",url);
    http.begin(client, url); //HTTP
    int httpCode = http.GET();
        // httpCode will be negative on error
      if (httpCode > 0)
        { // HTTP header has been sent and Server response header has been handled
            debugff("[HTTP] GET... code: %d\n", httpCode);
             // file found at server
            if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                retVal = true;
                entitledToSleep = true;
            }
        } else
      {
          debugff("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    http.end();
    return retVal;
  } // we just return if there is no IP to report to.
  return true;
}

void UpdateGas(){  //sends over the gas setting to server(s)
  if(strncmp(gasReceived5min, prevGAS, sizeof(gasReceived5min)) != 0){          // if we have a new value, report
      DomoticzJson(config_data.domoticzGasIdx, 0, gasDomoticz);// gasReceived5min);
      debug("gas: ");
      debugln(gasDomoticz);
      strcpy(prevGAS, gasReceived5min);                              // retain current value for future reference
  }
}

void UpdateElectricity(){
  char sValue[255];
    //  sprintf(sValue, "%d.3;%d.3;%d.3;%d.3;%d;%d", electricityUsedTariff1, electricityUsedTariff2, mEOLT, mEOHT, mEAV, mEAT);
      sprintf(sValue, "%s;%s;%s;%s;%s;%s", electricityUsedTariff1, electricityUsedTariff2, electricityReturnedTariff1, electricityReturnedTariff2, actualElectricityPowerDelivered, actualElectricityPowerReturned);
      debugln(sValue);
      DomoticzJson(config_data.domoticzEnergyIdx, 0, sValue);
}
