/*
 * Copyright (c) 2023 Jean-Pierre sneyers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Additionally, please note that the original source code of this file
 * may contain portions of code derived from (or inspired by)
 * previous works by:
 *
 * Ronald Leenes (https://github.com/romix123/P1-wifi-gateway and http://esp8266thingies.nl)
 */

#include "HTTPMgr.h"

HTTPMgr::HTTPMgr(settings &currentConf, TelnetMgr &currentTelnet, MQTTMgr &currentMQTT, P1Reader &currentP1) : conf(currentConf), TelnetSrv(currentTelnet), MQTT(currentMQTT), P1Captor(currentP1), server(80)
{
}

void HTTPMgr::start_webservices()
{
  SendDebugPrintf("[WWW] Start on port %d", WWW_PORT_HTTP);
  MDNS.begin(HOSTNAME);

  //  Flash OTA
  httpUpdater.setup(&server);

  server.on("/style.css", std::bind(&HTTPMgr::handleStyleCSS, this));
  server.on("/", std::bind(&HTTPMgr::handleRoot, this));
  server.on("/welcome", std::bind(&HTTPMgr::handleWelcome, this));
  server.on("/Setup", std::bind(&HTTPMgr::handleSetup, this));
  server.on("/SetupSave", std::bind(&HTTPMgr::handleSetupSave, this));
  server.on("/update", HTTP_GET, std::bind(&HTTPMgr::handleUploadForm, this));
  server.on("/reset", std::bind(&HTTPMgr::handleFactoryReset, this));
  server.on("/P1", std::bind(&HTTPMgr::handleP1, this));
  server.on("/Help", std::bind(&HTTPMgr::handleHelp, this));

  server.begin();
  MDNS.addService("http", "tcp", WWW_PORT_HTTP);
}

void HTTPMgr::DoMe()
{
  server.handleClient(); // handle web requests
  MDNS.update();
}

bool HTTPMgr::ChekifAsAdmin()
{
  if (strlen(conf.adminPassword) != 0)
  {
    if (!server.authenticate(conf.adminUser, conf.adminPassword))
    {
      server.requestAuthentication(DIGEST_AUTH, "Hello", "you fail!");
      //server.requestAuthentication();
      return false;
    }
  }
  return true;
}

void HTTPMgr::handleRoot()
{
  // You cannot use this page if is not your first boot
  if (conf.NeedConfig)
  {
    server.sendHeader("Location", "/welcome");
    server.send(302, "text/plain", "Redirecting");
    return;
  }

  String str = F("<main class='form-signin'>");
  str += F("<fieldset><legend>{-H1DATA-}</legend>");
  str += F("<form action='/P1' method='post'><button type='p1' class='bhome'>{-MENUP1-}</button></form>");
  str += F("</fieldset>");
  str += F("<fieldset><legend>{-ConfH1-}</legend>");
  str += F("<form action='/Setup' method='post'><button type='Setup'>{-MENUConf-}</button></form>");
  str += F("<form action='/update' method='GET'><button type='submit'>{-MENUOTA-}</button></form>");
  str += F("<form action='/reset' id=\"frmRst\" method='GET'><button type='button' onclick='ConfRST()'>{-MENURESET-}</button></form>");
  str += F("<script> function ConfRST() { if (confirm(\"{-ASKCONFIRM-}\")) { document.getElementById(\"frmRst\").submit();}}</script>");
  TradAndSend(200, "text/html", str, 0);
}

void HTTPMgr::ReplyErrorLogin(const String Where)
{
  SendDebug("[HTTP] Get wrong password from an client.");

  String str = F("<fieldset><legend>{-H1WRONGPSD-}</legend>");
  str += F("<p><b>{-WRONGPSDTXT-}</b><br>");
  str += F("</fieldset>");
  str += F("<form action='");
  str += Where;
  str += F("' method='POST'><button class='bhome'>{-WRONGPSDBACK-}</button></form></p>");
  TradAndSend(401, "text/html", str, 0);
}

void HTTPMgr::ReplyOTAOK()
{
  String str = F("<fieldset><p>{-OTASUCCESS1-}</p><p>{-OTASUCCESS2-}</p><p>{-OTASUCCESS3-}</p><p>{-OTASUCCESS4-}</p><p>{-OTASUCCESS5-}</p></fieldset>");
  TradAndSend(200, "text/html", str, 30);
  delay(2000);
}

void HTTPMgr::handleStyleCSS()
{
  SendDebug("[HTTP] Request style.css");

  String str = F("body {text-align: center; font-family: verdana, sans-serif; background: #ffffff;}");
  str += F("h2 {text-align:center;color:#000000;}");
  str += F("div, fieldset, input, select {padding: 5px; font-size: 1em}");
  str += F("fieldset {background: #ECEAE4;margin-bottom: 20px}");
  str += F("legend {font-weight: bold;}");
  str += F("label {display: inline-block;width:50%;text-align: right;}");
  str += F(".help, .footer {text-align:right;font-size:11px;color:#aaa}");
  str += F("p {margin: 0.5em 0;}");
  str += F("input {box-sizing: border-box; -webkit-box-sizing: border-box; -moz-box-sizing: border-box; background: #ffffff; color: #000000;}");
  str += F("input[type=range] {width: 90%;}");
  str += F("select {background: #ffffff; color: #000000;}");
  str += F("textarea {resize: vertical; width: 98%; height: 318px; padding: 5px; overflow: auto; background: #ffffff; color: #000000;}");
  str += F("button {border: 0; border-radius: 0.3rem; background: #97C1A9; color: #ffffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer;}");
  str += F("button:hover {background: #0e70a4;}");
  str += F(".bhome {background: #55CBCD;}");
  str += F(".bhome:hover {background: #A2E1DB;}");
  str += F("a {color: #1fa3ec;text-decoration: none;}");
  str += F(".column {float: left;width: 48%;}");
  str += F(".column3 {float: left; width: 31%;}");
  str += F(".row:after {content: \"\";display: table; clear: both;}");
  str += F("input.c6 {text-align:right}");
  str += F("input.c7 {text-align:right; color:#97C1A9}");

  // Cache pendant 10 heure (en secondes)
  server.sendHeader("Cache-Control", "max-age=36000");
  // no translate for CSS
  server.send(200, "text/css", str);
}

void HTTPMgr::handleUploadForm()
{
  if (!ChekifAsAdmin())
  {
    return;
  }

  String str = F("<fieldset><legend>{-OTAH1-}</legend>");
  str += F("<form action='' method='post'><form method='POST' action='' enctype='multipart/form-data'>");
  str += F("<p><b>{-OTAFIRMWARE-}</b><input type='file' accept='.bin,.bin.gz' name='firmware'></p>");
  str += F("<button type='submit'>{-OTABTUPDATE-}</button>");
  str += F("</form>");
  str += F("<form action='/' method='POST'><button class='bhome'>{-MENU-}</button></form></fieldset>");
  TradAndSend(200, "text/html", str, 0);
}

void HTTPMgr::handleUploadResult()
{
  SendDebug("[WWW] handleUploadResult");
  if (ChekifAsAdmin())
  {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  }
}

void HTTPMgr::handleUploadFlash()
{
  SendDebug("[WWW] handleUploadFlash");
  if (ChekifAsAdmin())
  {
    SendDebug("[WWW] handleUploadFlash-1");
    HTTPUpload &upload = server.upload();
    SendDebug("[WWW] handleUploadFlash-2");
    SendDebug("[WWW] handleUploadFlash-3");
    if (upload.status == UPLOAD_FILE_START)
    {
      SendDebug("[WWW] Start Flash !!!");

      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      SendDebugPrintf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) // start with max available size
      {
        Update.printError(Serial);
      }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
      SendDebug("[WWW] UPLOAD_FILE_WRITE");
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      {
        SendDebug("[WWW] UPLOAD_FILE_WRITE ERROR");
        Update.printError(Serial);
      }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
      SendDebug("[WWW] UPLOAD_FILE_END");
      if (Update.end(true)) // true to set the size to the current progress
      {
        SendDebugPrintf("Update Success: %u\nRebooting...\n", upload.totalSize);
        ReplyOTAOK();
        ESP.restart();
      }
      else
      {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    SendDebug("[WWW] OTA OUT");
    yield();
  }
}

void HTTPMgr::handleFactoryReset()
{
  if (!ChekifAsAdmin())
  {
    return;
  }

  String str = F("<fieldset><legend>{-RF_RESTARTH1-}</legend>");
  str += F("<p>{-RF_RESTTXT-}</p>");
  str += F("<form action='/' method='POST'><button class='bhome'>{-MENU-}</button></form></p>");
  str += F("</fieldset>");
  TradAndSend(200, "text/html", str, 0);

  conf.ConfigVersion = SETTINGVERSIONNULL;

  EEPROM.put(0, conf);
  EEPROM.commit();

  delay(500);
  ESP.reset();
}

void HTTPMgr::handleWelcome()
{
  // You cannot use this page if is not your first boot
  if (!conf.NeedConfig)
  {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Redirecting");
    return;
  }

  // Is the new password ?
  if (server.method() == HTTP_POST && server.hasArg("psd1") && server.hasArg("psd2"))
  {
    if (server.arg("psd1") == server.arg("psd2"))
    {
      conf.NeedConfig = false;
      server.arg("psd1").toCharArray(conf.adminPassword, sizeof(conf.adminPassword));
      server.arg("adminUser").toCharArray(conf.adminUser, sizeof(conf.adminUser));

      SendDebug("[HTTP] New admin password");
      EEPROM.put(0, conf);
      EEPROM.commit();

      // Move to full setup !
      server.sendHeader("Location", "/Setup");
      server.send(302, "text/plain", "Redirecting");
      return;
    }
  }

  String str = F("<form action=\"/welcome\" method=\"POST\" onsubmit=\"return Check()\"><fieldset>");
  str += F("<fieldset><legend>{-H1Welcome-}</legend>");
  str += F("<label for=\"adminUser\">{-PSWDLOGIN-} :</label><input type=\"text\" name=\"adminUser\" maxlength=\"32\"><br />");
  str += F("<label for=\"psd1\">{-PSWD1-} :</label><input type=\"password\" name=\"psd1\" maxlength=\"32\"><br />");
  str += F("<label for=\"psd2\">{-PSWD2-} :</label><input type=\"password\" name=\"psd2\" maxlength=\"32\"><br />");
  str += F("<span id=\"passwordError\" class=\"error\"></span>");
  str += F("<button type='submit'>{-ACTIONLOGIN-}</button></form>");
  str += F("<script>function Check() {if (document.getElementById(\"psd1\").value !== document.getElementById(\"psd2\").value) {document.getElementById(\"passwordError\").innerHTML = \"{-PSWDERROR-}\";return false;}return true;}</script>");
  str += F("</fieldset>");
  TradAndSend(200, "text/html", str, 0);
}

void HTTPMgr::handleSetup()
{
  if (!ChekifAsAdmin())
  {
    return;
  }

  String str = F("<form action='/SetupSave' method='POST' onsubmit=\"return Check()\">");
  str += F("<fieldset><legend>{-H1Welcome-}</legend>");
  str += F("<label for=\"adminUser\">{-PSWDLOGIN-} :</label><input type=\"text\" name=\"adminUser\" maxlength=\"32\" value='");
  str += conf.adminUser;
  str += F("'><br />");
  str += F("<label for=\"psd1\">{-PSWD1-} :</label><input type=\"password\" name=\"psd1\" maxlength=\"32\"><br />");
  str += F("<label for=\"psd2\">{-PSWD2-} :</label><input type=\"password\" name=\"psd2\" maxlength=\"32\"><br />");
  str += F("<script>function Check() {if (document.getElementById(\"psd1\").value !== document.getElementById(\"psd2\").value) {document.getElementById(\"passwordError\").innerHTML = \"{-PSWDERROR-}\";return false;}return true;}</script>");
  str += F("</fieldset>");

  str += F("<fieldset><legend>{-ConfWIFIH2-}</legend>");
  str += F("<label for=\"ssid\">{-ConfSSID-} :</label><input type='text' name='ssid' maxlength=\"32\" value='");
  str += conf.ssid;
  str += F("'><br />");
  str += F("<label for=\"password\">{-ConfWIFIPWD-} :</label><input type='password' maxlength=\"64\" name='password' value='");
  str += conf.password;
  str += F("'><br />");
  str += F("</fieldset>");

  str += F("<fieldset><legend>{-ConfDMTZH2-}</legend>");
  str += F("<label for=\"domo\">{-ConfDMTZBool-} :</label><input type='checkbox' name='domo' id='domo' ");

  if (conf.domo)
  {
    str += F(" checked><br />");
  }
  else
  {
    str += F("><br />");
  }
  str += F("<label for=\"domoticzIP\">{-ConfDMTZIP-} :</label><input type='text' name='domoticzIP' maxlength=\"29\" value='");
  str += conf.domoticzIP;
  str += F("'><br />");
  str += F("<label for=\"domoticzPort\">{-ConfDMTZPORT-} :</label><input type='number' min='1' max='65535' name='domoticzPort' value='");
  str += conf.domoticzPort;
  str += F("'><br />");
  str += F("<label for=\"domoticzGasIdx\">{-ConfDMTZGIdx-} :</label><input type='number' min='1' name='domoticzGasIdx' value='");
  str += conf.domoticzGasIdx;
  str += F("'><br />");
  str += F("<label for=\"domoticzEnergyIdx\">{-ConfDMTZEIdx-} :</label><input type='number' min='1' name='domoticzEnergyIdx' value='");
  str += conf.domoticzEnergyIdx;
  str += F("'>");
  str += F("</fieldset>");

  str += F("<fieldset><legend>{-ConfMQTTH2-}</legend>");
  str += F("<label for=\"mqtt\">{-ConfMQTTBool-} :</label><input type='checkbox' name='mqtt' id='mqtt' ");
  if (conf.mqtt)
  {
    str += F(" checked></p>");
  }
  else
  {
    str += F("><br />");
  }

  str += F("<label for=\"mqttIP\">{-ConfMQTTIP-} :</label><input type='text' name='mqttIP' maxlength=\"29\" value='");
  str += conf.mqttIP;
  str += F("'><br />");
  str += F("<label for=\"mqttPort\">{-ConfMQTTPORT-} :</label><input type='number' min='1' max='65535' name='mqttPort' value='");
  str += conf.mqttPort;
  str += F("'><br />");
  str += F("<label for=\"mqttUser\">{-ConfMQTTUsr-} :</label><input type='text' name='mqttUser' maxlength=\"31\" value='");
  str += conf.mqttUser;
  str += F("'><br />");
  str += F("<label for=\"mqttPass\">{-ConfMQTTPSW-} :</label><input type='password' name='mqttPass' maxlength=\"31\" value='");
  str += conf.mqttPass;
  str += F("'><br />");
  str += F("<label for=\"mqttTopic\">{-ConfMQTTRoot-} :</label><input type='text' name='mqttTopic' maxlength=\"49\" value='");
  str += conf.mqttTopic;
  str += F("'><br />");
  str += F("<label for=\"interval\">{-ConfMQTTIntr-} :</label><input type='number' min='10' name='interval' value='");
  str += conf.interval;
  str += F("'><br />");
  str += F("<label for=\"watt\">{-ConfMQTTKW-} :</label><input type='checkbox' name='watt' id='watt' ");
  if (conf.watt)
  {
    str += F(" checked></p>");
  }
  else
  {
    str += F("><br />");
  }
  str += F("<label for=\"telnet\">{-ConfTLNETBool-} :</label><input type='checkbox' name='telnet' id='telnet' ");
  if (conf.telnet)
  {
    str += F(" checked><br />");
  }
  else
  {
    str += F("><br />");
  }
  str += F("<label for=\"debugToTelnet\">{-ConfTLNETDBG-} :</label><input type='checkbox' name='debugToTelnet' id='debugToTelnet' ");
  if (conf.debugToTelnet)
  {
    str += F(" checked><br />");
  }
  else
  {
    str += F("><br />");
  }
  str += F("<label for=\"debugToMqtt\">{-ConfMQTTDBG-} :</label><input type='checkbox' name='debugToMqtt' id='debugToMqtt' ");
  if (conf.debugToMqtt)
  {
    str += F(" checked><br />");
  }
  else
  {
    str += F("><br />");
  }
  str += F("</fieldset>");
  str += F("<span id=\"passwordError\" class=\"error\"></span>");
  str += F("<button type='submit'>{-ACTIONSAVE-}</button></form>");
  str += F("<form action='/' method='POST'><button class='bhome'>{-MENU-}</button></form>");
  TradAndSend(200, "text/html", str, 0);
}

void HTTPMgr::handleSetupSave()
{
  if (!ChekifAsAdmin())
  {
    return;
  }

  SendDebug("[WWW] Save new setup.");

  if (server.method() == HTTP_POST)
  {
    settings NewConf;
    NewConf.NeedConfig = false;
    // TODO : check id psd1 == psd2
    server.arg("psd1").toCharArray(NewConf.adminPassword, sizeof(NewConf.adminPassword));
    server.arg("adminUser").toCharArray(conf.adminUser, sizeof(conf.adminUser));
    server.arg("ssid").toCharArray(NewConf.ssid, sizeof(NewConf.ssid));
    server.arg("password").toCharArray(NewConf.password, sizeof(NewConf.password));
    server.arg("domoticzIP").toCharArray(NewConf.domoticzIP, sizeof(NewConf.domoticzIP));
    NewConf.domoticzPort = server.arg("domoticzPort").toInt();
    NewConf.domoticzEnergyIdx = server.arg("domoticzEnergyIdx").toInt();
    NewConf.domoticzGasIdx = server.arg("domoticzGasIdx").toInt();
    NewConf.mqtt = (server.arg("mqtt") == "on");
    NewConf.domo = (server.arg("domo") == "on");

    server.arg("mqttIP").toCharArray(NewConf.mqttIP, sizeof(NewConf.mqttIP));
    NewConf.mqttPort = server.arg("mqttPort").toInt();

    server.arg("mqttUser").toCharArray(NewConf.mqttUser, sizeof(NewConf.mqttUser));
    server.arg("mqttPass").toCharArray(NewConf.mqttPass, sizeof(NewConf.mqttPass));
    server.arg("mqttTopic").toCharArray(NewConf.mqttTopic, sizeof(NewConf.mqttTopic));

    NewConf.interval = server.arg("interval").toInt();
    NewConf.watt = (server.arg("watt") == "on");
    NewConf.telnet = (server.arg("telnet") == "on");
    NewConf.debugToTelnet = (server.arg("debugToTelnet") == "on");
    NewConf.debugToMqtt = (server.arg("debugToMqtt") == "on");

    NewConf.ConfigVersion = SETTINGVERSION;

    SendDebug("[HTTP] save in EEPROM !!!");

    String str = F("<fieldset><legend>{-ConfH1-}</legend>");
    str += F("<p>{-Conf-Saved-}</p>");
    str += F("<p>{-ConfReboot-}</p>");
    str += F("<p></p>");
    str += F("<p>{-ConfLedStart-}</p>");
    str += F("<p>{-ConfLedError-}</p>");
    str += F("</fieldset>");
    str += F("<form action='/' method='POST'><button class='bhome'>{-MENU-}</button></form></p>");
    TradAndSend(200, "text/html", str, 0);

    EEPROM.put(0, NewConf);
    EEPROM.commit();

    SendDebug("[HTTP] Reboot !!!");
    delay(500);
    ESP.reset();
  }
}

void HTTPMgr::handleP1()
{
  String eenheid, eenheid2, eenheid3;
  if (conf.watt)
    eenheid = " kWh'></div>";
  else
    eenheid = " Wh'></div>";
  if (conf.watt)
  {
    eenheid2 = " kW'></div></p>";
  }
  else
  {
    eenheid2 = " W'></div></p>";
  }

  String str = F("<form ><fieldset><legend>{-DATAH1-}</legend>");

  str += "<p><div class='row'><div class='column'><b>{-DATAFullL-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.electricityUsedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>{-DATATODAY-}</b><input type='text' class='c7' value='";
  str += atof(P1Captor.DataReaded.electricityUsedTariff1); // - atof(Logger.log_data.dayE1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>{-DATAFullH-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.electricityUsedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>{-DATATODAY-}</b><input type='text' class='c7' value='";
  str += atof(P1Captor.DataReaded.electricityUsedTariff2); // - atof(Logger.log_data.dayE2);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>{-DATAFullProdL-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.electricityReturnedTariff1;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>{-DATATODAY-}</b><input type='text' class='c7' value='";
  str += atof(P1Captor.DataReaded.electricityReturnedTariff1); // - atof(Logger.log_data.dayR1);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><div class='column'><b>{-DATAFullProdH-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.electricityReturnedTariff2;
  str += eenheid;
  str += "<div class='column' style='text-align:right'><br><b>{-DATATODAY-}</b><input type='text' class='c7' value='";
  str += atof(P1Captor.DataReaded.electricityReturnedTariff2); // - atof(Logger.log_data.dayR2);
  str += eenheid;
  str += "</div></p>";

  str += "<p><div class='row'><b>{-DATACurAmp-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.actualElectricityPowerDeli;
  str += eenheid2;

  str += "<p><div class='row'><b>{-DATACurProdAmp-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.actualElectricityPowerRet;
  str += eenheid2;

  str += "<p><div class='row'><div class='column3'><b>{-DATAUL1-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.instantaneousVoltageL1;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><b>{-DATAUL2-}</b><input type='text' class='c7' value='";
  str += P1Captor.DataReaded.instantaneousVoltageL2;
  str += " V'></div>";
  str += "<div class='column3' style='text-align:right'><b>{-DATAUL3-}</b><input type='text' class='c7' value='";
  str += P1Captor.DataReaded.instantaneousVoltageL2;
  str += " V'></div></div></p>";

  str += "<p><div class='row'><div class='column3'><b>{-DATAAL1-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.instantaneousCurrentL1;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><b>{-DATAAL2-}</b><input type='text' class='c7' value='";
  str += P1Captor.DataReaded.instantaneousCurrentL2;
  str += " A'></div>";
  str += "<div class='column3' style='text-align:right'><b>{-DATAAL3-}</b><input type='text' class='c7' value='";
  str += P1Captor.DataReaded.instantaneousCurrentL3;
  str += " A'></div></div></p>";
  /*

    str += F("<p><b>Voltage dips</b><input type='text' style='text-align:right' value='");
    str += numberVoltageSagsL1;
    str += F("'></p>");
    str += F("<p><b>Voltage pieken</b><input type='text' style='text-align:right' value='");
    str += numberVoltageSwellsL1;
    str += F("'></p>");
    str += F("<p><b>Stroomonderbrekingen</b><input type='text' style='text-align:right' value='");
    str += numberPowerFailuresAny;
    str += F("'></p>");
    */
  str += "<p><div class='row'><div class='column'><b>{-DATAGFull-}</b><input type='text' class='c6' value='";
  str += P1Captor.DataReaded.gasReceived5min;
  str += F(" m3'></div>");
  str += "<div class='column' style='text-align:right'><b>{-DATATODAY-}</b><input type='text' class='c7' value='";
  str += atof(P1Captor.DataReaded.gasReceived5min); // - atof(Logger.log_data.dayG);
  str += " m3'></div></div></p>";
  str += F("</fieldset></form>");
  str += F("<form action='/' method='POST'><button class='bhome'>{-MENU-}</button></form>");
  TradAndSend(200, "text/html", str, 60);
}

void HTTPMgr::handleHelp()
{
  String str = F("<fieldset><legend>{-HLPH1-}</legend>");
  str += F("<p>{-HLPTXT1-}</p>");
  str += F("<p>{-HLPTXT2-}</p>");
  str += F("<p>{-HLPTXT3-}</p>");
  str += F("<p>{-HLPTXT4-}</p>");
  str += F("<p>{-HLPTXT5-}</p>");
  str += F("<p>{-HLPTXT6-}</p>");
  str += F("<p>{-HLPTXT7-}</p>");
  TradAndSend(200, "text/html", str, 0);
}

void HTTPMgr::TradAndSend(int code, const char *content_type, String content, int refresh)
{
  // HEADER
  String str = F("<!DOCTYPE html><html lang='{-HEADERLG-}'>");

  if (refresh > 0)
  {
    str += F("<META http-equiv='refresh' content='");
    str += String(refresh);
    str += F(";URL=/'>");
  }

  str += F("<meta charset='utf-8'><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>");
  str += F("<head><title>Slimme meter</title>");
  str += F("<link rel='stylesheet' type='text/css' href='style.css'></head>");
  str += F("<body><div style='text-align:left;display:inline-block;color:#000000;width:600px;'><h2>P1 wifi-gateway</h2>");
  str += F("<p class=\"help\"><a href='/Help' target='_blank'>{-HLPH1-}</a>");

  // CONTENT
  str += content;

  // FOOTER
  str += F("<div class=\"footer\">");
  if (conf.mqtt)
  {
    if (MQTT.IsConnected())
    {
      str += F("MQTT link: Connected ");
    }
    else
    {
      str += F("MQTT link: Not connected ");
    }
  }
  str += F("{-OTAFIRMWARE-} : ");
  str += F(VERSION);
  str += F("<br><a href='https://github.com/narfight/P1-wifi-gateway' target='_blank'>Github</a>");
  str += F("</div></div></body></html>");

  Trad.FindAndTranslateAll(str);
  server.send(code, content_type, str);
}