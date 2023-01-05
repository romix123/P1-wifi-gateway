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
 * @file debug.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 *
 * @brief This file contains debug functions 
 *
 * @see http://esp8266thingies.nl
 */
 
void readVoltage() { // read internal VCC
  volts = ESP.getVcc();
  debugf("The internal VCC reads %1.2f volts\n", volts / 1000);
}

void PrintConfigData(){
  debug("dataSet : ");
    debugln(user_data.dataSet);
  debug("Admin password : ");
    debugln(user_data.adminPassword);
  debug("SSID : ");
    debugln(user_data.ssid);
  debug("password : ");
    debugln(user_data.password);
  debug("DomoticxIP : ");
    debugln(user_data.domoticzIP);
  debug("DomoticzPort : ");
    debugln(user_data.domoticzPort);
  debug("domotixzGasIdx : ");
   debugln(user_data.domoticzGasIdx);
  debug("domotixzEnergyIdx : ");
   debugln(user_data.domoticzEnergyIdx);

  debug("mqttIP : ");
    debugln(user_data.mqttIP);
  debug("mqttPort : ");
    debugln(user_data.mqttPort);
  debug("mqttUser : ");
    debugln(user_data.mqttUser);
  debug("mqttPass : ");
    debugln(user_data.mqttPass);
  debug("mqttTopic : ");
    debugln(user_data.mqttTopic);

  debug("interval : ");
    debugln(user_data.interval);
  debug("json : ");
    debugln(user_data.domo);
  debug("mqtt : ");
    debugln(user_data.mqtt);
  debug("watt : ");
      debugln(user_data.watt);
  debug("telnet : ");
      debugln(user_data.telnet);
}
