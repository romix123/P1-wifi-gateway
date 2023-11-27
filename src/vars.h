/*
 * vars.h
 */
struct settings {
  char dataSet[4];
  char ssid[32];
  char password[64];
  char domoticzIP[30] = "IPaddress";
  char domoticzPort[10] = "Port";
  char domoticzEnergyIdx[10] = "EnergyIdx";
  char domoticzGasIdx[10] = "GasIdx";
  char mqttTopic[50] = "sensors/power/p1meter";
  char mqttIP[30] = "";
  char mqttPort[10] = "";
  char mqttUser[32] = "";
  char mqttPass[64] = "";
  char interval[3] = "20";
  char domo[4] = "j";
  char mqtt[4] = "n";
  char watt[4] = "n";
  char telnet[4] = "n";
  char debug[4] = "n";
  char adminPassword[32];
} config_data = {};

// admin related
bool bootSetup = false; // flag for adminpassword bypass
bool AdminAuthenticated = false;
char setupToken[18];

// Vars to store meter readings
// we capture all data in char arrays or Strings for longer hard to delineate
// data
String P1header;
char P1version[8];
int P1prot; // 4 or 5 based on P1version 1-3:0.2.8
char P1timestamp[30] = "\0";
char equipmentId[100] = "\0";
char equipmentId2[100] = "\0";
char electricityUsedTariff1[12];
char electricityUsedTariff2[12];
char electricityReturnedTariff1[12];
char electricityReturnedTariff2[12];
char tariffIndicatorElectricity[8];
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
char instantaneousCurrentL1[9];
char instantaneousCurrentL2[9];
char instantaneousCurrentL3[9];
char activePowerL1P[10];
char activePowerL2P[10];
char activePowerL3P[10];
char activePowerL1NP[10];
char activePowerL2NP[10];
char activePowerL3NP[10];
char actualElectricityPowerDeli[14];
char actualElectricityPowerRet[14];

// // Swedish specific
// char cumulativeActiveImport[12];    // 1.8.0
// char cumulativeActiveExport[12];    // 2.8.0
// char cumulativeReactiveImport[12];  // 3.8.0
// char cumulativeReactiveExport[12];  // 4.8.0
// char momentaryActiveImport[12];     // 1.7.0
// char momentaryActiveExport[12];     // 2.7.0
// char momentaryReactiveImport[12];   // 3.7.0
// char momentaryReactiveExport[12];   // 4.7.0
// char momentaryReactiveImportL1[12]; // 23.7.0
// char momentaryReactiveImportL2[12]; // 43.7.0
// char momentaryReactiveImportL3[12]; // 63.7.0
// char momentaryReactiveExportL1[12]; // 24.7.0
// char momentaryReactiveExportL2[12]; // 44.7.0
// char momentaryReactiveExportL3[12]; // 64.7.0

// char reactivePowerL1P[9]; // Sweden uses these 6
// char reactivePowerL2P[9];
// char reactivePowerL3P[9];
// char reactivePowerL1NP[9];
// char reactivePowerL2NP[9];
// char reactivePowerL3NP[9];

// end Swedish
char deviceType[5];
char gasId[100] = "\0";
;
char gasReceived5min[12];
char gasDomoticz[12]; // Domoticz wil gas niet in decimalen?

char prevGAS[12]; // not an P1 protocol var, but holds gas value

// logging vars
byte logFlags;
// #define secFlag    1
// #define minFlag    2
// #define hourFlag   4
// #define dayFlag    8
// #define weekFlag   16
// #define monthFlag  32
// #define yearFlag   64
bool timeIsSet = false;
bool TimeTriggersSet = false;

bool minFlag = false;
bool hourFlag = false;
bool dayFlag = false;
bool weekFlag = false;
bool monthFlag = false;
bool yearFlag = false;
unsigned long EverSoOften = 0;
unsigned long TenSecBeat = 0;
unsigned long FiveSecBeat = 0;
unsigned long SecBeat = 0;

#define SET_FLAG(n, f) n = n | f;  //((n) |= (f))
#define CLR_FLAG(n, f) n = n & !f; //((n) &= ~(f))
//#define TGL_FLAG(n, f) ((n) ^= (f))
#define CHK_FLAG(n, f) (n & f) //((n) & (f))

int checkMinute = 0; // holds when doHourly was called
char minG[12];

struct logvars {
  char hourE1[12]; // delivered tariff 1
  char hourE2[12]; // delivered tariff 2
  char hourR1[12]; // return tariff 1
  char hourR2[12]; // return tariff 2
  char hourTE[12]; // total tariff 1
  char hourTR[12]; // total tariff 2
  char hourG[12];  // Gas

  char dayE1[12];
  char dayE2[12];
  char dayR1[12];
  char dayR2[12];
  char dayTE[12];
  char dayTR[12];
  char dayG[12];

  char weekE1[12];
  char weekE2[12];
  char weekR1[12];
  char weekR2[12];
  char weekTE[12];
  char weekTR[12];
  char weekG[12];

  char monthE1[12];
  char monthE2[12];
  char monthR1[12];
  char monthR2[12];
  char monthTE[12];
  char monthTR[12];
  char monthG[12];

  char yearE1[12];
  char yearE2[12];
  char yearR1[12];
  char yearR2[12];
  char yearTE[12];
  char yearTR[12];
  char yearG[12];
} log_data = {};

// energy management vars
int interval;
unsigned long nextUpdateTime = 0;
unsigned long LastReportinMillis = 0;
unsigned long LastSample = 0;
unsigned long APtimer =
    0; // time we went into AP mode. Restart module if in AP for 10 mins as we
       // might have gotten in AP due to router wifi issue
time_t LastReportinSecs = 0;
float RFpower =
    0; // 20.5; // (For OdBm of output power, lowest. Supply current~70mA)
       // 20.5dBm (For 20.5dBm of output, highest. Supply current~85mA)

long last10 = 0;
unsigned long time_to_wake = 0;  // calculated wakeup time
unsigned long time_to_sleep = 0; // calculated sleep time
unsigned long lastSleeptime = 0;
boolean entitledToSleep =    false; // aangezien slaapinterval en meetinterval niet synchroon lopen,
           // moeten we na ontwaken eerst een telegram inlezen en afleveren
           // alvorens ModemSleep mag worden aangeroepen.
bool monitoring = false; // are we permitted to collect P1 data? Not if in setup
                         // or firmware update
bool atsleep = true;

// datagram stuff
#define MAXLINELENGTH                                                          \
  2048 // 0-0:96.13.0 has a maximum lenght of 1024 chars + 11 of its identifier
char telegram[MAXLINELENGTH]; // holds a single line of the datagram
String datagram;              // holds entire datagram for raw output
String meterId = "";
String meterName = "";
bool meternameSet = false; // do we have a metername already?

bool datagramValid = false; //
int dataFailureCount = 0; //counter for CRC failures
bool dataEnd = false; // signals that we have found the end char in the data (!)
unsigned int currentCRC = 0; // the CRC v alue of the datagram
bool gas22Flag = false; // flag for parsing second gas line on dsmr2.2 meters
bool reportInDecimals = true;
bool CRCcheckEnabled = true; // by default enable CRC checking
String mtopic = "Een lange MQTT boodschap als placeholder";

// process stuff
bool daystart = true;
bool OEstate =
    false; // 74125 OE output enable is off by default (EO signal high)
// a bit of a kludge to get the first readings at coldboot to reset the
// daycounter. the coldboot flags will be reset once the daycounters are reset
// with valid values. The availability of valid values is signalled by the two
// gotXReadings
bool coldbootE = true;
bool coldbootG = true;
bool gotPowerReading = false;
bool gotGasReading = false;
bool needToInitLogVars = false;
bool needToInitLogVarsGas = false;
int countryCode = 31;

#define DISABLED 0
#define WAITING 1
#define READING 2
#define CHECKSUM 3
#define DONE 4
#define FAILURE 5
#define FAULT 6
int state = DISABLED;

#define CONFIG 0 // getting basic Meter data to select correct parse rules
#define GOTMETER 1
#define RUNNING 2
int devicestate = CONFIG;

// Wifi stuff
bool rtcValid = false;
#define NONE 0
#define MAIN 1
#define DATA 2
#define WCONFIG 3
#define UPDATE 4
int webstate = NONE;
String wifiStatus = "";

bool wifiSta = false;
bool breaking = false;
bool softAp = false;
bool Json = false;
// We make a structure to store connection information
// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods
// read and write 4 bytes at a time, so the RTC data structure should be padded
// to a 4-byte multiple.
struct {
  uint32_t crc32;    // 4 bytes
  uint8_t channel;   // 1 byte,   5 in total
  uint8_t ap_mac[6]; // 6 bytes, 11 in total
  uint8_t padding;   // 1 byte,  12 in total
} rtcData;

// JSON
String LastJReport = ""; // timestamp of last telegram reported via JSON

// Telnet
bool Telnet = false;
int currentTelnetSession;
#define MAX_SRV_CLIENTS 10
#define STACK_PROTECTOR 1512 // bytes
bool telnetConnected = false;
bool telnetDebugConnected = false;
String LastTReport = ""; // timestamp of last telegram reported on Telnet
const int port = 23;
int activetelnets = 0;
int telnetDebugClient = 0;

// MQTT
bool Mqtt = false;
bool MqttConnected = false;
bool MqttDelivered = false;
String LastMReport = ""; // timestamp of last telegram reported on MQTT
bool mqtt_dsmr = false;  // deliver mqtt data in 'dsmr reader" format
bool MQTT_debug = false;
bool MQTT_Server_Fail = false;
long nextMQTTreconnectAttempt;

// debug related
float volts;
String statusMsg;
uint32_t FlashSize;
