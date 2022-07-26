/*
 * The reproting functions
 */

 /*
 * MQTT and Telnet support routines
 * 
*/


/* MQTT part based on 
 https://github.com/daniel-jong/esp8266_p1meter/blob/master/esp8266_p1meter/esp8266_p1meter.ino

*/

// **********************************
// * MQTT                           *
// **********************************

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

// * Reconnect to MQTT server and subscribe to in and out topics
bool mqtt_reconnect()
{
    // * Loop until we're reconnected
    int MQTT_RECONNECT_RETRIES = 0;

    while (!mqtt_client.connected() && MQTT_RECONNECT_RETRIES < MQTT_MAX_RECONNECT_TRIES)
    {
        MQTT_RECONNECT_RETRIES++;
        debugff("MQTT connection attempt %d / %d ...\n", MQTT_RECONNECT_RETRIES, MQTT_MAX_RECONNECT_TRIES);

        // * Attempt to connect
        if (mqtt_client.connect(HOSTNAME, user_data.mqttUser, user_data.mqttPass))
        {
            debugln("MQTT connected!");

            // * Once connected, publish an announcement...
            char *message = new char[16 + strlen(HOSTNAME) + 1];
            strcpy(message, "p1 meter alive: ");
            strcat(message, HOSTNAME);
            mqtt_client.publish("hass/status", message);

           debugf("MQTT root topic: %s\n", user_data.mqttTopic);
           MQTT_Server_Fail = false;
        }
        else
        {
            debug("MQTT Connection failed: rc=");
            debugln(mqtt_client.state());
            debugln(" Retrying in 5 seconds");
            debugln("");

            // * Wait 5 seconds before retrying
            delay(5000);
        }
    }

    if (MQTT_RECONNECT_RETRIES >= MQTT_MAX_RECONNECT_TRIES)
    {
        debugf("*** MQTT connection failed, giving up after %d tries ...\n", MQTT_RECONNECT_RETRIES);
        MQTT_Server_Fail = true;
        return false;
    }
    return true;
}

void send_metric(String name, long metric)
{
    char output[10];
    ltoa(metric, output, sizeof(output));

    String topic = String(user_data.mqttTopic) + "/" + name;
    send_mqtt_message(topic.c_str(), output);
    debug(topic);
    debugln(output);
}

void mqtt_send_metric(String name, char *metric)  //long metric)
{
    char output[10];
    String topic = String(user_data.mqttTopic) + "/" + name;
    send_mqtt_message(topic.c_str(), metric);
 //   debug(topic);
 //   debugln(output);
}


/*
 *    Telnet support routines
 * 
 */
void setupTelnet() {  
  // passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  
  // passing a lambda function
  telnet.onInputReceived([](String str) {
    // checks for a certain command
    if (str == "ping") {
      telnet.println("> pong");
      debugln("- Telnet: pong");
    // disconnect the client
    } else if (str == "bye") {
      telnet.println("> disconnecting you...");
      telnet.disconnectClient();
      }
  });

  debug("- Telnet: ");
  if (telnet.begin(port)) {
    debugln("running");
  } else {
    debugln("error.");
  }
}

void onTelnetConnect(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln(" connected");
  telnetConnected = true;
 }

void onTelnetDisconnect(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln(" disconnected");
  telnetConnected = false;
}

void onTelnetReconnect(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln(" reconnected");
  telnetConnected = true;
}

void onTelnetConnectionAttempt(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln("  tried to connect ");
}



bool DomoticzJson(char* idx, int nValue, char* sValue){
  WiFiClient client;
  HTTPClient http;
  bool retVal = false;
  char url[255];
 
  if (user_data.domoticzIP[0] != '-') {
    sprintf(url, "http://%s:%s/json.htm?type=command&param=udevice&idx=%s&nvalue=%d&svalue=%s", user_data.domoticzIP, user_data.domoticzPort, idx, nValue, sValue);
    debugf("[HTTP] GET... URL: %s\n",url);
    http.begin(client, url); //HTTP
    int httpCode = http.GET();
        // httpCode will be negative on error
      if (httpCode > 0)
        { // HTTP header has been sent and Server response header has been handled
            debugf("[HTTP] GET... code: %d\n", httpCode);
             // file found at server
            if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                retVal = true;
                entitledToSleep = true;
            }
        } else
      {
          debugf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    http.end();
    return retVal;
  } // we just return if there is no IP to report to.
  return true;
}



void UpdateGas(){  //sends over the gas setting to server(s)
  if(strncmp(gasReceived5min, prevGAS, sizeof(gasReceived5min)) != 0){          // if we have a new value, report
      DomoticzJson(user_data.domoticzGasIdx, 0, gasDomoticz);// gasReceived5min);
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
      DomoticzJson(user_data.domoticzEnergyIdx, 0, sValue);
}

void TelnetReporter(){
  if (telnetConnected){
    telnet.print(datagram);
    debugln("Raw Data Published to Telnet connection.");
   // debugln(datagram);
  }
}

void MQTT_reporter(){
bool mqqtReconnected = false;  

  
//    if (!mqtt_client.connected()) {
//      mqqtReconnected = mqtt_reconnect();
//      debug("MQTT reconnect invoked...");
//      if (!mqqtReconnected) {
//        debugln("and failed.");
//        return;
//      } 
//      debugln("and succeed. Sending data...");
//    }  
#ifdef DUTCH
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

#elifdef SWEDISH
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
#endif

      debugln("MQTT data sent.");
}
