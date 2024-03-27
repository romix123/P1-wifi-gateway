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
  Log.verbose("The internal VCC reads %1.2f volts\n", volts / 1000);
}

void PrintConfigData() {
  Log.verbose("dataSet : ");
  Log.verboseln(config_data.dataSet);
  Log.verbose("Admin password : ");
  Log.verboseln(config_data.adminPassword);
  Log.verbose("SSID : ");
  Log.verboseln(config_data.ssid);
  Log.verbose("password : ");
  Log.verboseln(config_data.password);
  Log.verbose("DomoticxIP : ");
  Log.verboseln(config_data.domoticzIP);
  Log.verbose("DomoticzPort : ");
  Log.verboseln(config_data.domoticzPort);
  Log.verbose("domotixzGasIdx : ");
  Log.verboseln(config_data.domoticzGasIdx);
  Log.verbose("domotixzEnergyIdx : ");
  Log.verboseln(config_data.domoticzEnergyIdx);

  Log.verbose("mqttIP : ");
  Log.verboseln(config_data.mqttIP);
  Log.verbose("mqttPort : ");
  Log.verboseln(config_data.mqttPort);
  Log.verbose("mqttUser : ");
  Log.verboseln(config_data.mqttUser);
  Log.verbose("mqttPass : ");
  Log.verboseln(config_data.mqttPass);
  Log.verbose("mqttTopic : ");
  Log.verboseln(config_data.mqttTopic);

  Log.verbose("interval : ");
  Log.verboseln(config_data.interval);
  Log.verbose("json : ");
  Log.verboseln(config_data.domo);
  Log.verbose("mqtt : ");
  Log.verboseln(config_data.mqtt);
  Log.verbose("watt : ");
  Log.verboseln(config_data.watt);
  Log.verbose("telnet : ");
  Log.verboseln(config_data.telnet);
}
