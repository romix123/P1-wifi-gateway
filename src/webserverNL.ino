#ifdef NEDERLANDS
void addIntro(String& str){
    char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
      str +=("<body><div style='text-align:left;display:inline-block;color:#000000;width:600px;'><div style='text-align:center;color:#000000;'><h2>P1 wifi-gateway</h2></div><br>");
      str += ("<p style='text-align:right;font-size:11px;color:#aaa'><a href='http://");
      str += ipstr;
      str += ("/Help' target='_blank'>Help</a>");
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
    sprintf_P(strUpTime, PSTR("%d dagen %d uren %d minuten"), dagen, uren, minuten);
  str += strUpTime;
  str += ("<p style='text-align:right;font-size:11px;color:#aaa'>");
  str += ipstr;
  str += ("</div>");
}

void addFoot(String& str){
  str += F("<div style='text-align:right;font-size:11px;color:#aaa;'><hr/>versie: ");
  str += version;
  char sysmsg[100];
  sprintf_P(sysmsg, PSTR(" – %1.2fV"), volts / 1000);
  str += sysmsg;
  str += F("<br><a href='http://esp8266thingies.nl' target='_blank' style='color:#aaa;'>esp8266thingies.nl</a>");
  str += F("</div></div></body></html>");
}

void setupSaved(String& str){
//  str += F("<script>var countDownDate = new Date().getTime()+5000;var x = setInterval(function() {var now = new Date().getTime();var distance = countDownDate - now;var seconds = Math.floor((distance % (1000 * 15)) / 1000);document.getElementById(\"timer\").innerHTML = seconds + \" seconden tot module weer bereikbaar.\";if (seconds < 2) {location.replace(\"http://p1wifi.local\")}}, 1000);</script>");
  str += F("<fieldset>");
  str += F("<fieldset><legend><b>Wifi en module setup</b></legend>");
  str += F("<p><b>De instellingen zijn succesvol bewaard</b><br><br>");
  str += F("<p>De module zal nu herstarten. Dat duurt ongeveer een minuut</p><br>");
  str += F("<p>De blauwe Led zal 2x oplichten wanneer de module klaar is met opstarten</p>");
  str += F("<p>De led zal langzaam knipperen tijdens koppelen aan uw WiFi netwerk.</p>");
  str += F("<p>Als de blauwe led blijft branden is de instelling mislukt en zult u <br>");
  str += F("opnieuw moeten koppelen met WIfi netwerk 'P1_Setup' .</p>");
  str += F("<br>");
  //str += F("<p id=\"timer\"></p>");
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
  str += F("<button type='submit'>Update Firmware</button>");
  str += F("</form>");
  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
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
    str += F("<form action='/P1' method='post'><button type='p1' class='button bhome'>Meterdata</button></form>");
    str += F("<form action='/Setup' method='post'><button type='Setup'>Setup</button></form>");
    str += F("<form action='/update' method='GET'><button type='submit'>Update firmware</button></form>");
  addUptime(str);
  addFoot(str);
  server.send(200, "text/html", str);
  webstate = MAIN;
  monitoring = true;
}

void handleSetup(){
    debugln("handleSetup");
    monitoring = false; // stop monitoring data

 String str = ""; 
      debugln("handleSetupForm");

    addHead(str);
    addIntro(str);
      str += F("<fieldset>");
       str += F("<fieldset><legend><b>&nbsp;Wifi parameters&nbsp;</b></legend>");
       str += F("<form action='/SetupSave' method='POST'><p><b>SSId</b><br>");
       str += F("<input type='text' class='form-control' name='ssid' value='");
       str+= user_data.ssid;
       str+=  F("'></p>");
       str += F("<p><label><b>Password</b></label><br><input type='password' class='form-control' name='password' value='");
       str += user_data.password;
       str += F("'></p>");
      str += F("</fieldset>");
      str += F("<fieldset><legend><b>&nbsp;Domoticz parameters&nbsp;</b></legend>");
      
      str += F("<p><b>Rapporteer aan Domoticz?</b><input type='checkbox' class='form-control' name='domo' id='domo' ");
      
       if (user_data.domo[0] =='j') str += F(" checked></p>"); else str += F("></p>");
       str += F("<p><b>Domoticz IP address</b><input type='text' class='form-control' name='domoticzIP' value='");
       str += user_data.domoticzIP;
       str += F("'></p><p>");
       str += F("<b>Domoticz Port</b><input type='text' class='form-control' name='domoticzPort' value='");
       str += user_data.domoticzPort;
       str += F("'></p><p>");
       str += F("<b>Domoticz Gas Idx</b><input type='text' class='form-control' name='domoticzGasIdx' value='");
       str += user_data.domoticzGasIdx;
       str += F("'></p><p>");
       str += F("<b>Domoticz Energy Idx</b><input type='text' class='form-control' name='domoticzEnergyIdx' value='");
       str += user_data.domoticzEnergyIdx;
       str += F("'></p>");
      str += F("</fieldset>");

       str += F("<fieldset><legend><b>&nbsp;MQTT parameters&nbsp;</b></legend>");
      str += F("<p><b>Rapporteer aan MQTT broker?</b><input type='checkbox' class='form-control' name='mqtt' id='mqtt' ");
       if (user_data.mqtt[0] =='j') str += F(" checked></p>"); else str += F("></p>");
     str += F("<p><b>MQTT broker IP address</b><input type='text' class='form-control' name='mqttIP' value='");
       str += user_data.mqttIP;
       str += F("'></p><p>");
       str += F("<b>MQTT broker Port</b><input type='text' class='form-control' name='mqttPort' value='");
       str += user_data.mqttPort;
       str += F("'></p><p>");
       str += F("<b>MQTT user</b><input type='text' class='form-control' name='mqttUser' value='");
       str += user_data.mqttUser;
       str += F("'></p><p>");
       str += F("<b>MQTT password</b><input type='text' class='form-control' name='mqttPass' value='");
       str += user_data.mqttPass;
       str += F("'></p>");
       str += F("<b>MQTT root topic</b><input type='text' class='form-control' name='mqttTopic' value='");
       str += user_data.mqttTopic;
       str += F("'></p>");
       str += F("</fieldset>");
       str += F("<fieldset>");
       str += F("<b>Meetinterval in sec (> 10 sec)</b><input type='text' class='form-control' name='interval' value='");
       str += user_data.interval; 
       str += F("'></p><p>");
       str += F("<p><b>Rapporteer in watt (ipv van in kWh) </b><input type='checkbox' class='form-control' name='watt' id='watt' ");
       if (user_data.watt[0] =='j') str += F(" checked></p>"); else str += F("></p>");
       str += F("<p><b>Activeer Telnet poort (23) </b><input type='checkbox' class='form-control' name='telnet' id='telnet' ");
       if (user_data.telnet[0] =='j') str += F(" checked></p>"); else str += F("></p>");
       str += F("<p><b>Debug via MQTT </b><input type='checkbox' class='form-control' name='debug' id='debug' ");
       if (user_data.debug[0] =='j') str += F(" checked></p>"); else str += F("></p>");
      str += F("</fieldset><div></div>");
      str += F("<p><button type='submit'>Save</button></form>");
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
    addHead(str);
    addIntro(str);
  //  str += ("<p>");
  //  str += P1timestamp;
   // str += P1timestamp[7];
   // str += P1timestamp[8];
   // str += P1timestamp[9];
 // str += ("</p>");
  str += F("<form ><fieldset><legend><b>Meetwaarden</b></legend>");
 // str += F("<form action='/' method='post'>");

  str += "<p><div class='row'><div class='column'><b>Verbruik laag tarief: totaal</b><input type='text' class='form-control c6' value='";
  str += electricityUsedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input type='text' class='form-control c7' value='";
  str += atof(electricityUsedTariff1) - atof(dayStartUsedT1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Verbruik hoog tarief: totaal</b><input type='text' class='form-control c6' value='";
  str += electricityUsedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input type='text' class='form-control c7' value='";
  str += atof(electricityUsedTariff2) - atof(dayStartUsedT2);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>Retour laag tarief: totaal</b><input type='text' class='form-control c6' value='";
  str += electricityReturnedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input type='text' class='form-control c7' value='";
  str += atof(electricityReturnedTariff1) - atof(dayStartReturnedT1);
  str += eenheid;
  str += "</div></p>";
  
  str += "<p><div class='row'><div class='column'><b>Retour hoog tarief: totaal</b><input type='text' class='form-control c6' value='";
  str += electricityReturnedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input type='text' class='form-control c7' value='";
  str += atof(electricityReturnedTariff2) - atof(dayStartReturnedT2);
  str += eenheid;
  str += "</div></p>";
  

  str += "<p><div class='row'><b>Actueel verbruik</b><input type='text' class='form-control c6' value='";
  str += actualElectricityPowerDelivered;
  str += eenheid2;


 str += "<p><div class='row'><b>Actuele teruglevering</b><input type='text' class='form-control c6' value='";
  str += actualElectricityPowerReturned;
  str += eenheid2;

  str += "<p><div class='row'><div class='column3'><b>Voltage: L1</b><input type='text' class='form-control c6' value='";
  str += instantaneousVoltageL1;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><b>L2</b><input type='text' class='form-control c7' value='";
  str += instantaneousVoltageL2;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><b>L3</b><input type='text' class='form-control c7' value='";
   str += instantaneousVoltageL2;
    str += " V'></div></div></p>";

       str += "<p><div class='row'><div class='column3'><b>Amperage: L1</b><input type='text' class='form-control c6' value='";
  str += instantaneousCurrentL1;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><b>L2</b><input type='text' class='form-control c7' value='";
  str += instantaneousCurrentL2;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><b>L3</b><input type='text' class='form-control c7' value='";
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
  str += "<p><div class='row'><div class='column'><b>Gasverbruik: totaal</b><input type='text' class='form-control c6' value='";
  str += gasReceived5min;
  str += F(" m3'></div>");
  str += "<div class='column' style='text-align:right'><b>vandaag</b><input type='text' class='form-control c7' value='";
  str += atof(gasReceived5min) - atof(dayStartGaz);
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

void handleHelp(){
    char ipstr[20];
  IPAddress ip = WiFi.localIP();
  sprintf_P(ipstr, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
 String str;
 addHead(str);
  addIntro(str);
  str += F("<fieldset>");
  str += F("<fieldset><legend><b>Help</b></legend>");
  str += F("<p><b>De P1 wifigateway kan op 4 manieren data afleveren.</b><br><br>");
  str += F("<p>Altijd beschikbaar zijn de kernwaarden via het P1 menu (of via ");
  str += ipstr;
  str += F("/P1, of via p1wifi.local/P1 )</p><br>");
  
  str += F("<p>De ruwe data is beschikbaar via ");
  str += ipstr;
  str += F("/Data, of via p1wifi.local/Data</p><br><br>");
  str += F("<p>Meer gangbaar is het gebruik van de gateway met een domotica systeem.</p>");
  str += F("<p><b>Domoticz</b> accepteert json berichten. Geef hiertoe het IP adres van <br>");
  str += F("uw Domoticz server op en het poortnummer waarop deze kan worden benaderd (doorgaans 8080).</p>");
  str += F("De benodigde Idx voor gas en electra verkrijgt u door eerst in Domoticz virtule sensors ");
  str += F("voor beiden aan te maken. Na creatie verschijnen de Idxen onder het tabblad 'devices'.</p><br>");
  str +=F("");
  str +=F("Data kan ook (door Domoticz bijvoorbeeld) worden uitgelezen via poort 23 van de module (p1wifi.local:23).");
  str +=F("Dit is de manier waarop Domoticz hardware device [P1 Smart Meter with LAN interface] data kan ophalen.");
  str +=F("Aan de p1wifi kant hoeft daarvoor niets te worden ingesteld (geen vinkjes bij json en mqtt). ");

  str += F("Voor andere systemen kunt u gebruik maken van een MQTT broker. Vul de gegevens van de ");
  str += F("broker in, en stel het root topic in. Voor Home Assistant is dat 'sensors/power/p1meter'.</p>");
  str += F("Daniel de Jong beschijft op zijn <a href='https://github.com/daniel-jong/esp8266_p1meter'>github</a> hoe u HA verder configureert.</p>");
  str += F("Geef met de checkboxes aan welke rapportage methode(n) u wilt gebruiken.</p>");

  str += F("</fieldset></p>");
  str += F("<div style='text-align:right;font-size:11px;'><hr/><a href='http://eps8266thingies.nl' target='_blank' style='color:#aaa;'>eps8266thingies.nl</a></div></div></fieldset></body></html>");
server.send(200, "text/html", str);
}
#endif
