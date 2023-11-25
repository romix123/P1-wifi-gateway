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

#define FRENCH // NEDERLANDS,SWEDISH,GERMAN,FRENCH

#include <Arduino.h>
#include <EEPROM.h>
#include "GlobalVar.h"
#include "Language.h"

unsigned long WatchDogsTimer = 0;

settings config_data;

#include "WifiMgr.h"
WifiMgr *WifiClient;

#include "MQTT.h"
MQTTMgr *MQTTClient;

#include "TelnetMgr.h"
TelnetMgr *TelnetServer;

#include "P1Reader.h"
P1Reader *DataReaderP1;

#include "JSONMgr.h"
JSONMgr *JSONClient;

#include "HTTPMgr.h"
HTTPMgr *HTTPClient;

ADC_MODE(ADC_VCC); // allows you to monitor the internal VCC level;

void SendDebug(String payload)
{
  if (MQTTClient != nullptr)
  {
    MQTTClient->SendDebug(payload);
  }
  if (TelnetServer != nullptr)
  {
    TelnetServer->SendDebug(payload);
  }
  Serial.println(payload);
}

void SendDebugPrintf(const char *format, ...)
{
  const int bufferSize = 100; // Définir la taille du tampon pour stocker le message formaté
  char buffer[bufferSize];

  // Utiliser la liste d'arguments variables
  va_list args;
  va_start(args, format);

  // Formater la chaîne avec vsnprintf
  int length = vsnprintf(buffer, bufferSize, format, args);

  va_end(args);

  // Vérifier si le formatage a réussi et imprimer le message
  if (length >= 0 && length < bufferSize)
  {
    SendDebug(buffer);
  }
  else
  {
    SendDebug("Erreur de formatage du message de débogage.");
  }
}

void EventOnWifi(wl_status_t from, wl_status_t to)
{
  SendDebugPrintf("[WIFI] Event %s -> %s", WifiClient->StatusIdToString(from), WifiClient->StatusIdToString(to));
}

void blink(int t)
{
  for (int i = 0; i <= t; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    ;
  }
}

void alignToTelegram()
{
  // make sure we don't drop into the middle of a telegram on boot. Read whatever is in the stream until we find the end char !
  // then read until EOL and flsuh serial, return to loop to pick up the first complete telegram.

  if (Serial.available() > 0)
  {
    while (Serial.available())
    {
      int inByte = Serial.read();
      if (inByte == '!')
      {
        break;
      }
    }

    char buf[10];
    Serial.readBytesUntil('\n', buf, 9);
    Serial.flush();
  }
}

void PrintConfigData()
{
  SendDebug("Current configuration :");
  SendDebugPrintf(" - ConfigVersion : %d", config_data.ConfigVersion);
  // SendDebugPrintf(" - Admin password : %s", config_data.adminPassword);
  SendDebugPrintf(" - SSID : %s", config_data.ssid);
  // SendDebugPrintf(" - Wifi password : %s", config_data.password);
  SendDebugPrintf(" - Domoticz Actif : %s", (config_data.domo) ? "Y" : "N");
  SendDebugPrintf("   # Domoticz : %s:%u", config_data.domoticzIP, config_data.domoticzPort);
  SendDebugPrintf("   # DomotixzGasIdx : %u", config_data.domoticzGasIdx);
  SendDebugPrintf("   # DomotixzEnergyIdx : %u", config_data.domoticzEnergyIdx);
  SendDebugPrintf(" - MQTT Actif : %s", (config_data.mqtt) ? "Y" : "N");
  SendDebugPrintf("   # Send debug here : %s", (config_data.debugToMqtt) ? "Y" : "N");
  SendDebugPrintf("   # MQTT : mqtt://%s:***@%s:%u", config_data.mqttUser, config_data.mqttIP, config_data.mqttPort);
  SendDebugPrintf("   # MQTT Topic : %s", config_data.mqttTopic);
  SendDebugPrintf(" - interval : %u", config_data.interval);
  SendDebugPrintf(" - P1 In watt : %s", (config_data.watt) ? "Y" : "N");
  SendDebugPrintf(" - TELNET Actif : %s", (config_data.telnet) ? "Y" : "N");
  SendDebugPrintf("   # Send debug here : %s", (config_data.debugToTelnet) ? "Y" : "N");
  delay(500);
}

void setup()
{
  Serial.begin(115200);
  delay(50);
  Serial.println("Booting...");
  SendDebugPrintf("Firmware: v%s", VERSION);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(OE, OUTPUT);    // IO16 OE on the 74AHCT1G125
  digitalWrite(OE, HIGH); // Put(Keep) OE in Tristate mode
  pinMode(DR, OUTPUT);    // IO4 Data Request
  digitalWrite(DR, LOW);  // DR low (only goes high when we want to receive data)

  blink(2);
  SendDebug("Load configuration from EEprom");

  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, config_data);

  // Si la version de la configuration n'est celle attendu, on recet !
  if (config_data.ConfigVersion != SETTINGVERSION)
  {
    SendDebugPrintf("Config file version is wrong (wanted:%d actual:%d)", SETTINGVERSION, config_data.ConfigVersion);
    config_data = (settings){SETTINGVERSION, true, "ssid", "password", "192.168.1.12", 8080, 1234, 1235, "sensors/power/p1meter", "192.168.1.12", 1883, "", "", 30, false, false, false, false, false, false, "adminpwd"};
  }
  PrintConfigData();

  WifiClient = new WifiMgr(config_data);
  MQTTClient = new MQTTMgr(WifiClient->WifiCom, config_data);
  TelnetServer = new TelnetMgr(config_data);
  DataReaderP1 = new P1Reader(config_data);
  HTTPClient = new HTTPMgr(config_data, *TelnetServer, *MQTTClient, *DataReaderP1);
  JSONClient = new JSONMgr(config_data, *DataReaderP1);

  alignToTelegram();
  DataReaderP1->state = WAITING; // signal that we are waiting for a valid start char (aka /)
  WatchDogsTimer = millis();

  WifiClient->OnWifiEvent(EventOnWifi);
  WifiClient->Connect();
  HTTPClient->start_webservices();
}

void doWatchDogs()
{
  if (ESP.getFreeHeap() < 2000) // watchdog, in case we still have a memery leak
  {
    SendDebug("[WDG] FATAL : Memory leak !");
    ESP.reset();
  }

  if (millis() - DataReaderP1->LastSample > 300000)
  {
    Serial.flush();
    SendDebug("[WDG] No data in 300 sec, restarting monitoring");

    DataReaderP1->ResetnextUpdateTime();
  }

  if (WifiClient->AsAP() && (millis() - WifiClient->APtimer > 600000))
  {
    SendDebug("[WDG] No wifi, restart");
    ESP.reset(); // we have been in AP mode for 600 sec.
  }
}

void loop()
{
  WifiClient->DoMe();
  DataReaderP1->DoMe();
  HTTPClient->DoMe();

  if (DataReaderP1->datagramValid && (DataReaderP1->state == DONE) && (WifiClient->WifiCom.status() == WL_CONNECTED))
  {
    if (config_data.mqtt)
    {
      MQTTClient->doMe();
      if (MQTTClient->MqttDelivered)
      {
        MQTTClient->MqttDelivered = false; // reset
      }
    }

    if (config_data.domo)
    {
      JSONClient->DoMe();
    }

    if (config_data.telnet)
    {
      TelnetServer->DoMe(DataReaderP1->datagram);
    }

    DataReaderP1->datagramValid = false; // reset
    DataReaderP1->state = WAITING;
  }

  if (millis() > WatchDogsTimer)
  {
    doWatchDogs();
    WatchDogsTimer = millis() + 22000;
  }
}