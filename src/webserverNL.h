#ifdef NEDERLANDS

void handleSetup() {
  if (millis() > 300000) { // if we did not get here 5 mins after startup, check credentials
    debugln("indirect call");
    if (strcmp(server.arg("adminPassword").c_str(),
               config_data.adminPassword) != 0) { // passwords don't match
      debugln("Error: handlesetup entered with wrong password");
      errorLogin("Setup");
      return;
    }
  }
  debugln("direct call");
  // monitoring = false; // stop monitoring data

  String str = "";
  debugln("handleSetupForm");

  addHead(str, 0);
  addIntro(str);
  str += F("<fieldset>");
  str += F("<form action='/SetupSave' method='POST'><fieldset>");
  str += F("<input type='hidden' name='setuptoken' value='");
  str += setupToken;
  str += F("'>");
  str += F("<fieldset><legend><b>&nbsp;Admin&nbsp;</b></legend>");
  str += F("<p><b>admin password</b><br>");
  str +=
      F("<input type='text' class='form-control' name='adminPassword' value='");
  str += config_data.adminPassword;
  str += F("'></p></fieldset>");

  str += F("<fieldset><legend><b>&nbsp;Wifi parameters&nbsp;</b></legend>");
  str += F("<p><b>SSId</b><br>");
  str += F("<input type='text' class='form-control' name='ssid' value='");
  str += config_data.ssid;
  str += F("'></p>");
  str += F("<p><label><b>Password</b></label><br><input type='password' "
           "class='form-control' name='password' value='");
  str += config_data.password;
  str += F("'></p></fieldset>");

  str += F("<fieldset><legend><b>&nbsp;Domoticz parameters&nbsp;</b></legend>");

  str += F("<p><b>Rapporteer aan Domoticz?</b><input type='checkbox' "
           "class='form-control' name='domo' id='domo' ");

  if (config_data.domo[0] == 'j')
    str += F(" checked></p>");
  else
    str += F("></p>");
  str += F("<p><b>Domoticz IP address</b><input type='text' "
           "class='form-control' name='domoticzIP' value='");
  str += config_data.domoticzIP;
  str += F("'></p><p>");
  str += F("<b>Domoticz Port</b><input type='text' class='form-control' "
           "name='domoticzPort' value='");
  str += config_data.domoticzPort;
  str += F("'></p><p>");
  str += F("<b>Domoticz Gas Idx</b><input type='text' class='form-control' "
           "name='domoticzGasIdx' value='");
  str += config_data.domoticzGasIdx;
  str += F("'></p><p>");
  str += F("<b>Domoticz Energy Idx</b><input type='text' class='form-control' "
           "name='domoticzEnergyIdx' value='");
  str += config_data.domoticzEnergyIdx;
  str += F("'></p>");
  str += F("</fieldset>");

  str += F("<fieldset><legend><b>&nbsp;MQTT parameters&nbsp;</b></legend>");
  str += F("<p><b>Rapporteer aan MQTT broker?</b><input type='checkbox' "
           "class='form-control' name='mqtt' id='mqtt' ");
  if (config_data.mqtt[0] == 'j')
    str += F(" checked></p>");
  else
    str += F("></p>");
  str += F("<p><b>MQTT broker IP address</b><input type='text' "
           "class='form-control' name='mqttIP' value='");
  str += config_data.mqttIP;
  str += F("'></p><p>");
  str += F("<b>MQTT broker Port</b><input type='text' class='form-control' "
           "name='mqttPort' value='");
  str += config_data.mqttPort;
  str += F("'></p><p>");
  str += F("<b>MQTT user</b><input type='text' class='form-control' "
           "name='mqttUser' value='");
  str += config_data.mqttUser;
  str += F("'></p><p>");
  str += F("<b>MQTT password</b><input type='text' class='form-control' "
           "name='mqttPass' value='");
  str += config_data.mqttPass;
  str += F("'></p>");
  str += F("<b>MQTT root topic</b><input type='text' class='form-control' "
           "name='mqttTopic' value='");
  str += config_data.mqttTopic;
  str += F("'></p>");
  str += F("</fieldset>");
  str += F("<fieldset>");
  str += F("<b>Meetinterval in sec (> 10 sec)</b><input type='text' "
           "class='form-control' name='interval' value='");
  str += config_data.interval;
  str += F("'></p><p>");
  str += F("<p><b>Rapporteer in watt (ipv van in kWh) </b><input "
           "type='checkbox' class='form-control' name='watt' id='watt' ");
  if (config_data.watt[0] == 'j')
    str += F(" checked></p>");
  else
    str += F("></p>");
  str += F("<p><b>Activeer Telnet poort (23) </b><input type='checkbox' "
           "class='form-control' name='telnet' id='telnet' ");
  if (config_data.telnet[0] == 'j')
    str += F(" checked></p>");
  else
    str += F("></p>");
  str += F("<p><b>Debug via MQTT </b><input type='checkbox' "
           "class='form-control' name='debug' id='debug' ");
  if (config_data.debug[0] == 'j')
    str += F(" checked></p>");
  else
    str += F("></p>");
  str += F("</fieldset><div></div>");
  str += F("<p><button type='submit'>Save</button></form>");
  str += F("<form action='/' method='POST'><button class='button "
           "bhome'>Menu</button></form></p>");
  addFoot(str);
  server.send(200, "text/html", str);
  webstate = CONFIG;
}

void handleP1() {
  debug2("handleP1: actualElectricityPowerRet: ");
  debug2ln(actualElectricityPowerRet);
  debug2("handleP1: actualElectricityPowerDeli: ");
  debug2ln(actualElectricityPowerDeli);
  debugln("handleP1");
  String str = "";
  String eenheid, eenheid2, eenheid3;
  if (reportInDecimals)
    eenheid = " kWh'></div>";
  else
    eenheid = " Wh'></div>";
  // if (reportInDecimals) eenheid = " kWh</div></div>"; else eenheid = "
  // Wh</div></div>";
  if (reportInDecimals)
    eenheid2 = " kW'></div></p>";
  else
    eenheid2 = " W'></div></p>";

  addHead(str, 60);
  addIntro(str);

  str += F("<form ><fieldset><legend><b>Meetwaarden</b></legend>");

  str += "<p><div class='row'><div class='column'><b>Verbruik laag tarief: "
         "totaal</b><input type='text' class='form-control c6' value='";
  str += electricityUsedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input "
         "type='text' class='form-control c7' value='";
  str += atof(electricityUsedTariff1) - atof(log_data.dayE1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Verbruik hoog tarief: "
         "totaal</b><input type='text' class='form-control c6' value='";
  str += electricityUsedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input "
         "type='text' class='form-control c7' value='";
  str += atof(electricityUsedTariff2) - atof(log_data.dayE2);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Retour laag tarief: "
         "totaal</b><input type='text' class='form-control c6' value='";
  str += electricityReturnedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input "
         "type='text' class='form-control c7' value='";
  str += atof(electricityReturnedTariff1) - atof(log_data.dayR1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Retour hoog tarief: "
         "totaal</b><input type='text' class='form-control c6' value='";
  str += electricityReturnedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input "
         "type='text' class='form-control c7' value='";
  str += atof(electricityReturnedTariff2) - atof(log_data.dayR2);
  str += eenheid;
  str += "</div></p>";

  debug2("buildpage: actualElectricityPowerRet: ");
  debug2ln(actualElectricityPowerRet);
  debug2("handleP1: actualElectricityPowerDeli: ");
  debug2ln(actualElectricityPowerDeli);

  str += "<p><div class='row'><b>Actueel verbruik</b><input type='text' "
         "class='form-control c6' value='";
  str += actualElectricityPowerDeli;
  str += eenheid2;

  str += "<p><div class='row'><b>Actueel retour</b><input type='text' "
         "class='form-control c6' value='";
  str += actualElectricityPowerRet;
  str += eenheid2;

  if (P1prot == 5) {
    str += "<p><div class='row'><div class='column3'><b>Voltage: L1</b><input "
           "type='text' class='form-control c6' value='";
    str += instantaneousVoltageL1;
    str += " V'></div>";
    str += "<div class='column3' style='text-align:right'><b>L2</b><input "
           "type='text' class='form-control c7' value='";
    str += instantaneousVoltageL2;
    str += " V'></div>";
    str += "<div class='column3' style='text-align:right'><b>L3</b><input "
           "type='text' class='form-control c7' value='";
    str += instantaneousVoltageL3;
    str += " V'></div></div></p>";

    str += "<p><div class='row'><div class='column3'><b>Verbruik: L1</b><input "
           "type='text' class='form-control c6' value='";
    str += activePowerL1P;
    str += " kW'></div>";
    str += "<div class='column3' style='text-align:right'><b>L2</b><input "
           "type='text' class='form-control c7' value='";
    str += activePowerL2P;
    str += " kW'></div>";
    str += "<div class='column3' style='text-align:right'><b>L3</b><input "
           "type='text' class='form-control c7' value='";
    str += activePowerL3P;
    str += " kW'></div></div></p>";

    str += "<p><div class='row'><div class='column3'><b>Retour: L1</b><input "
           "type='text' class='form-control c6' value='";
    str += activePowerL1NP;
    str += " kW'></div>";
    str += "<div class='column3' style='text-align:right'><b>L2</b><input "
           "type='text' class='form-control c7' value='";
    str += activePowerL2NP;
    str += " kW'></div>";
    str += "<div class='column3' style='text-align:right'><b>L3</b><input "
           "type='text' class='form-control c7' value='";
    str += activePowerL3NP;
    str += " kW'></div></div></p>";
  }
  str += "<p><div class='row'><div class='column3'><b>Amperage: L1</b><input "
         "type='text' class='form-control c6' value='";
  str += instantaneousCurrentL1;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><b>L2</b><input "
         "type='text' class='form-control c7' value='";
  str += instantaneousCurrentL2;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><b>L3</b><input "
         "type='text' class='form-control c7' value='";
  str += instantaneousCurrentL3;
  str += " A'></div></div></p>";
  /*

    str += F("<p><b>Voltage dips</b><input type='text' class='form-control'
    style='text-align:right' value='"); str += numberVoltageSagsL1; str +=
    F("'></p>"); str += F("<p><b>Voltage pieken</b><input type='text'
    class='form-control' style='text-align:right' value='"); str +=
    numberVoltageSwellsL1; str += F("'></p>"); str +=
    F("<p><b>Stroomonderbrekingen</b><input type='text' class='form-control'
    style='text-align:right' value='"); str += numberPowerFailuresAny; str +=
    F("'></p>");
    */
  str += "<p><div class='row'><div class='column'><b>Gasverbruik: "
         "totaal</b><input type='text' class='form-control c6' value='";
  str += gasReceived5min;
  str += F(" m3'></div>");
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input "
         "type='text' class='form-control c7' value='";
  str += atof(gasReceived5min) - atof(log_data.dayG);
  str += " m3'></div></div></p>";
  str += F("</fieldset></form>");
  str += F("<form action='/' method='POST'><button class='button "
           "bhome'>Menu</button></form>");
  addUptime(str);
  addFoot(str);
  server.send(200, "text/html", str);
  webstate = DATA;
}

void handleRawData() {
  String str;
  debugln("Raw data requested. Here you go.");
  str = "<html><head></head><body>";
  str += datagram;
  str += "</body></html>";
  if (dataEnd) {
    server.send(200, "text/html", str);
    debugln(datagram);
  }
}

#endif
