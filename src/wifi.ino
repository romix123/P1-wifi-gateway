/*
 * everything related to WIFI
 */


/*
   Return the quality (Received Signal Strength Indicator)
   of the WiFi network.
   Returns a number between 0 and 100 if WiFi is connected.
   Returns -1 if WiFi is disconnected.
*/
int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void setRFPower(){
     RFpower = roundf(20-(getQuality()/5)) + 2;
     if (RFpower >= 21) RFpower = 20.5;
     if (RFpower < 0) RFpower = 1;
     WiFi.setOutputPower(RFpower);
}

 
void modemSleep(){
   debug("modemSleep: ");
   debugln(millis()/1000);
 //  stop_services();
   WiFi.shutdown(WiFistate);
   ESP.rtcUserMemoryWrite(RTC_USER_DATA_SLOT_WIFI_STATE, reinterpret_cast<uint32_t *>(&WiFistate), sizeof(WiFistate));
  time_to_wake =  millis() + sleepTime; // set alarm for next wakeup
  atsleep = true;
 // blink(1);
  lastSleeptime = millis();
  //wifiSta = false;
}

void modemWake(){
   debug("modemWake: ");
   debugln(millis()/1000);
   ESP.rtcUserMemoryRead(RTC_USER_DATA_SLOT_WIFI_STATE, reinterpret_cast<uint32_t *>(&WiFistate), sizeof(WiFistate));
  if (!WiFi.resumeFromShutdown(WiFistate)
      || (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
    Serial.println("Cannot resume WiFi connection, connecting via begin...");
    WiFi.persistent(false);
    wifiReconnect();
  } else {
    debugln("RTC wakeup.");
  }
  wifiSta = true;
  webstate = NONE;
  atsleep = false;
  start_services();
  calcSleeptime(); // when do we go to sleep again?
}


 void wifiReconnect(){
    debugln("Trying to connect to your wifi network:");
     WiFi.mode(WIFI_STA);
     WiFi.begin(user_data.ssid, user_data.password);
     byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        ToggleLED
        delay(500);
        debug("o");
        if (tries++ > 30) {
          debugln("");
          debugln("Something is terribly wrong, can't connect to wifi (anymore).");
          LEDon
          delay(60000);
          ESP.reset();
    }
  }
}

void start_webservices(){
    server.on("/",       handleRoot);
    server.on("/Setup",  handleLogin); // handleSetup
    server.on("/Setup2", handleSetup);
    server.on("/SetupSave",  handleSetupSave);
    server.on("/P1",     handleP1);
    server.on("/Data",  handleRawData);
    server.on("/Help", handleHelp);
    server.begin();
    debugln("   …webserver");
}

void start_services(){
    debugln("Starting services…");
    start_webservices();
    MDNS.begin(host);
    MDNS.addService("http", "tcp", 80);
    debugln("   … MDNS");
    
    httpUpdater.setup(&server, "/update", update_username, user_data.adminPassword);
    debugln("   … HTTPupdater");
 
    if (Mqtt){
        mqtt_client.setServer(user_data.mqttIP, atoi(user_data.mqttPort));
        debugln("MQTT server assigned.");
       // mqtt_reconnect();
    debugln("   … MQTT");
    }
    if (Telnet){ 
      setupTelnet();
      debugln("   … telnet");
    }
}

void stop_services(){
//  server.stop();
  if (Telnet) telnet.stop();
}



void calcSleeptime(){
  switch (webstate){
    case NONE:
        time_to_sleep = millis() + wakeTime;
      break;
    case MAIN:
        time_to_sleep = millis() + 5000;
      break;
    case CONFIG:
        time_to_sleep = millis() + wakeTime + 10000;
      break;
    case DATA:
        time_to_sleep = millis() + 1000;
      break;
    case UPDATE:
        time_to_sleep = millis() + 99000; // allow sufficient time to upload firmware
      break;
    default:
      time_to_sleep = millis() + wakeTime;
      break;
  }
  debug("Scheduled shutdown at: ");
  debugln(time_to_sleep);
}
