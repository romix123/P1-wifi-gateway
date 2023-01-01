// HTML statics

void addHead(String& str)
{
  str += F("<!DOCTYPE html><html lang='en' class=''>");
  str += F("<meta charset='utf-8'><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
  str += F("<head><title>Slimme meter</title>");
  str += F("<style>");
  str += F("div, fieldset, input, select {padding: 5px; font-size: 1em;}");
  str += F("fieldset {background: #ECEAE4;}");
  str += F("p {margin: 0.5em 0;}");
  str += F("input {width: 100%; box-sizing: border-box; -webkit-box-sizing: border-box; -moz-box-sizing: border-box; background: #ffffff; color: #000000;}");
  str += F("input[type=range] {width: 90%;}");
  str += F("select {width: 100%; background: #ffffff; color: #000000;}");
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
    strncpy(user_data.adminPassword,     server.arg("adminPassword").c_str(),     sizeof(user_data.adminPassword) );
    if (server.arg("domo") == "on") user_data.domo[0] = 'j'; else user_data.domo[0] = 'n';
    strncpy(user_data.ssid,     server.arg("ssid").c_str(),     sizeof(user_data.ssid) );
    strncpy(user_data.password, server.arg("password").c_str(), sizeof(user_data.password) );
    strncpy(user_data.domoticzIP, server.arg("domoticzIP").c_str(), sizeof(user_data.domoticzIP) );
    strncpy(user_data.domoticzPort, server.arg("domoticzPort").c_str(), sizeof(user_data.domoticzPort) );
    strncpy(user_data.domoticzEnergyIdx, server.arg("domoticzEnergyIdx").c_str(), sizeof(user_data.domoticzEnergyIdx) );
    strncpy(user_data.domoticzGasIdx, server.arg("domoticzGasIdx").c_str(), sizeof(user_data.domoticzGasIdx) );

    if (server.arg("mqtt") == "on") user_data.mqtt[0] = 'j'; else user_data.mqtt[0] = 'n';
    strncpy(user_data.mqttIP, server.arg("mqttIP").c_str(), sizeof(user_data.mqttIP) );
    strncpy(user_data.mqttPort, server.arg("mqttPort").c_str(), sizeof(user_data.mqttPort) );
    strncpy(user_data.mqttUser, server.arg("mqttUser").c_str(), sizeof(user_data.mqttUser) );
    strncpy(user_data.mqttPass, server.arg("mqttPass").c_str(), sizeof(user_data.mqttPass) );
    strncpy(user_data.mqttTopic, server.arg("mqttTopic").c_str(), sizeof(user_data.mqttTopic) );
                
    strncpy(user_data.interval, server.arg("interval").c_str(), sizeof(user_data.interval) );

    if (server.arg("watt") == "on") user_data.watt[0] = 'j'; else user_data.watt[0] = 'n';
    if (server.arg("telnet") == "on") user_data.telnet[0] = 'j'; else user_data.telnet[0] = 'n';
      if (server.arg("debug") == "on") user_data.debug[0] = 'j'; else user_data.debug[0] = 'n';

    user_data.dataSet[0] = 'j';
    user_data.dataSet[1] =
    user_data.ssid[server.arg("adminPassword").length()] =
    user_data.ssid[server.arg("ssid").length()] = 
    user_data.password[server.arg("password").length()] = 
    user_data.domoticzIP[server.arg("domoticzIP").length()] = 
    user_data.domoticzPort[server.arg("domoticzPort").length()] = 
    user_data.domoticzGasIdx[server.arg("domoticzGasIdx").length()] = 
    user_data.domoticzEnergyIdx[server.arg("domoticzEnergyIdx").length()] = 
    user_data.interval[server.arg("interval").length()] =
    user_data.mqttIP[server.arg("mqttIP").length()] = 
    user_data.mqttPort[server.arg("mqttPort").length()] = 
    user_data.mqttUser[server.arg("mqttUser").length()] = 
    user_data.mqttPass[server.arg("mqttPass").length()] = 
    user_data.mqttTopic[server.arg("mqttTopic").length()] = 
    user_data.mqtt[1] = //server.arg("mqtt").length()] = 
    user_data.domo[1] = //server.arg("domo").length()] = 
    user_data.watt[1] = //server.arg("watt").length()] = 
    user_data.telnet[1] = //server.arg("telnet").length()] = 
    user_data.debug[1] = 
    '\0';
    
    EEPROM.put(0, user_data);
    EEPROM.commit();

    addHead(str);
    addIntro(str);
    setupSaved(str);
    server.send(200, "text/html", str);    
    delay(500);
    ESP.reset();
  } 
}
