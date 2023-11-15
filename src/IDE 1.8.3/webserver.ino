// HTML statics

void addHead(String& str, int refresh)
{
  String str2; 
  str += F("<!DOCTYPE html><html lang='en' class=''>");
  if (refresh > 0){
  str += F("<META http-equiv='refresh' content='");
    str += String(refresh);
    str += F(";URL=/'>");
  }
  str += F("<meta charset='utf-8'><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
  str += F("<head><title>Slimme meter</title>");
  str += F("<style>");
  str += F("div, fieldset, input, select {padding: 5px; font-size: 1em;}");
  str += F("fieldset {background: #ECEAE4;}");
  str += F("p {margin: 0.5em 0;}");
  str += F("input {width: 100%; box-sizing: border-box; -webkit-box-sizing: border-box; -moz-box-sizing: border-box; background: #ffffff; color: #000000;}");
  str += F("input[type=range] {width: 90%;}");
  str += F("select {background: #ffffff; color: #000000;}"); //
  str += F("textarea {resize: vertical; width: 98%; height: 318px; padding: 5px; overflow: auto; background: #ffffff; color: #000000;}");
  str += F("body {text-align: center; font-family: verdana, sans-serif; background: #ffffff;}");
  str += F("td {padding: 0px;}");
  str += F("button {border: 0; border-radius: 0.3rem; background: #97C1A9; color: #ffffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer;}");
  str += F("button:hover {background: #0e70a4;}");
  str += F(".bred {background: #d43535;}");
  str += F(".bred:hover {background: #931f1f;}");
  str += F(".bhome {background: #55CBCD;}");
  str += F(".bhome:hover {background: #A2E1DB;}");
  str += F("a {color: #1fa3ec;text-decoration: none;}");
  str += F(".p {float: left; text-align: left;}");  
  str += F(".q {float: right;text-align: right;}");
  str += F(".r {border-radius: 0.3em; padding: 2px; margin: 6px 2px;}");
  str += F(".column {float: left;width: 48%;}");
  str += F(".column3 {float: left; width: 31%;}");
  str += F(".row:after {content: \"\";display: table; clear: both;}"); 
  str += F("input.c4 {text-align:left}");
  str += F("input.c6 {text-align:right}");
  str += F("input.c7 {text-align:right; color:#97C1A9}");
  str += F("</style></head>");
}

//void addRefreshHead(String& str)
//{
//  str += F("<!DOCTYPE html><html lang='en' class=''>");
//  str += F("<META http-equiv='refresh' content='60;URL=/'>");
//  str += F("<meta charset='utf-8'><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
//  str += F("<head><title>Slimme meter</title>");
//  str += F("<style>");
//  str += F("div, fieldset, input, select {padding: 5px; font-size: 1em;}");
//  str += F("fieldset {background: #ECEAE4;}");
//  str += F("p {margin: 0.5em 0;}");
//  str += F("body {text-align: center; font-family: verdana, sans-serif; background: #ffffff;}");
//  str += F(".p {float: left; text-align: left;}");  
//  str += F("</style></head>");
//}

void addGraphHead(String& str){
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  // here begin chunked transfer
  server.send(200, "text/html", "");
  
    str += F("<!DOCTYPE html><html lang='en' class=''>");
  str += F("<meta charset='utf-8'><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
  str += F("<head><title>Slimme meter</title>");
  str += F("<style>");
  str += F("div, fieldset, select, input, {padding: 5px; font-size: 1em;}");
  str += F("fieldset {background: #ECEAE4;}");
  str += F("p {margin: 0.5em 0;}");
  str += F("body {text-align: center; font-family: verdana, sans-serif; background: #ffffff;}");
  str += F("button {border: 0; border-radius: 0.3rem; background: #97C1A9; color: #ffffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer;}");
  str += F("button:hover {background: #0e70a4;}");
  str += F(".bhome {background: #55CBCD;}");
  str += F(".bhome:hover {background: #A2E1DB;}");
  str += F(".p {float: left; text-align: left;}</style>");
}

void addIntro(String& str){
    char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
      str += F("<body><div style='text-align:left;display:inline-block;color:#000000;width:600px;'><div style='text-align:center;color:#000000;'><h2>P1 wifi-gateway</h2></div><br>");
      str += F("<p style='text-align:right;font-size:11px;color:#aaa'><a href='http://");
      str += ipstr;
      str += F(HELP);
}

void addUptime(String& str){
  char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
    
  str += F("<div style='text-align:left;font-size:11px;color:#aaa'><hr/>");
    char strUpTime[40];
    int minuten = millis() / 60000;
    int dagen = minuten / 1440;
    minuten = minuten % 1440;
    int uren = minuten / 60;
    minuten = minuten % 60;
    sprintf_P(strUpTime, PSTR(TIMESTAMP), dagen, uren, minuten);
  str += strUpTime;
  str += F("</div><div style='text-align:right;font-size:11px;color:#aaa'>");
  str += meterName;
  str += " @ ";
  str += ipstr;
  str += F("</div>");
  if (Telnet){
  str += F("<div style='text-align:right;font-size:11px;color:#aaa'>");
  str += activetelnets;
  str += F(" actieve telnet connecties");
  str += F("</div>");
  }
}

void handleRoot(){
    debugln("handleRoot");
   String str = "";
    addHead(str,0);
    addIntro(str);

    str += F("<main class='form-signin'>");
     str += F("<fieldset><legend><b> Data </b></legend>");
    str += F(MAIN_METER_VALUES);
#if GRAPH == 1
  str += F(MAIN_GRAPHS);
 // str += F("<form action='/Dir' method='GET'><button type='submit'>Directory</button></form>");
#endif
    str += F("</fieldset>");
     str += F(MAIN_SETUP_TITLE);
    str += F(MAIN_SETUP);
    str += F(MAIN_UPDATE);
  addUptime(str);
  addFoot(str);
  server.send(200, "text/html", str);
  webstate = MAIN;
  monitoring = true;
 // nextUpdateTime = millis() + 2000;
}



void handleSetupSave() {
  debugln("handleSetupSave");
  debug("Server args: ");
  debugln(server.args());
  debugln(server.arg("plain"));

  if (server.args() == 0) {
    debugln("lege submit, dus redirect naar handleRoot");
    handleRoot();
    debugln("En terug naar mainloop");
    return;
  }
  
  String str = ""; 
  
  if (server.method() == HTTP_POST) {
    if (strncmp(setupToken, server.arg("setuptoken").c_str(), 16) != 0) {
      debug("Error: non matching tokens: ");
      debug(setupToken);
      debug(" -> returned: ");
      debugln(server.arg("setuptoken"));
      return;
    }
    
    strncpy(config_data.adminPassword,     server.arg("adminPassword").c_str(),     server.arg("adminPassword").length() );

  
    if (server.arg("domo") == "on") config_data.domo[0] = 'j'; else config_data.domo[0] = 'n';
    strncpy(config_data.ssid,     server.arg("ssid").c_str(),     server.arg("ssid").length() );
    strncpy(config_data.password, server.arg("password").c_str(), server.arg("password").length() );
    strncpy(config_data.domoticzIP, server.arg("domoticzIP").c_str(), server.arg("domoticzIP").length() );
    strncpy(config_data.domoticzPort, server.arg("domoticzPort").c_str(), server.arg("domoticzPort").length() );
    strncpy(config_data.domoticzEnergyIdx, server.arg("domoticzEnergyIdx").c_str(), server.arg("domoticzEnergyIdx").length() );
    strncpy(config_data.domoticzGasIdx, server.arg("domoticzGasIdx").c_str(), server.arg("domoticzGasIdx").length() );

    if (server.arg("mqtt") == "on") config_data.mqtt[0] = 'j'; else config_data.mqtt[0] = 'n';
    strncpy(config_data.mqttIP, server.arg("mqttIP").c_str(), server.arg("mqttIP").length() );
    strncpy(config_data.mqttPort, server.arg("mqttPort").c_str(), server.arg("mqttPort").length() );
    strncpy(config_data.mqttUser, server.arg("mqttUser").c_str(), server.arg("mqttUser").length() );
    strncpy(config_data.mqttPass, server.arg("mqttPass").c_str(), server.arg("mqttPass").length() );
    strncpy(config_data.mqttTopic, server.arg("mqttTopic").c_str(), server.arg("mqttTopic").length() );
                
    strncpy(config_data.interval, server.arg("interval").c_str(), server.arg("interval").length() );

    if (server.arg("watt") == "on") config_data.watt[0] = 'j'; else config_data.watt[0] = 'n';
    if (server.arg("telnet") == "on") config_data.telnet[0] = 'j'; else config_data.telnet[0] = 'n';
      if (server.arg("debug") == "on") config_data.debug[0] = 'j'; else config_data.debug[0] = 'n';

    config_data.dataSet[0] = 'j';
    config_data.dataSet[1] =
    config_data.adminPassword[server.arg("adminPassword").length()] =
    config_data.ssid[server.arg("ssid").length()] = 
    config_data.password[server.arg("password").length()] = 
    config_data.domoticzIP[server.arg("domoticzIP").length()] = 
    config_data.domoticzPort[server.arg("domoticzPort").length()] = 
    config_data.domoticzGasIdx[server.arg("domoticzGasIdx").length()] = 
    config_data.domoticzEnergyIdx[server.arg("domoticzEnergyIdx").length()] = 
    config_data.interval[server.arg("interval").length()] =
    config_data.mqttIP[server.arg("mqttIP").length()] = 
    config_data.mqttPort[server.arg("mqttPort").length()] = 
    config_data.mqttUser[server.arg("mqttUser").length()] = 
    config_data.mqttPass[server.arg("mqttPass").length()] = 
    config_data.mqttTopic[server.arg("mqttTopic").length()] = 
    config_data.mqtt[1] = //server.arg("mqtt").length()] = 
    config_data.domo[1] = //server.arg("domo").length()] = 
    config_data.watt[1] = //server.arg("watt").length()] = 
    config_data.telnet[1] = //server.arg("telnet").length()] = 
    config_data.debug[1] = 
    '\0';
    
    EEPROM.put(0, config_data);
    EEPROM.commit();

    addHead(str,0);
    addIntro(str);
    setupSaved(str);
    server.send(200, "text/html", str);    
    delay(500);
    ESP.reset();
  } 
}

void handleLogin(){
  createToken();
    debugln("handleLogin");

  if (millis() < 120000) {
    debug(millis());
    debugln(" – You made it within the timeframe, go to setup without login."); 
    bootSetup = true; // our ticket to handleSetup
    handleSetup();
  }
   String str = "";
    addHead(str,0);
    addIntro(str);
      str += F("<form action='/Setup2' method='POST'><fieldset>");
       str += F("<input type='hidden' name='setuptoken' value='");
       str+= setupToken;
       str+=  F("'>");
      str += F("<fieldset><legend><b>&nbsp;Login&nbsp;</b></legend>");
      str += F("<p><b>Admin password</b><br>");
      str += F("<input type='text' class='form-control' name='adminPassword' value='' </p>");
      str+=  F("</fieldset>");
      str += F("<p><button type='submit'>Login</button></form>");
  addFoot(str);
  server.send(200, "text/html", str);
}

void handleUpdateLogin(){
  createToken();
    debugln("handleUpdateLogin");

   String str = "";
    addHead(str,0);
    addIntro(str);
      str += F("<form action='/uploadDialog' method='POST'><fieldset>");
       str += F("<input type='hidden' name='setuptoken' value='");
       str+= setupToken;
       str+=  F("'>");
      str += F("<fieldset><legend><b>&nbsp;Login&nbsp;</b></legend>");
      str += F("<p><b>Admin password</b><br>");
      str += F("<input type='text' class='form-control' name='adminPassword' value='' </p>");
      str+=  F("</fieldset>");
      str += F("<p><button type='submit'>Login</button></form>");
  addFoot(str);
  server.send(200, "text/html", str);
}

void handleUploadForm(){
   if (strcmp(server.arg("adminPassword").c_str(), config_data.adminPassword) != 0) {  // passwords don't match
      debugln("Error: update entered with wrong password");
      errorLogin("Update");
      return;
    } else  AdminAuthenticated = true;
  String str="";
  monitoring = false; // stop monitoring data
  addHead(str,0);
  addIntro(str);
  str += F("<fieldset><fieldset><legend><b>Update firmware</b></legend>");
  str += F("<form method='POST' action='/update' enctype='multipart/form-data'><p>");
  str += F("<b>Firmware</b><input type='file' accept='.bin,.bin.gz' name='firmware'></p>");
  str += F("<button type='submit'>Update</button>");
  str += F("</form>");
  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
  addFootBare(str); 
  webstate = UPDATE;
  server.send(200, "text/html", str);
}

void errorLogin(String returnpage){
  debugln("errorLogin");
    String str = "";
    addHead(str,0);
    addIntro(str);
      str += F("<fieldset><legend><b>Fout</b></legend>");
      str += F("<p><b>Admin password is incorrect.</b><br>");
      str+=  F("</fieldset>");
      str += F("<form action='/");
      str += returnpage;
      str += F("' method='POST'><button class='button bhome'>Opnieuw</button></form></p>");
    addFoot(str);
    server.send(200, "text/html", str);
    bootSetup = false;
}

void setupSaved(String& str){
//  str += F("<script>var countDownDate = new Date().getTime()+5000;var x = setInterval(function() {var now = new Date().getTime();var distance = countDownDate - now;var seconds = Math.floor((distance % (1000 * 15)) / 1000);document.getElementById(\"timer\").innerHTML = seconds + \" seconden tot module weer bereikbaar.\";if (seconds < 2) {location.replace(\"http://p1wifi.local\")}}, 1000);</script>");
  str += F("<fieldset>");
  str += F(SETUP_SAVED_TITLE);
  str += F(SETUP_SAVED_TEXT);
  str += F("</fieldset></p>");
  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a></div></div></fieldset></body></html>");
}


void successResponse(){
  String str = "";
  addHead(str, 30);
  addIntro(str);
  str += F("<fieldset>");
  str += F(FIRMWARE_UPDATE_SUCCESS_TITLE);
  str += F(FIRMWARE_UPDATE_SUCCESS_TEXT);
  str += F("</fieldset></p>");
  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a></div></div></fieldset></body></html>");
  server.send(200, "text/html", str);
  delay(2000);
}

void handleHelp(){
    char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
 String str;
  addHead(str,0);
  addIntro(str);
  str += F("<fieldset>");
  str += F(HELP_TITLE);
  str += F(HELP_TEXT1);
  str += ipstr;
  str += F("/P1 / p1wifi.local/P1 ");
  str += F(HELP_RAW); 
  str += ipstr;
  str += F("/Data / p1wifi.local/Data</p><br><br>");
   str += F(HELP_TEXT2); 
  str += F("</fieldset></p>");

  str += F("<div style='text-align:right;font-size:11px;'><hr/><form><input type='button' value='Go back!' onclick='history.back()'></form><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a></div></div></fieldset></body></html>");
server.send(200, "text/html", str);
}


void addFoot(String& str){
  str += F("<div style='text-align:right;font-size:11px;color:#aaa;'><hr/>");

  str += F("[");
  if (monitoring) str += F("M"); else str += F("m");
  if (hourFlag) str += F("H"); else str += F("h");
  if (dayFlag) str += F("D"); else str += F("d");
  if (weekFlag) str += F("W"); else str += F("w");
  str += F("] ");
  
  if (Mqtt) {
    if (MqttConnected) str += F("MQTT link: √ "); else str += F("MQTT – ");
    str += F(LAST_SAMPLE);
    str += LastReport;
  }
  str += F(FIRMWARE_REV);
  str += version;
  str += F("<br><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a>");
  str += F("</div></div></body></html>");
}
void addFootBare(String& str){
  str += F("<div style='text-align:right;font-size:11px;color:#aaa;'><hr/>");
  str += F(FIRMWARE_REV);
  str += version;
  str += F("<br><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a>");
  str += F("</div></body></html>"); //</div>
}
