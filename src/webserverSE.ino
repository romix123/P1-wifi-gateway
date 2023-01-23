#ifdef SWEDISH
void addIntro(String& str){
    char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
      str +=("<body><div style='text-align:left;display:inline-block;color:#000000;width:600px;'><div style='text-align:center;color:#000000;'><h2>P1 wifi-gateway</h2></div><br>");
      str += ("<p style='text-align:right;font-size:11px;color:#aaa'><a href='http://");
      str += ipstr;
      str += ("/Help' target='_blank'>Hjälp</a>");
}

void addUptime(String& str){
  char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
    
  str += ("<div style='text-align:left;font-size:11px;color:#aaa'><hr/>");
    char strUpTime[40];
    int minuten = millis() / 60000;
    int dagen = minuten / 1440;
    minuten = minuten % 1440;
    int uren = minuten / 60;
    minuten = minuten % 60;
    sprintf_P(strUpTime, PSTR("%d dagar %d timmar %d minuter"), dagen, uren, minuten);
  str += strUpTime;
  str += ("<p style='text-align:right;font-size:11px;color:#aaa'>");
  str += ipstr;
  str += ("</div>");
}

void addFoot(String& str){
  str += F("<div style='text-align:right;font-size:11px;color:#aaa;'><hr/>version: ");
  str += version;
  char sysmsg[100];
  sprintf_P(sysmsg, PSTR(" – %1.2fV"), volts / 1000);
  str += sysmsg;
  str += F("<br><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a>");
  str += F("</div></div></body></html>");
}

void setupSaved(String& str){
 // str += F("<script>var countDownDate = new Date().getTime()+600000;var x = setInterval(function() {var now = new Date().getTime();var distance = countDownDate - now;var seconds = Math.floor((distance % (1000 * 60)) / 1000);document.getElementById(\"timer\").innerHTML = seconds + \" seconden tot module weer bereikbaar.\";if (seconds < 2) {location.replace(\"http://p1wifi.local\")}}, 1000);</script>");
  str += F("<fieldset>");
  str += F("<fieldset><legend><b>Wifi och moduluppställning</b></legend>");
  str += F("<p><b>Inställningarna har sparats.</b><br><br>");
  str += F("<p>Modulen kommer nu att starta om. Det tar ungefär en minut.</p><br>");
  str += F("<p>Den blå lysdioden tänds 2x när modulen har startat klart.</p>");
  str += F("<p>Lysdioden blinkar långsamt när den ansluter till ditt WiFi-nätverk.</p>");
  str += F("<p>Om den blå lysdioden fortsätter att lysa har inställningen misslyckats och du måste återansluta till WiFi-nätverket 'P1_Setup' .</p>");
  str += F("<br>");
 // str += F("<p id=\"timer\"></p>");
  str += F("</fieldset></p>");
  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://eps8266thingies.nl' target='_blank' style='color:#aaa;'>eps8266thingies.nl</a></div></div></fieldset></body></html>");
}

void uploadDiag(String& str){
  monitoring = false; // stop monitoring data

  addHead(str);
  addIntro(str);
  str += F("<fieldset><fieldset><legend><b>Update firmware</b></legend>");
  str += F("<form action='' method='post'><form method='POST' action='' enctype='multipart/form-data'><p>");
  str += F("<b>Firmware</b><input type='file' accept='.bin,.bin.gz' name='firmware'></p>");
  str += F("<button type='submit'>Uppdatera firmware</button>");
  str += F("</form>");
  str += F("<form action='/' method='POST'><button class='button bhome'>Meny</button></form>");
  addFoot(str); 
  webstate = UPDATE;
}

void successResponse(String& str){
  addHead(str);
  addIntro(str);
  str += F("<fieldset>");
  str += F("<fieldset><legend><b>Firmware update</b></legend>");
  str += F("<p><b>De firmware is succesvol bijgewerkt</b><br><br>");
  str += F("<p>De module zal nu herstarten. Dat duurt ongeveer een minuut</p><br>");
  str += F("<p>De blauwe Led zal 2x oplichten wanneer de module klaar is met opstarten</p>");
  str += F("<p>De led zal langzaam knipperen tijdens koppelen aan uw WiFi netwerk.</p>");
  str += F("<p>Als de blauwe led blijft branden is de instelling mislukt en zult u <br>");
  str += F("opnieuw moeten koppelen met WIfi netwerk 'P1_Setup' met wachtwoord 'configP1'.</p>");
  str += F("</fieldset></p>");
  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://eps8266thingies.nl' target='_blank' style='color:#aaa;'>eps8266thingies.nl</a></div></div></fieldset></body></html>");
}

void handleRoot(){
    debugln("handleRoot");
  String str = ""; 
    addHead(str);
    addIntro(str);

    str += F("<main class='form-signin'>");
    str += F("<form action='/P1' method='post'><button type='p1' class='button bhome'>Mätardata</button></form>");
    str += F("<form action='/Setup' method='post'><button type='Setup'>Uppstart</button></form>");
    str += F("<form action='/update' method='GET'><button type='submit'>Uppdatera firmware</button></form>");
  addUptime(str);
  addFoot(str);
  server.send(200, "text/html", str);
  webstate = MAIN;
}

void handleSetup(){
    debugln("handleSetup");
    monitoring = false; // stop monitoring data

 String str = ""; 
      debugln("handleSetupForm");

    addHead(str);
    addIntro(str);
      str += F("<fieldset>");
       str += F("<fieldset><legend><b>&nbsp;Wifi parametrar&nbsp;</b></legend>");
       str += F("<form action='/SetupSave' method='POST'><p><b>SSId</b><br>");
       str += F("<input type='text' class='form-control' name='ssid' value='");
       str+= config_data.ssid;
       str+=  F("'></p>");
       str += F("<p><label><b>Password</b></label><br><input type='password' class='form-control' name='password' value='");
       str += config_data.password;
       str += F("'></p>");
      str += F("</fieldset>");
      str += F("<fieldset><legend><b>&nbsp;Domoticz parameters&nbsp;</b></legend>");
      
      str += F("<p><b>Rapportera till Domoticz?</b><input type='checkbox' class='form-control' name='domo' id='domo' ");
      
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

       str += F("<fieldset><legend><b>&nbsp;MQTT parametrar&nbsp;</b></legend>");
      str += F("<p><b>Rapportera till MQTT-mäklare?</b><input type='checkbox' class='form-control' name='mqtt' id='mqtt' ");
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
       str += F("<b>Mätintervall in sec (> 10 sec)</b><input type='text' class='form-control' name='interval' value='");
       str += config_data.interval; 
       str += F("'></p><p>");
       str += F("<p><b>Rapportera in watt (istället för in kWh) </b><input type='checkbox' class='form-control' name='watt' id='watt' ");
       if (config_data.watt[0] =='j') str += F(" checked></p>"); else str += F("></p>");
              str += F("<p><b>Activera Telnet (23) </b><input type='checkbox' class='form-control' name='telnet' id='telnet' ");
       if (config_data.telnet[0] =='j') str += F(" checked></p>"); else str += F("></p>");
       str += F("<p><b>Debug po MQTT </b><input type='checkbox' class='form-control' name='debug' id='debug' ");
       if (config_data.debug[0] =='j') str += F(" checked></p>"); else str += F("></p>");

      str += F("</fieldset><div></div>");
      str += F("<p><button type='submit'>Spara</button></form>");
      str += F("<form action='/' method='POST'><button class='button bhome'>Meny</button></form></p>");
     addFoot(str);
    server.send(200, "text/html", str);
    webstate = CONFIG;
}

void handleP1(){
  debugln("handleP1");
  String str = "";
  String eenheid, eenheid2;
  if (reportInDecimals) eenheid = " kWh'></div>"; else eenheid = " Wh'></div>";
 //if (reportInDecimals) eenheid = " kWh</div></div>"; else eenheid = " Wh</div></div>";
  if (reportInDecimals) eenheid2 = " kW'></div></p>"; else eenheid2 = " W'></div></p>";
  
  char str2[10];
  int temp;
    addHead(str);
    addIntro(str);
  //  str += ("<p>");
  //  str += P1timestamp;
   // str += P1timestamp[7];
   // str += P1timestamp[8];
   // str += P1timestamp[9];
 // str += ("</p>");
  str += F("<form ><fieldset><legend><b>Mätardata</b></legend>");
 // str += F("<form action='/' method='post'>");

  str += "<p><div class='row'><div class='column'><b>Aktiv energi uttag</b><input type='text' class='form-control c6' value='";
  str += cumulativeActiveImport;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>idag</b><input type='text' class='form-control c7' value='";
  str += atof(cumulativeActiveImport) - atof(log_data.dayE1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Reaktiv energi uttag</b><input type='text' class='form-control c6' value='";
  str += cumulativeReactiveImport;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>idag</b><input type='text' class='form-control c7' value='";
  str += atof(cumulativeReactiveImport) - atof(log_data.dayE2);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Aktiv energi inmatning</b><input type='text' class='form-control c6' value='";
  str += cumulativeActiveExport;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>idag</b><input type='text' class='form-control c7' value='";
  str += atof(cumulativeActiveExport) - atof(log_data.dayR1);
  str += eenheid;
  str += "</div></p>";
  
  str += "<p><div class='row'><div class='column'><b>Reaktiv energi inmatning</b><input type='text' class='form-control c6' value='";
  str += cumulativeReactiveExport;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>idag</b><input type='text' class='form-control c7' value='";
  str += atof(cumulativeReactiveExport) - atof(log_data.dayR2);
  str += eenheid;
  str += "</div></p>";
  

  str += "<p><div class='row'><b>Aktiv Effekt Uttag Momentan trefaseffekt</b><input type='text' class='form-control c6' value='";
  str += momentaryActiveImport;
  str += eenheid2;

  str += "<p><div class='row'><b>Fasspänning</b><input type='text' class='form-control c6' value='";
  str += momentaryActiveExport;
  str += eenheid2;

  str += "<p><div class='row'><div class='column3'><b>Aktiv Effekt Uttag:<br>L1</b><input type='text' class='form-control c6' value='";
  str += instantaneousVoltageL1;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><br><b>L2</b><input type='text' class='form-control c7' value='";
  str += instantaneousVoltageL2;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><br><b>L3</b><input type='text' class='form-control c7' value='";
   str += instantaneousVoltageL3;
    str += " V'></div></div></p>";

  str += "<p><div class='row'><div class='column3'><b>Fasström:<br>L1</b><input type='text' class='form-control c6' value='";
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
  if (gasReceived5min[0] != '\0'){
  str += "<p><div class='row'><div class='column'><b>Gasförbrukning: total</b><input type='text' class='form-control c6' value='";
  str += gasReceived5min;
  str += F(" m3'></div>");
  str += "<div class='column' style='text-align:right'><b>idag</b><input type='text' class='form-control c7' value='";
  str += atof(gasReceived5min) - atof(log_data.dayG);
  str += " m3'></div></div></p>";
  }
  str += F("</fieldset></form>");
  str += F("<form action='/' method='POST'><button class='button bhome'>Meny</button></form>");
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

void handleHelp(){
    char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
  
 String str;
 addHead(str);
  addIntro(str);
  str += F("<fieldset>");
  str += F("<fieldset><legend><b>Hjälp</b></legend>");
  str += F("<p><b>P1 WiFi-gatewayen kan leverera data på fyra sätt.</b><br><br>");
  str += F("<p>Kärnvärdena är alltid tillgängliga via P1 Meny (of via ");
  str += ipstr;
  str += F("/P1, eller via p1wifi.local/P1 )</p><br>");
  
  str += F("<p>Rådata är tillgänglig via ");
  str += ipstr;
  str += F("/Data, eller via p1wifi.local/Data )</p><br><br>");
  str += F("<p>Vanligare är användningen av gatewayen med ett hemautomationssystem.</p>");
  str += F("<p><b>Domoticz</b> accepterar json-meddelanden. För att göra detta, ange IP-adressen för ");
  str += F("din Domoticz-server och portnumret på vilken den kan nås (vanligtvis 8080).</p>");
  str += F("Du får den nödvändiga Idx för gas och el genom att först använda virtuella sensorer i Domoticz ");
  str += F("att skapa för båda. Efter skapande visas Idxerna under fliken 'devices'.</p><br>");
  str +=F("");
  str +=F("Data kan också läsas (av till exempel Domoticz) via port 23 på modulene (p1wifi.local:23).");
  str +=F("Så här kan Domoticz hårdvaruenhet [P1 Smart Meter med LAN-gränssnitt] hämta data.");
  str +=F("Ingenting behöver ställas in på p1wifi-sidan för detta (inga bockar vid json och mqtt ). ");

  str += F("För andra system kan du använda en MQTT-mäklare. Fyll i uppgifterna om ");
  str += F("mäklare och ställ in rotämnet. För Home Assistant alltså 'sensors/power/p1meter'.</p>");
  str += F("Daniel de Jong beskriver i sin <a href='https://github.com/daniel-jong/esp8266_p1meter'>github</a> hur man konfigurerar HA ytterligare.</p>");
  str += F("Använd kryssrutorna för att ange vilken/vilka rapporteringsmetoder du vill använda.</p>");
  str += F("Erik Forsberg beskriver i sin <a href='https://github.com/forsberg/esphome-p1reader'>github</a> hur man konfigurerar esphome ytterligare.</p>");

  str += F("</fieldset></p>");
  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://eps8266thingies.nl' target='_blank' style='color:#aaa;'>eps8266thingies.nl</a></div></div></fieldset></body></html>");
server.send(200, "text/html", str);
}

#endif
