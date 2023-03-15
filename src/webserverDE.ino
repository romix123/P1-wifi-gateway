#ifdef GERMAN
//
//void setupSaved(String& str){
// // str += F("<script>var countDownDate = new Date().getTime()+50000;var x = setInterval(function() {var now = new Date().getTime();var distance = countDownDate - now;var seconds = Math.floor((distance % (1000 * 60)) / 1000);document.getElementById(\"timer\").innerHTML = seconds + \" seconden tot module weer bereikbaar.\";if (seconds < 2) {location.replace(\"http://p1wifi.local\")}}, 1000);</script>");
//  str += F("<fieldset>");
//  str += F("<fieldset><legend><b>WLAN- und Modul-Setup</b></legend>");
//   str += F("<p><b>Die Einstellungen wurden erfolgreich gespeichert</b><br><br>");
//   str += F("<p>Das Modul wird jetzt neu gestartet. Dies dauert etwa eine Minute.<br>Die blaue LED leuchtet 2x auf, wenn das Modul den Bootvorgang beendet hat</p>");
//   str += F("<p>Die LED blinkt langsam, während die Verbindung zu Ihrem WLAN-Netzwerk hergestellt wird.</p>");
//   str += F("<p>Wenn die blaue LED an bleibt, ist die Einstellung fehlgeschlagen und Sie werden <br>");
//   str += F("muss erneut mit dem WLAN-Netzwerk 'P1_Setup' verbunden werden .</p>");
//   str += F("<br>");
// // str += F("<p id=\"timer\"></p>");
//  str += F("</fieldset></p>");
//  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a></div></div></fieldset></body></html>");
//}

void uploadDiag(String& str){
  monitoring = false; // stop monitoring data
  addHead(str,0);
  addIntro(str);
  str += F("<fieldset><fieldset><legend><b>Update firmware</b></legend>");
  str += F("<form action='' method='post'><form method='POST' action='' enctype='multipart/form-data'><p>");
  str += F("<b>Firmware</b><input type='file' accept='.bin,.bin.gz' name='firmware'></p>");
  str += F("<button type='submit'>Update Firmware</button>");
  str += F("</form>");
  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
  addFoot(str);
  webstate = UPDATE;
}


//
//void handleRoot(){
//    debugln("handleRoot");
//  String str = ""; 
//    addHead(str,0);
//    addIntro(str);
//
//    str += F("<main class='form-signin'>");
//    str += F("<form action='/P1' method='post'><button type='p1' class='button bhome'>Messwerte</button></form>");
//    str += F("<form action='/Setup' method='post'><button type='Setup'>Setup</button></form>");
//    str += F("<form action='/update' method='GET'><button type='submit'>Update firmware</button></form>");
//  addUptime(str);
//  addFoot(str);
//  server.send(200, "text/html", str);
//  webstate = MAIN;
//}

void handleSetup(){
  if (millis() > 60000) {            // if we did not get here directly, check credentials
     debugln("indirect call");
    if (strcmp(server.arg("adminPassword").c_str(), config_data.adminPassword) != 0) {  // passwords don't match
      debugln("Error: handlesetup entered with wrong password");
      errorLogin("Setup");
      return;
    }
  }    
 String str = ""; 
      debugln("handleSetupForm");

    addHead(str,0);
    addIntro(str);
      str += F("<fieldset>");
      str += F("<form action='/SetupSave' method='POST'><p><b>SSId</b><br>");
      str += F("<input type='hidden' name='setuptoken' value='");
       str+= setupToken;
       str+=  F("'>");
             str += F("<fieldset><legend><b>&nbsp;Admin&nbsp;</b></legend>");
      str += F("<p><b>admin password</b><br>");
       str += F("<input type='text' class='form-control' name='adminPassword' value='");
       str+= config_data.adminPassword;
       str+=  F("'></p></fieldset>");
       
       str += F("<fieldset><legend><b>&nbsp;Wifi parameters&nbsp;</b></legend>");
       
       str += F("<input type='text' class='form-control' name='ssid' value='");
       str+= config_data.ssid;
       str+=  F("'></p>");
       str += F("<p><label><b>Password</b></label><br><input type='password' class='form-control' name='password' value='");
       str += config_data.password;
       str += F("'></p>");
      str += F("</fieldset>");
      str += F("<fieldset><legend><b>&nbsp;Domoticz parameters&nbsp;</b></legend>");
      
      str += F("<p><b>An Domoticz melden?</b><input type='checkbox' class='form-control' name='domo' id='domo' ");
      
       if (config_data.domo[0] =='j') str += F(" checked></p>"); else str += F("></p>");
       str += F("<p><b>Domoticz IP address</b><input type='text' class='form-control' name='domoticzIP' value='");
       str += config_data.domoticzIP;
       str += F("'></p><p>");
       str += F("<b>Domoticz Port</b><input type='text' class='form-control' name='domoticzPort' value='");
       str += config_data.domoticzPort;
       str += F("'></p><p>");
       str += F("<b>Domoticz Gas Idx</b><input type='text' class='form-control' name='domoticzGasIdx' value='");
       str += config_data.domoticzGasIdx;
       str += F("'></p><p>");
       str += F("<b>Domoticz Energy Idx</b><input type='text' class='form-control' name='domoticzEnergyIdx' value='");
       str += config_data.domoticzEnergyIdx;
       str += F("'></p>");
      str += F("</fieldset>");

       str += F("<fieldset><legend><b>&nbsp;MQTT parameters&nbsp;</b></legend>");
      str += F("<p><b>An MQTT broker melden?</b><input type='checkbox' class='form-control' name='mqtt' id='mqtt' ");
       if (config_data.mqtt[0] =='j') str += F(" checked></p>"); else str += F("></p>");
     str += F("<p><b>MQTT broker IP address</b><input type='text' class='form-control' name='mqttIP' value='");
       str += config_data.mqttIP;
       str += F("'></p><p>");
       str += F("<b>MQTT broker Port</b><input type='text' class='form-control' name='mqttPort' value='");
       str += config_data.mqttPort;
       str += F("'></p><p>");
       str += F("<b>MQTT user</b><input type='text' class='form-control' name='mqttUser' value='");
       str += config_data.mqttUser;
       str += F("'></p><p>");
       str += F("<b>MQTT password</b><input type='text' class='form-control' name='mqttPass' value='");
       str += config_data.mqttPass;
       str += F("'></p>");
       str += F("<b>MQTT root topic</b><input type='text' class='form-control' name='mqttTopic' value='");
       str += config_data.mqttTopic;
       str += F("'></p>");
       str += F("</fieldset>");
       str += F("<fieldset>");
      str += F("<b>Messintervall in Sek. (> 10 Sek.)</b><input type='text' class='form-control' name='interval' value='");
      str += config_data.interval; 
       str += F("'></p><p>");
       str += F("<p><b>Angabe in Watt (statt in kWh) </b><input type='checkbox' class='form-control' name='watt' id='watt' ");
       if (config_data.watt[0] =='j') str += F(" checked></p>"); else str += F("></p>");
              str += F("<p><b>Activeer Telnet (23) </b><input type='checkbox' class='form-control' name='telnet' id='telnet' ");
       if (config_data.telnet[0] =='j') str += F(" checked></p>"); else str += F("></p>");
       str += F("<p><b>Debug via MQTT </b><input type='checkbox' class='form-control' name='debug' id='debug' ");
       if (config_data.debug[0] =='j') str += F(" checked></p>"); else str += F("></p>");

      str += F("</fieldset><div></div>");
      str += F("<p><button type='submit'>Speichern</button></form>");
      str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form></p>");
     addFoot(str);
    server.send(200, "text/html", str);
    webstate = CONFIG;
}


void handleP1(){
  debugln("handleP1");
  String str = "";
  String eenheid, eenheid2, eenheid3;
  if (reportInDecimals) eenheid = " kWh'></div>"; else eenheid = " Wh'></div>";
 //if (reportInDecimals) eenheid = " kWh</div></div>"; else eenheid = " Wh</div></div>";
  if (reportInDecimals) eenheid2 = " kW'></div></p>"; else eenheid2 = " W'></div></p>";
  
  char str2[10];
  int temp;
    addHead(str,60);
    addIntro(str);
  //  str += ("<p>");
  //  str += P1timestamp;
   // str += P1timestamp[7];
   // str += P1timestamp[8];
   // str += P1timestamp[9];
 // str += ("</p>");
  str += F("<form ><fieldset><legend><b>Messwerte</b></legend>");
 // str += F("<form action='/' method='post'>");

  str += "<p><div class='row'><div class='column'><b>Verbrauch niedriger Satz: <br>total</b><input type='text' class='form-control c6' value='";
  str += electricityUsedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>Heute</b><input type='text' class='form-control c7' value='";
  str += atof(electricityUsedTariff1) - atof(log_data.dayE1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Verbrauch höchster Satz:<br> total</b><input type='text' class='form-control c6' value='";
  str += electricityUsedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>Heute</b><input type='text' class='form-control c7' value='";
  str += atof(electricityUsedTariff2) - atof(log_data.dayE2);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Niedrige Rücklaufquote: total</b><input type='text' class='form-control c6' value='";
  str += electricityReturnedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>Heute</b><input type='text' class='form-control c7' value='";
  str += atof(electricityReturnedTariff1) - atof(log_data.dayR1);
  str += eenheid;
  str += "</div></p>";
  
  str += "<p><div class='row'><div class='column'><b>Hohe Rücklaufquote:<br>insgesamt</b><input type='text' class='form-control c6' value='";
  str += electricityReturnedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>Heute</b><input type='text' class='form-control c7' value='";
  str += atof(electricityReturnedTariff2) - atof(log_data.dayR2);
  str += eenheid;
  str += "</div></p>";
  

  str += "<p><div class='row'><b>Aktuelle Nutzung</b><input type='text' class='form-control c6' value='";
  str += actualElectricityPowerDelivered;
  str += eenheid2;

  str += "<p><div class='row'><b>Aktuelle Einspeisung</b><input type='text' class='form-control c6' value='";
  str += actualElectricityPowerReturned;
  str += eenheid2;

  str += "<p><div class='row'><div class='column3'><b>Stromspannung:<br>L1</b><input type='text' class='form-control c6' value='";
  str += instantaneousVoltageL1;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><br><b>L2</b><input type='text' class='form-control c7' value='";
  str += instantaneousVoltageL2;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><br><b>L3</b><input type='text' class='form-control c7' value='";
   str += instantaneousVoltageL2;
    str += " V'></div></div></p>";

       str += "<p><div class='row'><div class='column3'><b>Stromstärke: <br>L1</b><input type='text' class='form-control c6' value='";
  str += instantaneousCurrentL1;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><br><b>L2</b><input type='text' class='form-control c7' value='";
  str += instantaneousCurrentL2;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><br><b>L3</b><input type='text' class='form-control c7' value='";
   str += instantaneousCurrentL3;
    str += " A'></div></div></p>";
/*

  str += F("<p><b>Voltage dips</b><input type='text' class='form-control' style='text-align:right' value='");
  str += numberVoltageSagsL1;
  str += F("'></p>");
  str += F("<p><b>Voltage pieken</b><input type='text' class='form-control' style='text-align:right' value='");
  str += numberVoltageSwellsL1;
  str += F("'></p>");
  str += F("<p><b>Stroomonderbrekingen</b><input type='text' class='form-control' style='text-align:right' value='");
  str += numberPowerFailuresAny;
  str += F("'></p>");
  */
  str += "<p><div class='row'><div class='column'><b>Gasverbrauch: totaal</b><input type='text' class='form-control c6' value='";
  str += gasReceived5min;
  str += F(" m3'></div>");
  str += "<div class='column' style='text-align:right'><b>Heute</b><input type='text' class='form-control c7' value='";
  str += atof(gasReceived5min) - atof(log_data.dayG);
  str += " m3'></div></div></p>";
  str += F("</fieldset></form>");
  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
  addUptime(str);
  addFoot(str);
  server.send(200, "text/html", str);
  webstate = DATA;
}

void handleRawData(){
  String str;
  debugln("Raw data requested. Here you go.");
  str= "<html><head></head><body>";
  str += datagram;
  str += "</body></html>";
    if (dataEnd){   
    server.send(200, "text/html", str);
    debugln(datagram);
  }
}

#endif
