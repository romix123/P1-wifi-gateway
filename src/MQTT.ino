/*
 * MQTT functions
 * 
 */


/* MQTT part based on 
 https://github.com/daniel-jong/esp8266_p1meter/blob/master/esp8266_p1meter/esp8266_p1meter.ino

*/

// **********************************
// * MQTT                           *
// **********************************

void doMQTT(){
  debugln("Starting mqtt");
  mqtt_connect();
  MQTT_reporter();
}

void mqtt_connect(){
        if (!mqtt_client.connected()) {
        debugln("mqtt_client loop");
        mqtt_reconnect();
        }
}

void mqtt_reconnect(){
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    debugln("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "P1 Smart Meter – ";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(HOSTNAME, user_data.mqttUser, user_data.mqttPass)){
      debugln("connected");
      // Once connected, publish an announcement...
      mqtt_client.publish("outTopic", "p1 gateway running");
      // ... and resubscribe
      mqtt_client.subscribe("inTopic");
    } else {
      debug("failed, rc=");
      debug(mqtt_client.state());
      debugln(" try again later (non blocking)");
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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    LEDon
  } else {
    LEDoff
  }
}

// * Send a message to a broker topic
void send_mqtt_message(const char *topic, char *payload)
{
    debug("MQTT Outgoing on ");
    debug(topic);
    debugln(payload);

    bool result = mqtt_client.publish(topic, payload, false);

    if (!result)
    {
        debug("MQTT publish to topic ");
        debug(topic);
        debugln(" failed.");
    }
}


void send_metric(String name, long metric)
{
 char output[100];
    ltoa(metric, output, sizeof(output));

    mtopic = String(user_data.mqttTopic) + "/" + name;
    send_mqtt_message(mtopic.c_str(), output);
}

void mqtt_send_metric(String name, char *metric)  //long metric)
{
    char output[100];
    mtopic = String(user_data.mqttTopic) + "/" + name;
    send_mqtt_message(mtopic.c_str(), metric);
}


 void MQTT_reporter(){
      char outstr[10];
      dtostrf(volts/1000, 1,3, outstr);
#ifdef NEDERLANDS
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
      mqtt_send_metric("P1module_voltage", outstr);
#endif
    if (Debug){ //#if MDEBUG == 1      
      mqtt_send_metric("P1module_voltage", outstr);

      dtostrf(millis(), 10,0, outstr);
      mqtt_send_metric("P1now", outstr);

      dtostrf(time_to_sleep, 10,0, outstr);
      mqtt_send_metric("P1time_toSleep", outstr);
      
      dtostrf(time_to_wake, 10,0, outstr);
      mqtt_send_metric("P1time_toWake", outstr);

     dtostrf(ESP.getFreeHeap(), 10,0, outstr);
      mqtt_send_metric("P1Heap", outstr);

      dtostrf(ESP.getHeapFragmentation(), 10,0, outstr);
      mqtt_send_metric("P1HeapFrag", outstr);

      dtostrf(WiFi.RSSI(), 10,0, outstr);
      mqtt_send_metric("RSSI", outstr);

      
      dtostrf(RFpower, 10,0, outstr);
      mqtt_send_metric("RF power", outstr);
    }
// #endif // debug == 3
      debugln("MQTT data sent.");
}
