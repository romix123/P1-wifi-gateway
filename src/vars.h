#include <WString.h>

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

// Value that is parsed as a three-decimal float, but stored as an
// integer (by multiplying by 1000). Supports val() (or implicit cast to
// float) to get the original value, and int_val() to get the more
// efficient integer value. The unit() and int_unit() methods on
// FixedField return the corresponding units for these values.
struct FixedValue {
  FixedValue() {
    _value = 0;
  }
  FixedValue(String value) {
    _value = value.toFloat() * 1000;
  }
  operator float() { return val(); }
  float val() { return _value / 1000.0; }
  uint32_t int_val() { return _value; }

  uint32_t _value{0};
};

// Vars to store meter readings
// we capture all data in char arrays or Strings for longer hard to delineate
// data
struct DSMRData {
  // String identification;
  String p1_version;                   // P1version
  String timestamp;                    // P1timestamp
  String equipment_id;                 // equipmentId
  FixedValue energy_delivered_tariff1; // electricityUsedTariff1
  FixedValue energy_delivered_tariff2; // electricityUsedTariff2
  FixedValue energy_returned_tariff1;  // electricityReturnedTariff1
  FixedValue energy_returned_tariff2;  // electricityReturnedTariff2
  String electricity_tariff;           // tariffIndicatorElectricity
  FixedValue power_delivered;          // actualElectricityPowerDeli
  FixedValue power_returned;           // actualElectricityPowerRet
  // FixedValue electricity_threshold;
  // uint8_t electricity_switch_position;
  uint32_t electricity_failures;      // numberPowerFailuresAny
  uint32_t electricity_long_failures; // numberLongPowerFailuresAny
  String electricity_failure_log;     // longPowerFailuresLog
  uint32_t electricity_sags_l1;       // numberVoltageSagsL1
  uint32_t electricity_sags_l2;       // numberVoltageSagsL2
  uint32_t electricity_sags_l3;       // numberVoltageSagsL3
  uint32_t electricity_swells_l1;     // numberVoltageSwellsL1
  uint32_t electricity_swells_l2;     // numberVoltageSwellsL2
  uint32_t electricity_swells_l3;     // numberVoltageSwellsL3
  // String message_short;
  // String message_long;
  FixedValue voltage_l1;         // instantaneousVoltageL1
  FixedValue voltage_l2;         // instantaneousVoltageL2
  FixedValue voltage_l3;         // instantaneousVoltageL3
  FixedValue current_l1;         // instantaneousCurrentL1
  FixedValue current_l2;         // instantaneousCurrentL2
  FixedValue current_l3;         // instantaneousCurrentL3
  FixedValue power_delivered_l1; // activePowerL1P
  FixedValue power_delivered_l2; // activePowerL2P
  FixedValue power_delivered_l3; // activePowerL3P
  FixedValue power_returned_l1;  // activePowerL1NP
  FixedValue power_returned_l2;  // activePowerL2NP
  FixedValue power_returned_l3;  // activePowerL3NP

  // uint16_t gas_device_type;
  String gas_equipment_id; // equipmentId2
  // uint8_t gas_valve_position;
  String gas_delivered; // gasReceived5min

  // uint16_t thermal_device_type;
  // String thermal_equipment_id;
  // uint8_t thermal_valve_position;
  // String thermal_delivered;

  // uint16_t water_device_type;
  // String water_equipment_id;
  // uint8_t water_valve_position;
  // String water_delivered;
  // uint16_t sub_device_type;
  // String sub_equipment_id;
  // uint8_t sub_valve_position;
  // String sub_delivered;

  uint8_t P1prot() { return p1_version[0] == '4' ? 4 : 5; }
} dsmrData;

String prevGAS; // not an P1 protocol var, but holds gas value

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
time_t LastReportinSecs = 0;

long last10 = 0;
bool monitoring = false; // are we permitted to collect P1 data? Not if in setup
                         // or firmware update

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

// JSON
bool Json = false;
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
