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

void setRFPower() {
  RFpower = roundf(20 - (getQuality() / 5)) + 2;
  if (RFpower >= 21)
    RFpower = 20.5;
  if (RFpower < 0)
    RFpower = 1;
  WiFi.setOutputPower(RFpower);
}

void modemSleep() {
  debug("modemSleep: ");
  debugln(millis() / 1000);
  //  stop_services();
  WiFi.shutdown(WiFistate);
  ESP.rtcUserMemoryWrite(RTC_config_data_SLOT_WIFI_STATE,
                         reinterpret_cast<uint32_t *>(&WiFistate),
                         sizeof(WiFistate));
  time_to_wake = millis() + sleepTime; // set alarm for next wakeup
  atsleep = true;
  // blink(1);
  lastSleeptime = millis();
  // wifiSta = false;
}

void modemWake() {
  debug("modemWake: ");
  debugln(millis() / 1000);
  ESP.rtcUserMemoryRead(RTC_config_data_SLOT_WIFI_STATE,
                        reinterpret_cast<uint32_t *>(&WiFistate),
                        sizeof(WiFistate));
  if (!WiFi.resumeFromShutdown(WiFistate) ||
      (WiFi.waitForConnectResult(10000) != WL_CONNECTED)) {
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

void wifiReconnect() {
  debugln("Trying to connect to your wifi network:");
  WiFi.mode(WIFI_STA);
  WiFi.begin(config_data.ssid, config_data.password);
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    ToggleLED delay(500);
    debug("o");
    if (tries++ > 30) {
      debugln("");
      debugln("Something is terribly wrong, can't connect to wifi (anymore).");
      LEDon delay(60000);
      ESP.reset();
    }
  }
}

const char *serverIndex =
    "<form method='POST' action='/update' enctype='multipart/form-data'><input "
    "type='file' name='update'><input type='submit' value='Update'></form>";

void start_webservices() {
  //  String str = "";
  server.on("/", handleRoot);
  server.on("/Setup", handleLogin); // handleSetup
  server.on("/Setup2", handleSetup);
  server.on("/SetupSave", handleSetupSave);
  server.on("/Update", handleUpdateLogin); // use our own authentication
  server.on("/uploadDialog", handleUploadForm);

  server.on("/P1", handleP1);
  server.on("/Data", handleRawData);
  server.on("/Help", handleHelp);
#if GRAPH == 1
  //  server.on("/Graph", handleGraph);
  server.on("/Grafieken", handleGraphMenu);
  server.on("/GasCalendar", calendarGas);
  server.on("/SelectGraph", selectGraph);

  server.on("/zapFiles", zapFiles);
  server.on("/zapConfig", zapConfig);
  server.on("/Dir", DirListing);
  server.on("/Format", formatFS);

  server.onNotFound(handleNotFound);
#endif

  server.on("/update", HTTP_GET, []() {
    debugln("Index");
    handleUpdateLogin();
  });
  server.on(
      "/update", HTTP_POST,
      []() {
        if (AdminAuthenticated) {
          debugln("Connection close part");
          // debugln("Error: handleUpdateLogin entered with wrong password");
          server.sendHeader("Connection", "close");
          server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
          //  ESP.restart();
        }
      },
      []() {
        if (AdminAuthenticated) {
          HTTPUpload &upload = server.upload();
          debugln("Upload part");
          if (upload.status == UPLOAD_FILE_START) {
            Serial.setDebugOutput(true);
            WiFiUDP::stopAll();
            debugff("Update: %s\n", upload.filename.c_str());
            uint32_t maxSketchSpace =
                (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
            if (!Update.begin(maxSketchSpace)) { // start with max available
                                                 // size
              Update.printError(Serial);
            }
          } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) !=
                upload.currentSize) {
              Update.printError(Serial);
            }
          } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(
                    true)) { // true to set the size to the current progress
              debugff("Update Success: %u\nRebooting...\n", upload.totalSize);
              successResponse();
              ESP.restart();
            } else {
              Update.printError(Serial);
            }
            Serial.setDebugOutput(false);
          }
          yield();
        } // AdminAuthenticated
      });
  debugln("   … HTTPupdater");
  server.begin();
  debugln("   … webserver");
}

void start_services() {
  debugln("Starting services");
  start_webservices();
  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);
  debugln("   … MDNS");

  if (Mqtt) {
    mqtt_client.setServer(config_data.mqttIP, atoi(config_data.mqttPort));
    debugln("MQTT server assigned.");
    // mqtt_reconnect();
    debugln("   … MQTT");
  }
  if (Telnet) {
    setupTelnet();
    Telnet = true;
    debugln("   … telnet");
  }
}

void stop_services() {
  //  server.stop();
  if (Telnet)
    telnet.stop();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event) {
  debugln("Connected to Wi-Fi sucessfully.");
  debug("IP address: ");
  debugln(WiFi.localIP());
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event) {
  debugln("Disconnected from Wi-Fi, trying to connect...");
  WiFi.disconnect();
  if (rtcValid)
    WiFi.begin(config_data.ssid, config_data.password, rtcData.channel,
               rtcData.ap_mac, true);
  else
    WiFi.begin(config_data.ssid, config_data.password);
}

void calcSleeptime() {
  switch (webstate) {
  case NONE:
    time_to_sleep = millis() + wakeTime;
    break;
  case MAIN:
    time_to_sleep = millis() + 5000;
    break;
  case WCONFIG:
    time_to_sleep = millis() + wakeTime + 10000;
    break;
  case DATA:
    time_to_sleep = millis() + 1000;
    break;
  case UPDATE:
    time_to_sleep =
        millis() + 99000; // allow sufficient time to upload firmware
    break;
  default:
    time_to_sleep = millis() + wakeTime;
    break;
  }
  debug("Scheduled shutdown at: ");
  debugln(time_to_sleep);
}
