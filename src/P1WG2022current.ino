/*
 * P1 wifi gateway 2022
 * 
 * Deze software brengt verschillende componenten bij elkaar in een handzaam pakket.
 * 
 * De software draait op een EPS8285 of ESP8266 en heeft een bescheiden voetafdruk.
 * 
 * De data processing is gebaseerd op: http://domoticx.com/p1-poort-slimme-meter-data-naar-domoticz-esp8266/
 * De captive portal is gebaseerd op een van de ESP8266 voorbeelden, de OTA eenheid eveneens.
 * 
 * De module levert data aan een host via JSON of MQTT messages, maar kan ook via de webinterface van de module of via telnet worden uitgelezen
 * 
 * De module zal bij opstarten eerst de buffercondensator laden. 
 * Vervolgens knippert hij 2x en zal eerst proberen te koppelen aan je wifi netwerk. Gedurende dat proces zal de LED knipperen
 * met een periode van 0.5 Hz.
 * Wanneer geen verbinding kan worden gemaakt start de modiule een Access point onder de naam P1_Setup
 * Verbind met dit netwerk en surf naar 192.168.4.1
 * 
 * Het menuscherm van de module verschijnt dan. Vul daar SSID en wachtwoord van je wifi netwerk in. 
 * Daaronder vul je gegevens van de ontvangede server in (IP en poortnummer). Dit kan je DOmotociz server zijn
 * In dat geval moet je ook het IDx dat Domoticz maakt voor een Gas device en een Energy device invoeren.
 * Tenslotte geef je aan per hoeveel seconde je data naar de server wilt laten sturen.
 * DSMR4 meters hanteren een meetinterval van 10 sec. SMR5 meters leveren iedere seconde data. Dat lijkt me wat veel en 
 * kost de module ook veel stroom waardoor het maar de vraag is of hij parasitair (door de meter gevoed) kan werken.
 * Ik raad een interval van 15 sec of hoger aan. Het interval dat je hier invoert zal niet altijd synchroon lopen met
 * het data interval van de meter, ga er dus niet van uit dat je exact iedere x seconden een meetwaarde ziet in DOmoticz. 
 * Tussen metingen 'slaapt' de module (de modem wordt afgeschakeld waardoor het stroomverbruik van zo'n 70mA terugvalt naar 15 mA). 
 * Dit geeft de bufferelco tijd omm op te laden voor de stroompiekjes die de wifi zender van de module produceert 
 * (en het bespaart hoe dan ook wat stroom (die weliswaar door je energieleverancier wordt betaald, maar toch). Alle kleine 
 * beetjes helpen..
 *
 *  informatie, vragen, suggesties ed richten aan romix@macuser.nl 
 *  
 *  te doen:
 *    check mqtt whether connecction still exists before sending data
 *    
 *  versie: 1.0m 
 *  datum:  18 July 2022
 *  auteur: Ronald Leenes
 *  
 *   *      mqtt reconnect after connection loss
 *  m: setupsave fix, relocate to p1wifi.local na 60 sec 
 *      mqtt - kw/W fix
 *  l: wifireconnect after wifi loss
 *  k: fixed big BUG, softAP would not produce accessible webserver.
 *  j: raw data on port 23
 *      major code rewrite
 *      implemented data reporting methods: 
 *        parsed data: json, mqtt, p1wifi.local/P1
 *        raw data: telnet on port 23, p1wifi.local/Data
 *        
 *  i:  extended max line length for TD210-D meters, which have a really loong 1-0:99.97.0 line
 *  h:  extended mqtt set with instant powers, voltages, actual tarif group. power outages, power drops
 *  g: fixed mqtt
 *  
 *  Generic ESP8285 module 
*   Flash Size: 2mb (FS: 64Kb, OTA: –992Kb) 
*   
*   1. Als ik allen gebruik wil maken van MQTT dan lukt dat wel.
2. Maar het vinkje "rapporteer in Watt" werkt in ieder geval dan precies omgekeerd.
Ofwel: Zonder het vinkje krijg ik in MQTT en op het interface zelf de meterstanden in Watts.
3. En die eenheid moet ik in Domoticz ook hebben. Maar een vinkje bij Domoticz en geen vinkje bij "rapporteer in Watt" wordt niet geaccepteerd. Het vinkje om te rapporteren aan Domoticz is dan na een save gewoon weer weg.
4. Tot slot een slordigheidje:
De url die je krijgt bij het Saven blijft in beeld. Doe je dan, nadat de module weer opgestart is, zoiets als F5, dan start de module gewoon weer opnieuw op. 
Dus je moet op de hand dat stukje SetupSave verwijderen.


 */

String version = "1.0m";
const char* host = "P1wifi";
#define HOSTNAME "p1meter"

#define DEBUG 0 // 1 is on serial only, 2 is serial + telnet
#define ESMR5 1
#define BELGIQUE 0

#if ESMR5
bool CRCcheckEnabled = true;      // by default enable CRC checking
#else
bool CRCcheckEnabled = false;      // by default enable CRC checking
#endif

#if BELGIQUE
bool reportInDecimals = false;      // report in Watts
#else
bool reportInDecimals = true;
#endif

#if DEBUG == 1
#define BLED LED_BUILTIN
int sleepTime = 0;
#define debug(x) Serial.print(x)
#define debugf(x) Serial.printf(x)
#define debugf(x,y) Serial.printf(x,y)
#define debugff(x,y,z) Serial.printf(x,y,z)
#define debugln(x) Serial.println(x)
#elif DEBUG == 2
#define BLED LED_BUILTIN
int sleepTime = 0;
#define debug(x) Serial.print(x);if(telnetConnected)telnet.print(x)
#define debugf(x) Serial.printf(x)
#define debugf(x,y) Serial.printf(x,y)
#define debugff(x,y,z) Serial.printf(x,y,z)
#define debugln(x) Serial.println(x);if(telnetConnected)telnet.println(x)
#else
int sleepTime = 30;
#define BLED LED_BUILTIN
#define debug(x)
#define debugln(x)
#define debugf(x)
#define debugf(x,y)
#define debugff(x,y,z)
#endif

#define ToggleLED  digitalWrite(BLED, !digitalRead(BLED));
#define LEDoff  digitalWrite(BLED, HIGH);
#define LEDon   digitalWrite(BLED, LOW);


#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include "CRC16.h"

//van captive portal
/*  ============================================================================================================

    Captive Portal based on:
    https://gitlab.com/MrDIYca/code-samples/-/raw/master/esp8266_setup_portal.ino

    For more info, please watch my video at https://youtu.be/1VW-z4ibMXI
    MrDIY.ca
  ============================================================================================================== */
#include <EEPROM.h>
#include <ESP8266WebServer.h>
ESP8266WebServer    server(80);

#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include "ESP8266HTTPUpdateServer.h"
ESP8266HTTPUpdateServer httpUpdater(true);

// mqtt stuff . https://github.com/ict-one-nl/P1-Meter-ESP8266-MQTT/blob/master/P1Meter/P1Meter.ino
#include <PubSubClient.h>
WiFiClient espClient;                   // * Initiate WIFI client
PubSubClient mqtt_client(espClient);    // * Initiate MQTT client
long LAST_RECONNECT_ATTEMPT = 0;        // * MQTT Last reconnection counter
#define MQTT_MAX_RECONNECT_TRIES 10     // * MQTT network settings
bool MQTT_Server_Fail = false;
// end mqtt stuff

//// Raw data server
#include "ESPTelnet.h" 
ESPTelnet telnet;
uint16_t  port = 23;
IPAddress ip;
bool telnetConnected = false;
// end raw data stuff


struct settings {
  char ssid[30];
  char password[30];
  char domoticzIP[30] = "IPaddress";
  char domoticzPort[10] = "Port";
  char domoticzEnergyIdx[10] = "EnergyIdx";
  char domoticzGasIdx[10] = "GasIdx";
  char mqttTopic[50] = "sensors/power/p1meter";
  char mqttIP[30] = "";
  char mqttPort[10]  = "";
  char mqttUser[16] = "";
  char mqttPass[16] = "";
  char interval[3] = "20";
  char domo[3] ="j";
  char mqtt[3] ="n";
  char watt[3] = "n";
} user_data = {};

// energy management vars
int modemSleepTime = 8; // to be changed by interval setting
int interval;
uint32_t timeSinceLastUpdate = 0;
boolean entitledToSleep = false;       // aangezien slaapinterval en meetinterval niet synchroon lopen, moeten we na ontwaken eerst een telegram inlezen en afleveren alvorens ModemSleep mag worden aangeroepen.


// datagram stuff
#define MAXLINELENGTH 1040        // 0-0:96.13.0 has a maximum lenght of 1024 chars + 11 of its identifier
char telegram[MAXLINELENGTH];     // holds a single line of the datagram 
String datagram;                  // holds entire datagram for raw output  
bool datagramValid = false;    //
bool dataEnd = false;             // signals that we have found the end char in the data (!)
unsigned int currentCRC=0;        // the CRC v alue of the datagram
bool gas22Flag = false;        // flag for parsing second gas line on dsmr2.2 meters



// Vars to store meter readings
/* P1 telegram
/Ene5\T210-D ESMR5.0            header

1-3:0.2.8                       version     S2                      (50)
0-0:1.0.0                       timestamp   YYMMDDhhmmssX           (220528175910S) 
0-0:96.1.1                      equipmentId                         (xxxxxxxxxxxx) 
1-0:1.8.1                       electricityUsedTariff1              (048320.784*kWh) 
1-0:1.8.2                       electricityUsedTariff2              (066904.236*kWh) 
1-0:2.8.1                       electricityReturnedTariff1          (000213.613*kWh) 
1-0:2.8.2                       electricityReturnedTariff2          (000399.451*kWh) 
0-0:96.14.0                     tariffIndicatorElectricity          (0001) 
1-0:1.7.0                       actualElectricityPowerDelivere   (00.416*kW) 
1-0:2.7.0                       actualElectricityPowerReturned   (00.710*kW) 
0-0:96.7.21                     numberPowerFailuresAny              (01274) 
0-0:96.7.9                      numberLongPowerFailuresAny          (00030) 
1-0:99.97.0                     longPowerFailuresLog                (7)(0-0:96.7.19)(220325092704W)(0000001596*s)(211109092740W)(0000003500*s)(210317125706W)(0000000219*s)(191121111 231W)(0000002891*s)(180127175539W)(0000031065*s)(180127082729W)(0000002493*s)(180127074429W)(0000723567*s) 
1-0:32.32.0                     numberVoltageSagsL1                 (00270) 
1-0:52.32.0                     numberVoltageSagsL2                 (00016) 
1-0:72.32.0                     numberVoltageSagsL3                 (00124) 
1-0:32.36.0                     numberVoltageSwellsL1               (00003) 
1-0:52.36.0                     numberVoltageSwellsL2               (00003) 
1-0:72.36.0                     numberVoltageSwellsL3               (00003) 
0-0:96.13.0                     textMessage               1024      () 
1-0:32.7.0                      instantaneousVoltageL1              (228.0*V) 
1-0:52.7.0                      instantaneousVoltageL2              (227.0*V) 
1-0:72.7.0                      instantaneousVoltageL3              (231.0*V) 
1-0:31.7.0                      instantaneousCurrentL1              (001*A) 
1-0:51.7.0                      instantaneousCurrentL2              (000*A) 
1-0:71.7.0                      instantaneousCurrentL3              (002*A) 
1-0:21.7.0                      activePowerL1P                      (00.416*kW) 
1-0:41.7.0                      activePowerL2P                      (00.000*kW) 
1-0:61.7.0                      activePowerL3P                      (00.000*kW) 
1-0:22.7.0                      activePowerL1NP                     (00.000*kW) 
1-0:42.7.0                      activePowerL2NP                     (00.128*kW) 
1-0:62.7.0                      activePowerL3NP                     (00.582*kW) 
0-1:24.1.0                      deviceType(003) 
0-1:96.1.0                      gasId                               (4730303533303033373531323235323138) 
0-1:24.2.1                      gasReceived5min                     (220528175500S)(14330.108*m3) 
!58EF

Note: the block 0-n:24.1.0 (device type), 0-n.91.1.0 (deviceId), 0-1:24.2.1 (delivered in last 5 min) may be either:
gas, water, heat or cold

*/
// we capture all data in char arrays or Strings for longer hard to delineate data

String P1header;
char P1version[3];
char P1timestamp[14];
String equipmentId;
char electricityUsedTariff1[12];
char electricityUsedTariff2[12];
char electricityReturnedTariff1[12];
char electricityReturnedTariff2[12];
char tariffIndicatorElectricity[6];
char actualElectricityPowerDelivered[8];
char actualElectricityPowerReturned[8];
char numberPowerFailuresAny[6];
char numberLongPowerFailuresAny[6];
String longPowerFailuresLog;
char numberVoltageSagsL1[7];
char numberVoltageSagsL2[7];
char numberVoltageSagsL3[7];
char numberVoltageSwellsL1[7];
char numberVoltageSwellsL2[7];
char numberVoltageSwellsL3[7];
String textMessage;
char instantaneousVoltageL1[7];
char instantaneousVoltageL2[7];
char instantaneousVoltageL3[7];
char instantaneousCurrentL1[6];
char instantaneousCurrentL2[6];
char instantaneousCurrentL3[6];
char activePowerL1P[9];
char activePowerL2P[9];
char activePowerL3P[9];
char activePowerL1NP[9];
char activePowerL2NP[9];
char activePowerL3NP[9];
char deviceType[5];
String gasId;
char gasReceived5min[12];

char prevGAS[12];           // not an P1 protocol var, but holds gas value


// process stuff
#define DISABLED 0
#define WAITING 1
#define READING 2
#define CHECKSUM 3
#define DONE 4
#define FAILURE 5
int state = DISABLED;

bool wifiSta = false;
bool breaking = false;
bool softAp = false;

void setup() {
  WiFi.forceSleepBegin(sleepTime * 1000000L); //In uS. Must be same length as your delay
  delay(sleepTime * 1000); //Hang out at 15mA for 60 (sleeptime) seconds
  WiFi.forceSleepWake(); // Wifi on

  pinMode(BLED, OUTPUT);
  #if ESMR5 == 1
    Serial.begin(115200);
    debugln("Serial.begin(115200);");
  #else
    Serial.begin(9600,SERIAL_7E1);
    debugln("Serial.begin(9600,SERIAL_7E1);");
  #endif

  blink(2);
    debugln("");
    debugln("Booting");
    debugln("back from my 60 sec Cap charging nap… ");
    debugln("Let's go…");
    
  EEPROM.begin(sizeof(struct settings) );
  EEPROM.get( 0, user_data );
  debugln("EEprom read: done");
  PrintConfigData();  

  if (user_data.watt[0] =='j') reportInDecimals = true; else reportInDecimals = false;
  
  interval = atoi( user_data.interval) * 1000; 
  modemSleepTime = interval - 2000;
  
  debugln("Trying to connect to your wifi network:");
     WiFi.mode(WIFI_STA);
     WiFi.begin(user_data.ssid, user_data.password);
     byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
      ToggleLED
        delay(500);
        debug("o");
    
      if (tries++ > 30) {
        debugln("");
        debugln("Setting up Captive Portal by the name 'P1_setup'");
        LEDon
        WiFi.mode(WIFI_AP);
        softAp = WiFi.softAP("P1_Setup", "");
        breaking = true;
        break;
      }
    } 
   
    debugln("");
    debugln("Set up wifi, either in STA or AP mode");
    if (softAp) debugln("running in AP mode, all handles will be initiated");
    server.on("/",       handleRoot);
    server.on("/Setup",  handleSetup);
    server.on("/SetupSave",  handleSetupSave);
    server.on("/P1",     handleP1);
    server.on("/Data",  handleRawData);
    server.on("/Help", handleHelp);
    server.begin();

    if (WiFi.status() == WL_CONNECTED){ 
      debugln("HTTP server running.");
      debug("IP address: ");
      debugln(WiFi.localIP());
      wifiSta = true;
      debugln("wifi running in Sta (normal) mode");
      LEDoff
      setupTelnet();  //komt van een paar regels lager. moved up ivm no setup issue in v1.0j
    
    MDNS.begin(host);
    httpUpdater.setup(&server);
    server.begin();
    MDNS.addService("http", "tcp", 80);
    if (user_data.mqtt[0] == 'j'){
        mqtt_client.setServer(user_data.mqttIP, atoi(user_data.mqttPort));
        debugln("MQTT server assigned.");
    }
 //  setupTelnet();
   debugln("All systems are go...");
  }
  state = WAITING;    // signal that we are waiting for a valid start char (aka /)
}

void wifiReconnect(){
    debugln("Trying to connect to your wifi network:");
     WiFi.mode(WIFI_STA);
     WiFi.begin(user_data.ssid, user_data.password);
     byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        ToggleLED
        delay(500);
        debug("o");
        if (tries++ > 30) {
          debugln("");
          debugln("Something is terribly wrong, can't connect to wifi (anymore).");
          LEDon
          delay(60000);
    }
  }
}

bool isNumber(char* res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9'))  && (res[i] != '.' && res[i] != 0)) {
      return false;
    }
  }
  return true;
}

int FindCharInArrayRev(char array[], char c, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}

long getValidVal(long valNew, long valOld, long maxDiffer)
{
  //check if the incoming value is valid
      if(valOld > 0 && ((valNew - valOld > maxDiffer) && (valOld - valNew > maxDiffer)))
        return valOld;
      return valNew;
}

void getValue(char *theValue, char *buffer, int maxlen, char startchar, char endchar){
    String cleanres="";
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;

    char res[16];
    memset(res, 0, sizeof(res));
    

   if (strncpy(res, buffer + s + 1, l)) {
        if (endchar == '*')
        {
            if (isNumber(res, l)){
              int flag = 1;
              for(int i=0; i < l + 1; i++)     // was <
              {
                if (flag == 1 && res[i] != '0') flag = 0;
                if (res[i] == '0' && res[i+1] == '.') flag = 0;
                if(flag != 1){
                  if (!reportInDecimals)    {       // BELGIQUE // report in Watts instead of KW
                      if (res[i] != '.') cleanres += res[i];
                    }
                    else cleanres += res[i];
                  }
              }
            }
          cleanres.toCharArray(theValue, cleanres.length());
          theValue[cleanres.length()+1]=0;
              } else if (endchar == ')') 
        {
            if (isNumber(res, l))  strncpy(theValue, res, l);
            theValue[cleanres.length()+1]=0;
        }
    }
 }

 void getGasValue(char *theValue, char *buffer, int maxlen, char startchar, char endchar){ 
  String cleanres="";
  int s = 0;
  if  (FindCharInArrayRev(buffer, ')(', maxlen - 2) != -1)  // some meters report the meterID in () before the section with actual gas value
      s = FindCharInArrayRev(buffer, ')(', maxlen - 2);
  else
    s = FindCharInArrayRev(buffer, '(', maxlen - 2);
    
  if (s < 8) return;
  if (s > 32) s = 32;
  int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
  if (l < 4) return;
  if (l > 12) return;
  char res[16];
  memset(res, 0, sizeof(res));
  if (strncpy(res, buffer + s + 1, l)) {
        if (endchar == '*')
        {
            if (isNumber(res, l)){
              int flag = 1;
              for(int i=0; i < l + 1; i++)     // was <
              {
                if (flag == 1 && res[i] != '0') flag = 0;
                if (res[i] == '0' && res[i+1] == '.') flag = 0;
                if(flag != 1){
                  if (!reportInDecimals)    {       // BELGIQUE // report in Watts instead of KW
                      if (res[i] != '.') cleanres += res[i];
                    }
                    else cleanres += res[i];
                  }
              }
            }
          cleanres.toCharArray(theValue, cleanres.length());
          theValue[cleanres.length()+1]=0;
              } else if (endchar == ')') 
        {
            if (isNumber(res, l))  strncpy(theValue, res, l);
        }
    }
  
}

void getGas22Value(char *theValue, char *buffer, int maxlen, char startchar, char endchar){
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;
    char res[16];
    memset(res, 0, sizeof(res));
    
   if (strncpy(res, buffer + s + 1, l)) {
     if (isNumber(res, l))  strncpy(theValue, res, l);
     theValue[l+1]=0;
   }
}

void readTelegram() {
  if (Serial.available()) {
    memset(telegram, 0, sizeof(telegram));
    while (Serial.available()) {
      int len = Serial.readBytesUntil('\n', telegram, MAXLINELENGTH);
      telegram[len] = '\n';
      telegram[len+1] = 0;
      yield();
      ToggleLED
      debugln(telegram);
      
      if(decodeTelegram(len+1))     // test for valid CRC
      {
          TelnetReporter();
          if (user_data.mqtt[0] =='j') MQTT_reporter();
          if (user_data.domo[0] =='j') {
            UpdateElectricity();
            UpdateGas();
          }            
          timeSinceLastUpdate = millis();
          entitledToSleep = true;
          state = WAITING;
      } 
        else { // we don't have valid data, but still may need to report to Domo
        if (dataEnd && !CRCcheckEnabled) { //this is used for dsmr 2.2 meters
          debugln("incorrect CRC or running on a DSMR2 meter");
             TelnetReporter();
             if (user_data.mqtt[0] =='j') MQTT_reporter();
              if (user_data.domo[0] =='j') {
                 UpdateElectricity();
                 UpdateGas();
          }            
          state = WAITING;
        }
      }
      
    }
    LEDoff
  }
}

void blink(int t){
  for (int i=0 ; i <=t; i++){
    LEDon       // Signaal naar laag op ESP-M3
    delay(200); // wacht 200 millisec
    LEDoff;     // LEDoff, signaal naar hoog op de ESP-M3
  }
}

void loop() {  
  
  server.handleClient(); //handle web requests
  MDNS.update();
  
  if (millis() > timeSinceLastUpdate + interval)
  {
    readTelegram();  
    if (wifiSta) {
      telnet.loop();
      
    if (user_data.mqtt[0] =='j' && ! MQTT_Server_Fail){
      debugln("mqtt loop");
        if (!mqtt_client.connected())
        {
          if (millis() - LAST_RECONNECT_ATTEMPT > 5000)
          {
            LAST_RECONNECT_ATTEMPT = millis();
            if (mqtt_reconnect())  LAST_RECONNECT_ATTEMPT = 0;
          }
        }    else  mqtt_client.loop();
      }
    }
  }
 if (wifiSta && WiFi.status() != WL_CONNECTED) wifiReconnect();
}



void modemSleep(){
  debugln("modemSleep");
  WiFi.forceSleepBegin(modemSleepTime * 1000000L); //In uS. Must be same length as your delay
  delay(modemSleepTime * 1000); //Hang out at 15mA for sleeptime
  WiFi.forceSleepWake(); // Wifi on
  entitledToSleep = false;
}
