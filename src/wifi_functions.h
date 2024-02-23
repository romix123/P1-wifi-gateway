/*
 * everything related to WIFI
 */
const char *serverIndex =
    "<form method='POST' action='/update' enctype='multipart/form-data'><input "
    "type='file' name='update'><input type='submit' value='Update'></form>";

void start_webservices() {
  Log.verboseln("Starting Webservices");
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
    Log.verboseln("Index");
    handleUpdateLogin();
  });
  server.on(
      "/update", HTTP_POST,
      []() {
        if (AdminAuthenticated) {
          Log.verboseln("Connection close part");
          // Log.verboseln("Error: handleUpdateLogin entered with wrong password");
          server.sendHeader("Connection", "close");
          server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
          //  ESP.restart();
        }
      },
      []() {
        if (AdminAuthenticated) {
          HTTPUpload &upload = server.upload();
          Log.verboseln("Upload part");
          if (upload.status == UPLOAD_FILE_START) {
            Serial.setDebugOutput(true);
            WiFiUDP::stopAll();
            Log.verbose("Update: %s\n", upload.filename.c_str());
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
              Log.verbose("Update Success: %u\nRebooting...\n", upload.totalSize);
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
  Log.verboseln("   … HTTPupdater");
  server.begin();
  Log.verboseln("   … webserver");
}

void start_services() {
  Log.verboseln("Starting services");
  start_webservices();
  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);
  Log.verboseln("   … MDNS");

  if (Mqtt) {
    mqtt_client.setServer(config_data.mqttIP, atoi(config_data.mqttPort));
    Log.verboseln("MQTT server assigned.");
    // mqtt_reconnect();
    Log.verboseln("   … MQTT");
  }
  if (Telnet) {
    setupTelnet();
    Telnet = true;
    Log.verboseln("   … telnet");
  }
}

void onStationModeGotIP(const WiFiEventStationModeGotIP &event) {
  Log.infoln("WiFi running in Station (normal) mode");
  Log.infoln("IP address: %p", WiFi.localIP());
  start_services();
}

void onWiFiModeChange(const WiFiEventModeChange &event) {
  if (event.newMode == WIFI_AP) {
    Log.infoln("WiFi running in AP mode");
    start_webservices();
  }
}

void initWifiHandlers() {
  stationModeGotIPHandler = WiFi.onStationModeGotIP(onStationModeGotIP);
  wiFiModeChangeHandler = WiFi.onWiFiModeChange(onWiFiModeChange);
}