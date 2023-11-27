#include <Arduino.h>
#include <ESP8266WiFi.h>

// CRC16.h
unsigned int CRC16(unsigned int crc, unsigned char *buf, int len);

// p1debug.h
void readVoltage();
void PrintConfigData();

// decoder.h
void decodeLine(int len);
String readUntilStar(int start, int end);
String readBetweenDoubleParenthesis(int start, int end);
String readFirstParenthesisVal(int start, int end);
void OBISparser(int len);

// functions.h
void alignToTelegram();
void blink(int t);
void RTS_on();
void RTS_off();
bool isNumber(char *res, int len);
int FindCharInArray(char array[], char c, int len);
void settime();
String timestamp();
String timestampkaal();
void timeIsSet_cb();
void createToken();
void listDir(const char *dirname);
void readFile(const char *path);
void writeFile(const char *path, const char *message);
void appendFile(const char *path, const char *message);
void renameFile(const char *path1, const char *path2);
void deleteFile(const char *path);
int numLines(const char *path);
boolean MountFS();
static void handleNotFound();
void zapFiles();
void zapConfig();
void formatFS();
String totalXY(const char *type, String period);
void identifyMeter();
void initTimers();
void checkCounters();
void resetFlags();
void doWatchDogs();
uint32_t calculateCRC32(const uint8_t *data, size_t length);

// graph.h
void handleGraphMenu();
void DumpDataFiles();
void selectGraph();
void theGraph(const char *type1, const char *type2, String title1,
              String title2, String label, String period);
void calendarGas();

// JSON.h
void doJSON();
bool DomoticzJson(char *idx, int nValue, char *sValue);
void UpdateGas();
void UpdateElectricity();

// logging.h
void doInitLogVars();
void doInitLogVarsGas();
void doMinutelyLog();
void doHourlyLog();
void doDailyLog();
void doWeeklyLog();
void doMonthlyLog();
void doYearlyLog();
void resetEnergyDaycount();
void resetEnergyMonthcount();
void resetGasCount();
void DirListing();

// MQTT.h
void doMQTT();
bool mqtt_connect();
void callback(char *topic, byte *payload, unsigned int length);
void send_mqtt_message(const char *topic, char *payload);
void send_metric(String name, float metric);
void mqtt_send_metric(String name, char *metric);
void MQTT_reporter();
void MQTT_Debug();

// newMQTT.h

// TELNET.h
void setupTelnet();
void telnetloop();
bool telnetStillRunning();
void TelnetReporter();
void telnetD(String x);
void telnetDLn(String x);

// webserver.h
void addHead(String &str, int refresh);
void addGraphHead(String &str);
void addIntro(String &str);
void addUptime(String &str);
void handleRoot();
void handleSetupSave();
void handleLogin();
void handleUpdateLogin();
void handleUploadForm();
void errorLogin(String returnpage);
void setupSaved(String &str);
void successResponse();
void handleHelp();
void addFoot(String &str);
void addFootBare(String &str);

// webserverNL.h
void handleSetup();
void handleP1();
void handleRawData();

// wifi_functions.h
int getQuality();
void setRFPower();
void modemSleep();
void modemWake();
void wifiReconnect();
void start_webservices();
void start_services();
void stop_services();
void onWifiConnect(const WiFiEventStationModeGotIP &event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
void calcSleeptime();
