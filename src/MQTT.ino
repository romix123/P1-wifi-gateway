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

void doMQTT(){
  mqtt_connect();
  if (MqttConnected) MQTT_reporter();
}

void mqtt_connect(){
  if (!mqtt_client.connected()) {
     MqttConnected = false;
     debugln("Reconnecting to mqtt broker …");
     mqtt_reconnect();
  } else MqttConnected = true;
}

void mqtt_reconnect(){
  
  if (millis() > nextMQTTreconnectAttempt) {
    debugln("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "P1 Smart Meter – ";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(HOSTNAME, config_data.mqttUser, config_data.mqttPass)){
      debugln("   connected to broker");
      // Once connected, publish an announcement...
      mqtt_client.publish("outTopic", "p1 gateway running");
      // ... and resubscribe
      mqtt_client.subscribe("inTopic");
      MqttConnected = true;
    } else {
      debug("failed, rc=");
      debug(mqtt_client.state());
      debugln(" try again later (non blocking)");
      nextMQTTreconnectAttempt = millis() + 2000; // try again in 2 seconds
      MqttConnected = false;
    }
  } 
}


void callback(char* topic, byte* payload, unsigned int length) {
  debug("Message arrived [");
  debug(topic);
  debug("] ");
  for (int i = 0; i < length; i++) {
    debug((char)payload[i]);
  }
  debugln();
}

// * Send a message to a broker topic
void send_mqtt_message(const char *topic, char *payload)
{
    if (payload[0] == 0) return; //nothing to report
    debug("MQTT Outgoing on > ");
    debug(topic);
    debug(" > ");
    debugln(payload);

    bool result = mqtt_client.publish(topic, payload, false);

    if (!result)
    {
        debug("MQTT publish to topic: ");
        debug(topic);
        debugln(" failed.");
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

void mqtt_send_metric(String name, char *metric)  
{
    char output[100];
    mtopic = String(config_data.mqttTopic) + "/" + name;
    send_mqtt_message(mtopic.c_str(), metric);
}


 void MQTT_reporter(){
#ifdef NEDERLANDS
  if (mqtt_dsmr){
      mqtt_send_metric("equipmentID", equipmentId);

      mqtt_send_metric("reading/electricity_delivered_1", electricityUsedTariff1);
      mqtt_send_metric("reading/electricity_delivered_2", electricityUsedTariff2);
      mqtt_send_metric("reading/electricity_returned_1", electricityReturnedTariff1);
      mqtt_send_metric("reading/electricity_returned_2", electricityReturnedTariff2);
      mqtt_send_metric("reading/electricity_currently_delivered", actualElectricityPowerDelivered);
      mqtt_send_metric("reading/electricity_currently_returned", actualElectricityPowerReturned);

      mqtt_send_metric("reading/phase_currently_delivered_l1", activePowerL1P);
      mqtt_send_metric("reading/phase_currently_delivered_l2", activePowerL2P);
      mqtt_send_metric("reading/phase_currently_delivered_l3", activePowerL3P);
      mqtt_send_metric("reading/phase_currently_returned_l1", instantaneousCurrentL1);
      mqtt_send_metric("reading/phase_currently_returned_l2", instantaneousCurrentL2);
      mqtt_send_metric("reading/phase_currently_returned_l3", instantaneousCurrentL3);
      mqtt_send_metric("reading/phase_voltage_l1", instantaneousVoltageL1);
      mqtt_send_metric("reading/phase_voltage_l2", instantaneousVoltageL2);
      mqtt_send_metric("reading/phase_voltage_l3", instantaneousVoltageL3);
    
      mqtt_send_metric("consumption/gas/delivered", gasReceived5min);

 //     mqtt_send_metric("meter-stats/actual_tarif_group", tariffIndicatorElectricity[3]);
    send_metric("meter-stats/actual_tarif_group", tariffIndicatorElectricity[3]);
      mqtt_send_metric("meter-stats/power_failure_count", numberPowerFailuresAny);
      mqtt_send_metric("meter-stats/long_power_failure_count", numberLongPowerFailuresAny);
      mqtt_send_metric("meter-stats/short_power_drops", numberVoltageSagsL1);
      mqtt_send_metric("meter-stats/short_power_peaks", numberVoltageSwellsL1);    

      send_metric("day-consumption/electricity1", (atof(electricityUsedTariff1) - atof(log_data.dayE1)));
      send_metric("day-consumption/electricity2", (atof(electricityUsedTariff2) - atof(log_data.dayE2)));
      send_metric("day-consumption/electricity1_returned", (atof(electricityReturnedTariff1) - atof(log_data.dayR1)));
      send_metric("day-consumption/electricity2_returned", (atof(electricityReturnedTariff2) - atof(log_data.dayR2)));

      send_metric("day-consumption/electricity_merged", ((atof(electricityUsedTariff1) - atof(log_data.dayE1)) + (atof(electricityUsedTariff2) - atof(log_data.dayE2))));
      send_metric("day-consumption/electricity_returned_merged", ((atof(electricityReturnedTariff1) - atof(log_data.dayR1)) + (atof(electricityReturnedTariff2) - atof(log_data.dayR2))));
      send_metric("day-consumption/gas", (atof(gasReceived5min) - atof(log_data.dayG)));

      send_metric("current-month/electricity1", (atof(electricityUsedTariff1) - atof(log_data.monthE1)));
      send_metric("current-month/electricity2", (atof(electricityUsedTariff2) - atof(log_data.monthE2)));
      send_metric("current-month/electricity1_returned", (atof(electricityReturnedTariff1) - atof(log_data.monthR1)));
      send_metric("current-month/electricity2_returned", (atof(electricityReturnedTariff2) - atof(log_data.monthR2)));

      send_metric("current-month/electricity_merged", ((atof(electricityUsedTariff1) - atof(log_data.monthE1)) + (atof(electricityUsedTariff2) - atof(log_data.monthE2))));
      send_metric("current-month/electricity_returned_merged", ((atof(electricityReturnedTariff1) - atof(log_data.monthR1)) + (atof(electricityReturnedTariff2) - atof(log_data.monthR2))));
      send_metric("current-month/gas", (atof(gasReceived5min) - atof(log_data.monthG)));
      LastReport = timestampkaal();
      MqttDelivered = true;
      LastReportinMillis = millis();
    return;
    
  }
      mqtt_send_metric("equipmentID", equipmentId);

      mqtt_send_metric("consumption_low_tarif", electricityUsedTariff1);
      mqtt_send_metric("consumption_high_tarif", electricityUsedTariff2);
      mqtt_send_metric("returndelivery_low_tarif", electricityReturnedTariff1);
      mqtt_send_metric("returndelivery_high_tarif", electricityReturnedTariff2);
      mqtt_send_metric("actual_consumption", actualElectricityPowerDelivered);
      mqtt_send_metric("actual_returndelivery", actualElectricityPowerReturned);

      mqtt_send_metric("l1_instant_power_usage", activePowerL1P);
      mqtt_send_metric("l2_instant_power_usage", activePowerL2P);
      mqtt_send_metric("l3_instant_power_usage", activePowerL3P);
      mqtt_send_metric("l1_instant_power_current", instantaneousCurrentL1);
      mqtt_send_metric("l2_instant_power_current", instantaneousCurrentL2);
      mqtt_send_metric("l3_instant_power_current", instantaneousCurrentL3);
      mqtt_send_metric("l1_voltage", instantaneousVoltageL1);
      mqtt_send_metric("l2_voltage", instantaneousVoltageL2);
      mqtt_send_metric("l3_voltage", instantaneousVoltageL3);
    
      mqtt_send_metric("gas_meter_m3", gasReceived5min);

      send_metric("actual_tarif_group", tariffIndicatorElectricity[3]);
      mqtt_send_metric("short_power_outages", numberPowerFailuresAny);
      mqtt_send_metric("long_power_outages", numberLongPowerFailuresAny);
      mqtt_send_metric("short_power_drops", numberVoltageSagsL1);
      mqtt_send_metric("short_power_peaks", numberVoltageSwellsL1); 
      LastReportinMillis = millis();  
#endif
#ifdef GERMAN
      mqtt_send_metric("equipmentID", equipmentId);

      mqtt_send_metric("consumption_low_tarif", electricityUsedTariff1);
      mqtt_send_metric("consumption_high_tarif", electricityUsedTariff2);
      mqtt_send_metric("returndelivery_low_tarif", electricityReturnedTariff1);
      mqtt_send_metric("returndelivery_high_tarif", electricityReturnedTariff2);
      mqtt_send_metric("actual_consumption", actualElectricityPowerDelivered);
      mqtt_send_metric("actual_returndelivery", actualElectricityPowerReturned);

      mqtt_send_metric("l1_instant_power_usage", activePowerL1P);
      mqtt_send_metric("l2_instant_power_usage", activePowerL2P);
      mqtt_send_metric("l3_instant_power_usage", activePowerL3P);
      mqtt_send_metric("l1_instant_power_current", instantaneousCurrentL1);
      mqtt_send_metric("l2_instant_power_current", instantaneousCurrentL2);
      mqtt_send_metric("l3_instant_power_current", instantaneousCurrentL3);
      mqtt_send_metric("l1_voltage", instantaneousVoltageL1);
      mqtt_send_metric("l2_voltage", instantaneousVoltageL2);
      mqtt_send_metric("l3_voltage", instantaneousVoltageL3);
    
      mqtt_send_metric("gas_meter_m3", gasReceived5min);

      mqtt_send_metric("actual_tarif_group", tariffIndicatorElectricity);
      mqtt_send_metric("short_power_outages", numberPowerFailuresAny);
      mqtt_send_metric("long_power_outages", numberLongPowerFailuresAny);
      mqtt_send_metric("short_power_drops", numberVoltageSagsL1);
      mqtt_send_metric("short_power_peaks", numberVoltageSwellsL1);
      mqtt_send_metric("P1module_voltage", outstr);
      LastReportinMillis = millis();
#endif
#ifdef SWEDISH
/*
1-0:1.8.0 Mätarställning Aktiv Energi Uttag.  
1-0:2.8.0 Mätarställning Aktiv Energi Inmatning 
1-0:3.8.0 Mätarställning Reaktiv Energi Uttag 
1-0:4.8.0 Mätarställning Reaktiv Energi Inmatning 


1-0:1.7.0 Aktiv Effekt Uttag  Momentan trefaseffekt
1-0:2.7.0 Aktiv Effekt Inmatning  Momentan trefaseffekt
1-0:3.7.0 Reaktiv Effekt Uttag  Momentan trefaseffekt
1-0:4.7.0 Reaktiv Effekt Inmatning  Momentan trefaseffekt
1-0:21.7.0  L1 Aktiv Effekt Uttag Momentan effekt
1-0:22.7.0  L1 Aktiv Effekt Inmatning Momentan effekt
1-0:41.7.0  L2 Aktiv Effekt Uttag Momentan effekt
1-0:42.7.0  L2 Aktiv Effekt Inmatning Momentan effekt
1-0:61.7.0  L3 Aktiv Effekt Uttag Momentan effekt
1-0:62.7.0  L3 Aktiv Effekt Inmatning Momentan effekt
1-0:23.7.0  L1 Reaktiv Effekt Uttag Momentan effekt
1-0:24.7.0  L1 Reaktiv Effekt Inmatning Momentan effekt
1-0:43.7.0  L2 Reaktiv Effekt Uttag Momentan effekt
1-0:44.7.0  L2 Reaktiv Effekt Inmatning Momentan effekt
1-0:63.7.0  L3 Reaktiv Effekt Uttag Momentan effekt
1-0:64.7.0  L3 Reaktiv Effekt Inmatning Momentan effekt
1-0:32.7.0  L1 Fasspänning  Momentant RMS-värde
1-0:52.7.0  L2 Fasspänning  Momentant RMS-värde
1-0:72.7.0  L3 Fasspänning  Momentant RMS-värde
1-0:31.7.0  L1 Fasström Momentant RMS-värde
1-0:51.7.0  L2 Fasström Momentant RMS-värde
1-0:71.7.0  L3 Fasström Momentant RMS-värde
*/
/*
 * https://github.com/forsberg/esphome-p1reader
 */
      mqtt_send_metric("cumulativeActiveImport", cumulativeActiveImport);       // 1.8.0
      mqtt_send_metric("cumulativeActiveExport", cumulativeActiveExport);       // 2.8.0
      mqtt_send_metric("cumulativeReactiveImport", cumulativeReactiveImport);   // 3.8.0
      mqtt_send_metric("cumulativeReactiveExport", cumulativeReactiveExport);   // 4.8.0

      mqtt_send_metric("momentaryActiveImport", momentaryActiveImport);         // 1.7.0
      mqtt_send_metric("momentaryActiveExport", momentaryActiveExport);         // 2.7.0
      mqtt_send_metric("momentaryReactiveImport", momentaryReactiveImport);     // 3.7.0
      mqtt_send_metric("momentaryReactiveExport", momentaryReactiveExport);     // 4.7.0      

      mqtt_send_metric("momentaryActiveImportL1", activePowerL1P);              // 21.7.0
      mqtt_send_metric("momentaryActiveExportL1", reactivePowerL1NP);           // 22.7.0

      mqtt_send_metric("momentaryActiveImportL2", activePowerL2P);              // 41.7.0
      mqtt_send_metric("momentaryActiveExportL2", reactivePowerL2NP);           // 42.7.0

      mqtt_send_metric("momentaryActiveImportL3", activePowerL3P);              // 61.7.0
      mqtt_send_metric("momentaryActiveExportL3", reactivePowerL3NP);           // 62.7.0

      mqtt_send_metric("momentaryReactiveImportL1", momentaryReactiveImportL1);   // 23.7.0
      mqtt_send_metric("momentaryReactiveImportL1", momentaryReactiveExportL1);   // 24.7.0
      
      mqtt_send_metric("momentaryReactiveImportL2", momentaryReactiveImportL2);   // 43.7.0
      mqtt_send_metric("momentaryReactiveExportL2", momentaryReactiveExportL2);   // 44.7.0
      
      mqtt_send_metric("momentaryReactiveImportL3", momentaryReactiveImportL3);   // 63.7.0
      mqtt_send_metric("momentaryReactiveExportL1", momentaryReactiveExportL3);   // 64.7.0

      mqtt_send_metric("voltageL1", instantaneousVoltageL1);  // 32.7.0
      mqtt_send_metric("voltageL2", instantaneousVoltageL2);  // 52.7.0
      mqtt_send_metric("voltageL3", instantaneousVoltageL3);  // 72.7.0

      mqtt_send_metric("currentL1", instantaneousCurrentL1);  // 31.7.0
      mqtt_send_metric("currentL2", instantaneousCurrentL2);  // 51.7.0
      mqtt_send_metric("currentL3", instantaneousCurrentL3);  // 71.7.0
//      mqtt_send_metric("P1module_voltage", outstr);
#endif      
      LastReport = timestamp();
      MqttDelivered = true;
      LastReportinMillis = millis();
}

void MQTT_Debug(){
      char outstr[10];
      dtostrf(volts/1000, 1,3, outstr);
      send_mqtt_message("p1wifi/P1module_voltage", outstr);

      dtostrf(millis(), 10,0, outstr);
      send_mqtt_message("p1wifi/P1now", outstr);

      dtostrf(nextUpdateTime, 10,0, outstr);
      send_mqtt_message("p1wifi/P1next", outstr);
      
      dtostrf(time_to_sleep, 10,0, outstr);
      send_mqtt_message("p1wifi/P1time_toSleep", outstr);
      
      dtostrf(time_to_wake, 10,0, outstr);
      send_mqtt_message("p1wifi/P1time_toWake", outstr);

     dtostrf(ESP.getFreeHeap(), 10,0, outstr);
      send_mqtt_message("p1wifi/P1Heap", outstr);

      dtostrf(ESP.getHeapFragmentation(), 10,0, outstr);
      send_mqtt_message("p1wifi/P1HeapFrag", outstr);

      dtostrf(WiFi.RSSI(), 10,0, outstr);
      send_mqtt_message("p1wifi/RSSI", outstr);

      dtostrf(RFpower, 10,0, outstr);
      send_mqtt_message("p1wifi/RF power", outstr);
}
