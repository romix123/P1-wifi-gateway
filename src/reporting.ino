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
      DomoticzJson(user_data.domoticzGasIdx, 0, gasReceived5min);
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
      debugln("MQTT data sent.");
}
