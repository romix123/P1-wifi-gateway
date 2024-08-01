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

void doJSON() {
  Log.verboseln("doJSON>>>");
  UpdateElectricity();
  UpdateGas();
}

bool DomoticzJson(char *idx, int nValue, char *sValue) {
  WiFiClient client;
  HTTPClient http;
  bool retVal = false;
  char url[255];
  if (config_data.domoticzIP[0] != '-') {
    sprintf(url,
            "http://%s:%s/"
            "json.htm?type=command&param=udevice&idx=%s&nvalue=%d&svalue=%s",
            config_data.domoticzIP, config_data.domoticzPort, idx, nValue,
            sValue);
    // sprintf(debugM, "[HTTP] GET... URL: %s\n", url);
    // Log.verbose(">>JSON: ");
    // Log.verboseln(url);
    http.begin(client, url); // HTTP
    delay(200);
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) { // HTTP header has been sent and Server response header
                        // has been handled
                        // Log.verbose("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        Log.verboseln("return httpCode == HTTP_CODE_OK");
        // String payload = http.getString();
        retVal = true;
        LastJReport = timestampkaal();
      }
    } else {
      Log.verboseln("return httpCode == ERR");
    }
    http.end();
    return retVal;
  } // we just return if there is no IP to report to.
  return true;
}

void UpdateGas() {
  // sends over the gas setting to server(s)
  // if we have a new value, report
  if (!dsmrData.gas_delivered.equals(prevGAS)) {
    DomoticzJson(config_data.domoticzGasIdx, 0,
                 (char *)dsmrData.gas_delivered.c_str());
    Log.verbose(">> UpdateGas(): ");
    Log.verboseln(dsmrData.gas_delivered.c_str());
    // retain current value for future reference
    prevGAS = dsmrData.gas_delivered;
  }
}

void UpdateElectricity() {
  char sValue[300];
  snprintf(sValue, 300, "%.3f;%.3f;%.3f;%.3f;%.3f;%.3f",
          dsmrData.energy_delivered_tariff1.val(),
          dsmrData.energy_delivered_tariff2.val(),
          dsmrData.energy_returned_tariff1.val(),
          dsmrData.energy_returned_tariff2.val(),
          dsmrData.power_delivered.val(), dsmrData.power_returned.val());
  Log.verbose(">> UpdateElectricity(): ");
  Log.verboseln(sValue);
  DomoticzJson(config_data.domoticzEnergyIdx, 0, sValue);
}
