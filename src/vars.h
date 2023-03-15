/*
 * vars.h
 */

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
bool yearFlag=false;
int EverSoOften = 0;

#define SET_FLAG(n, f) n = n | f;   //((n) |= (f)) 
#define CLR_FLAG(n, f) n = n & !f;  //((n) &= ~(f)) 
//#define TGL_FLAG(n, f) ((n) ^= (f)) 
#define CHK_FLAG(n, f) (n & f) //((n) & (f))

int checkMinute = 0; // holds when doHourly was called 
char minG[12];

struct logvars{
  char hourE1[12];  // delivered tariff 1
  char hourE2[12];  // delivered tariff 2
  char hourR1[12];  // return tariff 1
  char hourR2[12];  // return tariff 2
  char hourTE[12];   // total tariff 1
  char hourTR[12];   // total tariff 2
  char hourG[12];   // Gas

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
unsigned long APtimer = 0;        // time we went into AP mode. Restart module if in AP for 10 mins as we might have gotten in AP due to router wifi issue 
time_t LastReportinSecs = 0;

long last10 = 0;
unsigned long time_to_wake = 0; // calculated wakeup time
unsigned long time_to_sleep = 0;    // calculated sleep time
unsigned long lastSleeptime = 0;
boolean entitledToSleep = false;       // aangezien slaapinterval en meetinterval niet synchroon lopen, moeten we na ontwaken eerst een telegram inlezen en afleveren alvorens ModemSleep mag worden aangeroepen.
bool monitoring = false;          // are we permitted to collect P1 data? Not if in setup or firmware update
bool atsleep = true;
bool bootSetup = false;           // flag for adminpassword bypass 

// datagram stuff
#define MAXLINELENGTH 1040        // 0-0:96.13.0 has a maximum lenght of 1024 chars + 11 of its identifier
char telegram[MAXLINELENGTH];     // holds a single line of the datagram 
String datagram;                  // holds entire datagram for raw output  
bool datagramValid = false;    //
bool dataEnd = false;             // signals that we have found the end char in the data (!)
unsigned int currentCRC=0;        // the CRC v alue of the datagram
bool gas22Flag = false;        // flag for parsing second gas line on dsmr2.2 meters
bool reportInDecimals = true;
bool CRCcheckEnabled = true;      // by default enable CRC checking
String mtopic ="Een lange MQTT boodschap als placeholder";

// process stuff
#define DISABLED 0
#define WAITING 1
#define READING 2
#define CHECKSUM 3
#define DONE 4
#define FAILURE 5
int state = DISABLED;

#define CONFIG  0   // getting basic Meter data to select correct parse rules
#define GOTMETER 1
#define RUNNING 2
int devicestate = CONFIG;

#define NONE 0
#define MAIN 1
#define DATA 2
#define WCONFIG 3
#define UPDATE 4
int webstate = NONE;


bool wifiSta = false;
bool breaking = false;
bool softAp = false;
bool Json = false;
bool Telnet = false;

bool AdminAuthenticated = false;
bool Mqtt = false;
bool MqttConnected = false;
bool MqttDelivered = false;
String LastReport = ""; //timestamp of last telegram reported

bool mqtt_dsmr = false; // deliver mqtt data in 'dsmr reader" format
bool MQTT_debug = false;

bool daystart = true;
bool OEstate = false;  // 74125 OE output enable is off by default (EO signal high) 
// a bit of a kludge to get the first readings at coldboot to reset the daycounter. the coldboot flags will be reset once the daycounters are reset with valid values. The availability of valid values is signalled by
// the two gotXReadings 
bool coldbootE = true;
bool coldbootG = true;
bool gotPowerReading = false;
bool gotGasReading = false;
char setupToken[18];

bool needToInitLogVars = false;
bool needToInitLogVarsGas = false;
