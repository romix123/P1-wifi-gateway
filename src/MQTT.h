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
 * @file MQTT.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 *
 * @brief This file contains the MQTT functions
 *
 * @see http://esp8266thingies.nl
 */

/* MQTT part based on
 https://github.com/daniel-jong/esp8266_p1meter/blob/master/esp8266_p1meter/esp8266_p1meter.ino

*/

// **********************************
// * MQTT                           *
// **********************************

// void doMQTT(){
//   mqtt_connect();
//   if (MqttConnected) MQTT_reporter();
// }

// void mqtt_connect(){
//   if (!mqtt_client.connected()) {
//      MqttConnected = false;
//      Log.verboseln("Reconnecting to mqtt broker …");
//      mqtt_reconnect();
//   } else MqttConnected = true;
// }

// void mqtt_reconnect(){
//   if (millis() > nextMQTTreconnectAttempt) {
//     Log.verboseln("Attempting MQTT connection...");
//     statusMsg = "Attempting MQTT connection...";
//     // Create a random client ID
//     String clientId = "P1 Smart Meter – ";
//     clientId += String(random(0xffff), HEX);
//     // Attempt to connect
//     if (mqtt_client.connect(HOSTNAME, config_data.mqttUser,
//     config_data.mqttPass)){
//       Log.verboseln("   connected to broker");
//       statusMsg += "   connected to broker";
//       // Once connected, publish an announcement...
//       mqtt_client.publish("outTopic", "p1 gateway running");
//       // ... and resubscribe
//       mqtt_client.subscribe("inTopic");
//       MqttConnected = true;
//     } else {
//       Log.verbose("failed, rc=");
//       statusMsg += "failed, rc=";
//       Log.verbose(mqtt_client.state());
//       Log.verboseln(" trying again later (non blocking)");
//       statusMsg += " trying again later (non blocking)";
//       nextMQTTreconnectAttempt = millis() + 15000; // try again in 15 seconds
//       MqttConnected = false;
//     }
//   }
// }

void doMQTT() {
  bool response = mqtt_connect();
  if (response)
    MQTT_reporter();
}

bool mqtt_connect() {
  mqtt_client.setCallback(callback);
  byte i = 0;
  while (!mqtt_client.connected() && (i < 3)) {
    Log.verboseln("Attempting MQTT connection");
    // Attempt to connect
    boolean ret;

    ret = mqtt_client.connect(HOSTNAME, config_data.mqttUser,
                              config_data.mqttPass);
    if (ret) {
      Log.verboseln("Connected to MQTT");
      MqttConnected = true;
      mqtt_client.publish("outTopic", "P1 gateway");
      mqtt_client.subscribe("inTopic");
      return true;
    } else {
      MqttConnected = false;
      Log.verbose("Failed MQTT connection, return code:");

      int Status = mqtt_client.state();

      switch (Status) {
      case -4:
        Log.verboseln("Connection timeout");
        break;

      case -3:
        Log.verboseln("Connection lost");
        break;

      case -2:
        Log.verboseln("Connect failed");
        break;

      case -1:
        Log.verboseln("Disconnected");
        break;

      case 1:
        Log.verboseln("Bad protocol");
        break;

      case 2:
        Log.verboseln("Bad client ID");
        break;

      case 3:
        Log.verboseln("Unavailable");
        break;

      case 4:
        Log.verboseln("Bad credentials");
        break;

      case 5:
        Log.verboseln("Unauthorized");
        break;
      }
      if (i == 2)
        Log.verboseln("Retrying MQTT connection next cycle");
      i++;
      delay(1000);
    }
  }
  return false;
}

void callback(char *topic, byte *payload, unsigned int length) {
  Log.verbose("Message arrived [");
  Log.verbose(topic);
  Log.verbose("] ");
  for (unsigned int i = 0; i < length; i++) {
    Log.verbose("%s", (char)payload[i]);
  }
  Log.verboseln("");
}

// * Send a message to a broker topic
void send_mqtt_message(const char *topic, char *payload) {
  if (payload[0] == 0)
    return; // nothing to report
  Log.verbose("MQTT Outgoing on > ");
  Log.verbose(topic);
  Log.verbose(" > ");
  Log.verboseln(payload);
  bool result = mqtt_client.publish(topic, payload, false);

  if (!result) {
    Log.verbose("MQTT publish to topic: ");
    Log.verbose(topic);
    Log.verboseln(" failed.");
  }
}

void send_metric(String name, float metric) // added *long
{
  char value[20];
  dtostrf(metric, 3, 3, value);
  //  output ftoa(metric, output, 3);

  mtopic = String(config_data.mqttTopic) + "/" + name;
  send_mqtt_message(mtopic.c_str(), value); // output
}

void mqtt_send_metric(String name, char *metric) {
  mtopic = String(config_data.mqttTopic) + "/" + name;
  send_mqtt_message(mtopic.c_str(), metric);
}

void MQTT_reporter() {
#ifdef NEDERLANDS
  Log.verbose("MQTT reporter: ");
  Log.verboseln(actualElectricityPowerRet);
  if (mqtt_dsmr) {
    mqtt_send_metric("equipmentID", equipmentId);

    mqtt_send_metric("reading/electricity_delivered_1", electricityUsedTariff1);
    mqtt_send_metric("reading/electricity_delivered_2", electricityUsedTariff2);
    mqtt_send_metric("reading/electricity_returned_1",
                     electricityReturnedTariff1);
    mqtt_send_metric("reading/electricity_returned_2",
                     electricityReturnedTariff2);
    mqtt_send_metric("reading/electricity_currently_delivered",
                     actualElectricityPowerDeli);
    mqtt_send_metric("reading/electricity_currently_returned",
                     actualElectricityPowerRet);

    mqtt_send_metric("reading/phase_currently_delivered_l1", activePowerL1P);
    mqtt_send_metric("reading/phase_currently_delivered_l2", activePowerL2P);
    mqtt_send_metric("reading/phase_currently_delivered_l3", activePowerL3P);
    mqtt_send_metric("reading/phase_currently_returned_l1", activePowerL1NP);
    mqtt_send_metric("reading/phase_currently_returned_l2", activePowerL2NP);
    mqtt_send_metric("reading/phase_currently_returned_l3", activePowerL3NP);
    mqtt_send_metric("reading/phase_voltage_l1", instantaneousVoltageL1);
    mqtt_send_metric("reading/phase_voltage_l2", instantaneousVoltageL2);
    mqtt_send_metric("reading/phase_voltage_l3", instantaneousVoltageL3);

    mqtt_send_metric("consumption/gas/delivered", gasReceived5min);

    //     mqtt_send_metric("meter-stats/actual_tarif_group",
    //     tariffIndicatorElectricity[3]);
    send_metric("meter-stats/actual_tarif_group",
                tariffIndicatorElectricity[3]);
    mqtt_send_metric("meter-stats/power_failure_count", numberPowerFailuresAny);
    mqtt_send_metric("meter-stats/long_power_failure_count",
                     numberLongPowerFailuresAny);
    mqtt_send_metric("meter-stats/short_power_drops", numberVoltageSagsL1);
    mqtt_send_metric("meter-stats/short_power_peaks", numberVoltageSwellsL1);

    send_metric("day-consumption/electricity1",
                (atof(electricityUsedTariff1) - atof(log_data.dayE1)));
    send_metric("day-consumption/electricity2",
                (atof(electricityUsedTariff2) - atof(log_data.dayE2)));
    send_metric("day-consumption/electricity1_returned",
                (atof(electricityReturnedTariff1) - atof(log_data.dayR1)));
    send_metric("day-consumption/electricity2_returned",
                (atof(electricityReturnedTariff2) - atof(log_data.dayR2)));

    send_metric("day-consumption/electricity_merged",
                ((atof(electricityUsedTariff1) - atof(log_data.dayE1)) +
                 (atof(electricityUsedTariff2) - atof(log_data.dayE2))));
    send_metric("day-consumption/electricity_returned_merged",
                ((atof(electricityReturnedTariff1) - atof(log_data.dayR1)) +
                 (atof(electricityReturnedTariff2) - atof(log_data.dayR2))));
    send_metric("day-consumption/gas",
                (atof(gasReceived5min) - atof(log_data.dayG)));

    send_metric("current-month/electricity1",
                (atof(electricityUsedTariff1) - atof(log_data.monthE1)));
    send_metric("current-month/electricity2",
                (atof(electricityUsedTariff2) - atof(log_data.monthE2)));
    send_metric("current-month/electricity1_returned",
                (atof(electricityReturnedTariff1) - atof(log_data.monthR1)));
    send_metric("current-month/electricity2_returned",
                (atof(electricityReturnedTariff2) - atof(log_data.monthR2)));

    send_metric("current-month/electricity_merged",
                ((atof(electricityUsedTariff1) - atof(log_data.monthE1)) +
                 (atof(electricityUsedTariff2) - atof(log_data.monthE2))));
    send_metric("current-month/electricity_returned_merged",
                ((atof(electricityReturnedTariff1) - atof(log_data.monthR1)) +
                 (atof(electricityReturnedTariff2) - atof(log_data.monthR2))));
    send_metric("current-month/gas",
                (atof(gasReceived5min) - atof(log_data.monthG)));
  } else {
    mqtt_send_metric("equipmentID", equipmentId);

    mqtt_send_metric("consumption_low_tarif", electricityUsedTariff1);
    mqtt_send_metric("consumption_high_tarif", electricityUsedTariff2);
    mqtt_send_metric("returndelivery_low_tarif", electricityReturnedTariff1);
    mqtt_send_metric("returndelivery_high_tarif", electricityReturnedTariff2);
    mqtt_send_metric("actual_consumption", actualElectricityPowerDeli);
    mqtt_send_metric("actual_returndelivery", actualElectricityPowerRet);

    mqtt_send_metric("l1_instant_power_usage", activePowerL1P);
    mqtt_send_metric("l2_instant_power_usage", activePowerL2P);
    mqtt_send_metric("l3_instant_power_usage", activePowerL3P);
    mqtt_send_metric("l1_instant_power_current", instantaneousCurrentL1);
    mqtt_send_metric("l2_instant_power_current", instantaneousCurrentL2);
    mqtt_send_metric("l3_instant_power_current", instantaneousCurrentL3);
    mqtt_send_metric("l1_voltage", instantaneousVoltageL1);
    mqtt_send_metric("l2_voltage", instantaneousVoltageL2);
    mqtt_send_metric("l3_voltage", instantaneousVoltageL3);
    mqtt_send_metric("l1_instant_power_return", activePowerL1NP);
    mqtt_send_metric("l2_instant_power_return", activePowerL2NP);
    mqtt_send_metric("l3_instant_power_return", activePowerL3NP);

    mqtt_send_metric("gas_meter_m3", gasReceived5min);

    send_metric("actual_tarif_group", tariffIndicatorElectricity[3]);
    mqtt_send_metric("short_power_outages", numberPowerFailuresAny);
    mqtt_send_metric("long_power_outages", numberLongPowerFailuresAny);
    mqtt_send_metric("short_power_drops", numberVoltageSagsL1);
    mqtt_send_metric("short_power_peaks", numberVoltageSwellsL1);
    LastReportinMillis = millis();
    LastMReport = timestampkaal();
    MqttDelivered = true;
    return;
  }
#endif
  // #ifdef GERMAN
  //       mqtt_send_metric("equipmentID", equipmentId);

  //       mqtt_send_metric("consumption_low_tarif", electricityUsedTariff1);
  //       mqtt_send_metric("consumption_high_tarif", electricityUsedTariff2);
  //       mqtt_send_metric("returndelivery_low_tarif",
  //       electricityReturnedTariff1);
  //       mqtt_send_metric("returndelivery_high_tarif",
  //       electricityReturnedTariff2); mqtt_send_metric("actual_consumption",
  //       actualElectricityPowerDeli);
  //       mqtt_send_metric("actual_returndelivery", actualElectricityPowerRet);

  //       mqtt_send_metric("l1_instant_power_usage", activePowerL1P);
  //       mqtt_send_metric("l2_instant_power_usage", activePowerL2P);
  //       mqtt_send_metric("l3_instant_power_usage", activePowerL3P);
  //       mqtt_send_metric("l1_instant_power_current", instantaneousCurrentL1);
  //       mqtt_send_metric("l2_instant_power_current", instantaneousCurrentL2);
  //       mqtt_send_metric("l3_instant_power_current", instantaneousCurrentL3);
  //       mqtt_send_metric("l1_voltage", instantaneousVoltageL1);
  //       mqtt_send_metric("l2_voltage", instantaneousVoltageL2);
  //       mqtt_send_metric("l3_voltage", instantaneousVoltageL3);

  //       mqtt_send_metric("gas_meter_m3", gasReceived5min);

  //       mqtt_send_metric("actual_tarif_group", tariffIndicatorElectricity);
  //       mqtt_send_metric("short_power_outages", numberPowerFailuresAny);
  //       mqtt_send_metric("long_power_outages", numberLongPowerFailuresAny);
  //       mqtt_send_metric("short_power_drops", numberVoltageSagsL1);
  //       mqtt_send_metric("short_power_peaks", numberVoltageSwellsL1);
  //       LastReportinMillis = millis();
  // #endif
  // #ifdef SWEDISH
  // /*
  // 1-0:1.8.0 Mätarställning Aktiv Energi Uttag.
  // 1-0:2.8.0 Mätarställning Aktiv Energi Inmatning
  // 1-0:3.8.0 Mätarställning Reaktiv Energi Uttag
  // 1-0:4.8.0 Mätarställning Reaktiv Energi Inmatning

  // 1-0:1.7.0 Aktiv Effekt Uttag  Momentan trefaseffekt
  // 1-0:2.7.0 Aktiv Effekt Inmatning  Momentan trefaseffekt
  // 1-0:3.7.0 Reaktiv Effekt Uttag  Momentan trefaseffekt
  // 1-0:4.7.0 Reaktiv Effekt Inmatning  Momentan trefaseffekt
  // 1-0:21.7.0  L1 Aktiv Effekt Uttag Momentan effekt
  // 1-0:22.7.0  L1 Aktiv Effekt Inmatning Momentan effekt
  // 1-0:41.7.0  L2 Aktiv Effekt Uttag Momentan effekt
  // 1-0:42.7.0  L2 Aktiv Effekt Inmatning Momentan effekt
  // 1-0:61.7.0  L3 Aktiv Effekt Uttag Momentan effekt
  // 1-0:62.7.0  L3 Aktiv Effekt Inmatning Momentan effekt
  // 1-0:23.7.0  L1 Reaktiv Effekt Uttag Momentan effekt
  // 1-0:24.7.0  L1 Reaktiv Effekt Inmatning Momentan effekt
  // 1-0:43.7.0  L2 Reaktiv Effekt Uttag Momentan effekt
  // 1-0:44.7.0  L2 Reaktiv Effekt Inmatning Momentan effekt
  // 1-0:63.7.0  L3 Reaktiv Effekt Uttag Momentan effekt
  // 1-0:64.7.0  L3 Reaktiv Effekt Inmatning Momentan effekt
  // 1-0:32.7.0  L1 Fasspänning  Momentant RMS-värde
  // 1-0:52.7.0  L2 Fasspänning  Momentant RMS-värde
  // 1-0:72.7.0  L3 Fasspänning  Momentant RMS-värde
  // 1-0:31.7.0  L1 Fasström Momentant RMS-värde
  // 1-0:51.7.0  L2 Fasström Momentant RMS-värde
  // 1-0:71.7.0  L3 Fasström Momentant RMS-värde
  // */
  // /*
  //  * https://github.com/forsberg/esphome-p1reader
  //  */
  //       mqtt_send_metric("cumulativeActiveImport", cumulativeActiveImport);
  //       // 1.8.0 mqtt_send_metric("cumulativeActiveExport",
  //       cumulativeActiveExport);       // 2.8.0
  //       mqtt_send_metric("cumulativeReactiveImport",
  //       cumulativeReactiveImport);   // 3.8.0
  //       mqtt_send_metric("cumulativeReactiveExport",
  //       cumulativeReactiveExport);   // 4.8.0

  //       mqtt_send_metric("momentaryActiveImport", momentaryActiveImport);
  //       // 1.7.0 mqtt_send_metric("momentaryActiveExport",
  //       momentaryActiveExport);         // 2.7.0
  //       mqtt_send_metric("momentaryReactiveImport", momentaryReactiveImport);
  //       // 3.7.0 mqtt_send_metric("momentaryReactiveExport",
  //       momentaryReactiveExport);     // 4.7.0

  //       mqtt_send_metric("momentaryActiveImportL1", activePowerL1P);
  //       // 21.7.0 mqtt_send_metric("momentaryActiveExportL1",
  //       reactivePowerL1NP);           // 22.7.0

  //       mqtt_send_metric("momentaryActiveImportL2", activePowerL2P);
  //       // 41.7.0 mqtt_send_metric("momentaryActiveExportL2",
  //       reactivePowerL2NP);           // 42.7.0

  //       mqtt_send_metric("momentaryActiveImportL3", activePowerL3P);
  //       // 61.7.0 mqtt_send_metric("momentaryActiveExportL3",
  //       reactivePowerL3NP);           // 62.7.0

  //       mqtt_send_metric("momentaryReactiveImportL1",
  //       momentaryReactiveImportL1);   // 23.7.0
  //       mqtt_send_metric("momentaryReactiveImportL1",
  //       momentaryReactiveExportL1);   // 24.7.0

  //       mqtt_send_metric("momentaryReactiveImportL2",
  //       momentaryReactiveImportL2);   // 43.7.0
  //       mqtt_send_metric("momentaryReactiveExportL2",
  //       momentaryReactiveExportL2);   // 44.7.0

  //       mqtt_send_metric("momentaryReactiveImportL3",
  //       momentaryReactiveImportL3);   // 63.7.0
  //       mqtt_send_metric("momentaryReactiveExportL1",
  //       momentaryReactiveExportL3);   // 64.7.0

  //       mqtt_send_metric("voltageL1", instantaneousVoltageL1);  // 32.7.0
  //       mqtt_send_metric("voltageL2", instantaneousVoltageL2);  // 52.7.0
  //       mqtt_send_metric("voltageL3", instantaneousVoltageL3);  // 72.7.0

  //       mqtt_send_metric("currentL1", instantaneousCurrentL1);  // 31.7.0
  //       mqtt_send_metric("currentL2", instantaneousCurrentL2);  // 51.7.0
  //       mqtt_send_metric("currentL3", instantaneousCurrentL3);  // 71.7.0
  // //      mqtt_send_metric("P1module_voltage", outstr);
  // #endif
  LastMReport = timestampkaal();
  MqttDelivered = true;
  LastReportinMillis = millis();
}

void MQTT_Debug() {
  char outstr[10];

  dtostrf(millis(), 10, 0, outstr);
  send_mqtt_message("p1wifi/P1now", outstr);

  dtostrf(nextUpdateTime, 10, 0, outstr);
  send_mqtt_message("p1wifi/P1next", outstr);

  LastMReport.toCharArray(outstr, 10);
  send_mqtt_message("p1wifi/P1time_LastMReport", outstr);

  LastTReport.toCharArray(outstr, 10);
  send_mqtt_message("p1wifi/P1time_LastTReport", outstr);

  LastJReport.toCharArray(outstr, 10);
  send_mqtt_message("p1wifi/P1time_LastJReport", outstr);

  //     dtostrf(time_to_wake, 10,0, outstr);
  //     send_mqtt_message("p1wifi/P1time_toWake", outstr);

  //    dtostrf(ESP.getFreeHeap(), 10,0, outstr);
  //     send_mqtt_message("p1wifi/P1Heap", outstr);

  //     dtostrf(ESP.getHeapFragmentation(), 10,0, outstr);
  //     send_mqtt_message("p1wifi/P1HeapFrag", outstr);

  //     dtostrf(WiFi.RSSI(), 10,0, outstr);
  //     send_mqtt_message("p1wifi/RSSI", outstr);

  dtostrf(RFpower, 10, 0, outstr);
  send_mqtt_message("p1wifi/RF power", outstr);
}
