/*
 * Copyright (c) 2022 Ronald Leenes
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
 */

/**
 * @file functions.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 *
 * @brief This file contains useful functions
 *
 * @see http://esp8266thingies.nl
 */

void alignToTelegram() {
  // make sure we don't drop into the middle of a telegram on boot. Read
  // whatever is in the stream until we find the end char ! then read until EOL
  // and flsuh serial, return to loop to pick up the first complete telegram.

  int inByte = 0; // incoming serial byte
  char buf[10];
  if (Serial.available() > 0) {
    while (Serial.available()) {
      inByte = Serial.read();
      if (inByte == '!')
        break;
    }
    Serial.readBytesUntil('\n', buf, 9);
    Serial.flush();
  }
}

void RTS_on() {           // switch on Data Request
  digitalWrite(OE, LOW);  // enable buffer
  digitalWrite(DR, HIGH); // turn on Data Request
  OEstate = true;
  Log.verbose("Data request on at ");
  Log.verbose("%d\n", millis());
}

void RTS_off() {          // switch off Data Request
  digitalWrite(DR, LOW);  // turn off Data Request
  digitalWrite(OE, HIGH); // put buffer in Tristate mode
  OEstate = false;
  nextUpdateTime = millis() + interval;
  Log.verbose("Data request off at: ");
  Log.verbose("%d", millis());
  Log.verbose(" nextUpdateTime: ");
  Log.verbose("%d\n", nextUpdateTime);
}

bool isNumber(char *res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0)) {
      return false;
    }
  }
  return true;
}

int FindCharInArray(char array[], char c, int len) {
  for (int i = 0; i < len; i++) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}

void settime() {
  //(hr,min,sec,day,mnth,yr);
  // YYMMDDhhmmssX
  setTime(NUM(6, 10) + NUM(7, 1), NUM(8, 10) + NUM(9, 1),
          NUM(10, 10) + NUM(11, 1), NUM(4, 10) + NUM(5, 1),
          NUM(2, 10) + NUM(3, 1), NUM(0, 10) + NUM(1, 1));
  Log.verbose("%d", timestamp());
  LastReportinSecs = now();
  timeIsSet = true;
}

String timestamp() {
  return (String) "time: " + hour() + ":" + minute() + ":" + second();
}

String timestampkaal() {
  return (String)hour() + ":" + minute() + ":" + second();
}

void timeIsSet_cb() {
  if (!timeIsSet) {
    Log.verboseln("Time is set, starting timer service.");
    timeIsSet = true;
    timerAlarm.startService();
  }
}

void createToken() {
  char HexList[] = "1234567890ABCDEF";

  for (int i = 0; i < 16; i++) {
    setupToken[i] = HexList[random(0, 16)];
  }
  setupToken[16] = '\0';
  Log.verboseln("Token: %s", setupToken);
}

void listDir(const char *dirname) {
  Log.verbose("Listing directory: %s\n", dirname);

  Dir root = FST.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Log.verbose("  FILE: ");
    Log.verbose("%s", root.fileName());
    Log.verbose("  SIZE: ");
    Log.verbose("%d", file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm *tmstruct = localtime(&cr);
    Log.verbose("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n",
                (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Log.verbose("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
                (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                tmstruct->tm_sec);
  }
}

void readFile(const char *path) {
  Log.verbose("Reading file: %s\n", path);
  Log.verboseln("");
  File file = FST.open(path, "r");
  if (!file) {
    Log.verboseln("Failed to open file for reading");
    return;
  }

  Log.verboseln("Read from file: ");
  while (file.available()) {
    Log.verbose("%s", String((char)file.read()));
  }
  Log.verboseln("");
  file.close();
}

void writeFile(const char *path, const char *message) {
  Log.verbose("Writing file: %s\n", path);

  File file = FST.open(path, "w");
  if (!file) {
    Log.verboseln("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Log.verboseln("File written");
  } else {
    Log.verboseln("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void appendFile(const char *path, const char *message) {

  Log.verboseln("Appending to file: %s time: %d", path, millis());
  char payload[50];

  File file = FST.open(path, "a");
  if (!file) {
    Log.verboseln("Failed to open file for appending");
    sprintf(payload, "can't open file %s", path);
    send_mqtt_message("p1wifi/logging", payload);
    return;
  }
  if (file.print(message)) {
    Log.verbose("Message appended: ");
    Log.verboseln(message);
    sprintf(payload, "Append to %s with %s succeeded: %s", path, message,
            string2char(timestampkaal()));
    send_mqtt_message("p1wifi/logging", payload);
  } else {
    Log.verboseln("Append failed");
    sprintf(payload, "Append to %s with %s failed: %s", path, message,
            string2char(timestampkaal()));
    send_mqtt_message("p1wifi/logging", payload);
  }
  file.flush();
  delay(3000);
  file.close();
}

void renameFile(const char *path1, const char *path2) {
  Log.verbose("Renaming file %s ", path1);
  Log.verbose("to %s\n", path2);
  if (FST.rename(path1, path2)) {
    Log.verboseln("File renamed");
  } else {
    Log.verboseln("Rename failed");
  }
}

void deleteFile(const char *path) {
  Log.verbose("Deleting file: %s\n", path);
  if (FST.remove(path)) {
    Log.verboseln("File deleted");
  } else {
    Log.verboseln("Delete failed");
  }
}

/*
 * Read a file one field at a time.
 *
 * file - File to read.
 *
 * str - Character array for the field.
 *
 * size - Size of str array.
 *
 * delim - String containing field delimiters.
 *
 * return - length of field including terminating delimiter.
 *
 * Note, the last character of str will not be a delimiter if
 * a read error occurs, the field is too long, or the file
 * does not end with a delimiter.  Consider this an error
 * if not at end-of-file.
 *
 */
// size_t readField(File* file, char* str, size_t size, char* delim) {
//  char ch;
//  size_t n = 0;
//
//
//  while ((n + 1) < size){ // && filel.available()) {
//    ch = file->read();
//    Log.verbose(ch);
//      // Delete CR.
//      if (ch == '\r') {
//      continue;
//    }
//    str[n++] = ch;
//    if (strchr(delim, ch)) {
//        break;
//    }
//  }
//  str[n] = '\0';
//  return n;
//}

int numLines(const char *path) {
  int numberOfLines = 0;
  char ch;

  Log.verboseln("counting lines in file");
  File file = FST.open(path, "r");
  if (!file) {
    Log.verboseln("Failed to open file for reading");
    return 0;
  }

  Log.verboseln("counting lines …");
  while (file.available()) {
    ch = file.read();
    if (ch == '\n')
      numberOfLines++;
  }
  Log.verbose("%d\n", numberOfLines);
  file.close();
  return numberOfLines;
}

boolean MountFS() {
  Log.verboseln("Mount LittleFS");
  if (!FST.begin()) {
    Log.verboseln("FST mount failed");
    return false;
  }
  return true;
}

static void handleNotFound() {
  String path = server.uri();
  if (!FST.exists(path)) {
    server.send(404, "text/plain",
                "Path " + path + " not found. Please double-check the URL");
    return;
  }
  String contentType = "text/plain";
  if (path.endsWith(".css")) {
    contentType = "text/css";
  } else if (path.endsWith(".html")) {
    contentType = "text/html";
  } else if (path.endsWith(".js")) {
    contentType = "application/javascript";
  } else if (path.endsWith(".log")) {
    contentType = "text/plain";
  }
  File file = FST.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
}

void zapFiles() {
  Log.verbose("Cleaning out logfiles ... ");

  deleteFile("/HourE1.log");
  deleteFile("/HourE2.log");
  deleteFile("/HourR1.log");
  deleteFile("/HourR2.log");
  deleteFile("/HourTE.log");
  deleteFile("/HourTR.log");
  deleteFile("/HourG.log");

  deleteFile("/HourE1.log");
  deleteFile("/HourE2.log");
  deleteFile("/HourR1.log");
  deleteFile("/HourR2.log");
  deleteFile("/HourTE.log");
  deleteFile("/HourTR.log");
  deleteFile("/HourG.log");

  deleteFile("/DayE1.log");
  deleteFile("/DayE2.log");
  deleteFile("/DayR1.log");
  deleteFile("/DayR2.log");
  deleteFile("/DayTE.log");
  deleteFile("/DayTR.log");
  deleteFile("/DayG.log");

  deleteFile("/WeekE1.log");
  deleteFile("/WeekE2.log");
  deleteFile("/WeekR1.log");
  deleteFile("/WeekR2.log");
  deleteFile("/WeekTE.log");
  deleteFile("/WeekTR.log");
  deleteFile("/WeekG.log");

  deleteFile("/MonthE1.log");
  deleteFile("/MonthE2.log");
  deleteFile("/MonthR1.log");
  deleteFile("/MonthR2.log");
  deleteFile("/MonthTE.log");
  deleteFile("/MonthTR.log");
  deleteFile("/MonthG.log");

  deleteFile("/YearE1.log");
  deleteFile("/YearE2.log");
  deleteFile("/YearR1.log");
  deleteFile("/YearR2.log");
  deleteFile("/YearTE.log");
  deleteFile("/YearTR.log");
  deleteFile("/YearG.log");
  deleteFile("/YearGc.log");
  Log.verboseln("done.");
}

void zapConfig() {
  Log.verbose("Cleaning out logData files ... ");
  deleteFile("/logData.txt");
  deleteFile("/logData-1.txt");
  Log.verboseln("done.");
}

void formatFS() {
  char payload[50];
  sprintf(payload, "Formatting filesystem at %s", string2char(timestampkaal()));
  if (MQTT_debug)
    send_mqtt_message("p1wifi/logging", payload);

  FST.format();

  if (!FST.begin()) {
    Log.verboseln("FST mount failed AGAIN");
  } else {
    sprintf(payload, "Filesystem formatted at %s",
            string2char(timestampkaal()));
    if (MQTT_debug)
      send_mqtt_message("p1wifi/logging", payload);
  }
}

String totalXY(const char *typeC, String period) {
  char value[12];
  String type;
  type = typeC[0];
  type += typeC[1];
  if (period == "day") {
    if (type == "E1")
      return (String)(atof(electricityUsedTariff1) - atof(log_data.dayE1));
    if (type == "E2")
      return (String)(atof(electricityUsedTariff2) - atof(log_data.dayE2));
    if (type == "R1")
      return (String)(atof(electricityReturnedTariff1) - atof(log_data.dayR1));
    if (type == "R2")
      return (String)(atof(electricityReturnedTariff2) - atof(log_data.dayR2));
    if (type == "TE") {
      dtostrf((atof(electricityUsedTariff1) - atof(log_data.dayE1)) +
                  (atof(electricityUsedTariff2) - atof(log_data.dayE2)),
              6, 2, value);
      return (String)value;
    }
    if (type == "TR") {
      dtostrf((atof(electricityReturnedTariff1) - atof(log_data.dayR1)) +
                  (atof(electricityReturnedTariff2) - atof(log_data.dayR2)),
              6, 2, value);
      return (String)value;
    }
    if (type == "G0")
      return (String)(atof(gasReceived5min) - atof(log_data.dayG));

  } else if (period == "week") {
    if (type == "E1")
      return (String)(atof(electricityUsedTariff1) - atof(log_data.weekE1));
    if (type == "E2")
      return (String)(atof(electricityUsedTariff2) - atof(log_data.weekE2));
    if (type == "R1")
      return (String)(atof(electricityReturnedTariff1) - atof(log_data.weekR1));
    if (type == "R2")
      return (String)(atof(electricityReturnedTariff2) - atof(log_data.weekR2));
    if (type == "TE") {
      dtostrf((atof(electricityUsedTariff1) - atof(log_data.weekE1)) +
                  (atof(electricityUsedTariff2) - atof(log_data.weekE2)),
              6, 2, value);
      return (String)value;
    }
    if (type == "TR") {
      dtostrf((atof(electricityReturnedTariff1) - atof(log_data.weekR1)) +
                  (atof(electricityReturnedTariff2) - atof(log_data.weekR2)),
              6, 2, value);
      return (String)value;
    }
    if (type == "G0")
      return (String)(atof(gasReceived5min) - atof(log_data.weekG));
  } else if (period == "month") {
    if (type == "E1")
      return (String)(atof(electricityUsedTariff1) - atof(log_data.monthE1));
    if (type == "E2")
      return (String)(atof(electricityUsedTariff2) - atof(log_data.monthE2));
    if (type == "R1")
      return (String)(atof(electricityReturnedTariff1) -
                      atof(log_data.monthR1));
    if (type == "R2")
      return (String)(atof(electricityReturnedTariff2) -
                      atof(log_data.monthR2));
    if (type == "TE") {
      dtostrf((atof(electricityUsedTariff1) - atof(log_data.monthE1)) +
                  (atof(electricityUsedTariff2) - atof(log_data.monthE2)),
              6, 2, value);
      return (String)value;
    }
    if (type == "TR") {
      dtostrf((atof(electricityReturnedTariff1) - atof(log_data.monthR1)) +
                  (atof(electricityReturnedTariff2) - atof(log_data.monthR2)),
              6, 2, value);
      return (String)value;
    }
    if (type == "G0")
      return (String)(atof(gasReceived5min) - atof(log_data.monthG));
  } else if (period == "year") {
    return "Year not implemented yet";
  }
  return "fault";
}

void identifyMeter(){
  if (datagram.indexOf("ISK5\\2M550E-1011") != -1) meterName = "ISKRA AM550e-1011";
  if (datagram.indexOf("KFM5KAIFA-METER") != -1) meterName = "Kaifa  (MA105 of MA304)";
  if (datagram.indexOf("XMX5LGBBFG10") != -1) meterName = "Landis + Gyr E350";
  if (datagram.indexOf("XMX5LG") != -1) meterName = "Landis + Gyr";
  if (datagram.indexOf("Ene5\\T210-D") != -1) meterName = "Sagemcom T210-D";
  if (datagram.indexOf("FLU5\\") !=-1) {
    meterName = "Siconia";
    countryCode = 32; // Belgium
  }
  Log.verboseln(meterName.c_str());
  if (meterName != "") meternameSet = true;
}

void initTimers() {
#if DEBUG == 1
  timerAlarm.createHour(59, 0, doHourlyLog);
  timerAlarm.createHour(15, 0, doHourlyLog);
  timerAlarm.createHour(30, 0, doHourlyLog);
  timerAlarm.createHour(45, 0, doHourlyLog);
#else
  timerAlarm.createHour(59, 0, doHourlyLog);
#endif
  timerAlarm.createDay(23, 58, 0, doDailyLog);
  timerAlarm.createWeek(timerAlarm.dw_Sunday, 23, 59, 0, doWeeklyLog);
  timerAlarm.createMonth(31, 0, 0, 0, doMonthlyLog);
}

void checkCounters() {
  // see logging.ino
  if (timeIsSet && !TimeTriggersSet) {
    initTimers();
    TimeTriggersSet = true;
  }

  if (coldbootE && gotPowerReading) {

    if (needToInitLogVars) {
      doInitLogVars();
    }
    resetEnergyDaycount();
    resetEnergyMonthcount();
  }
  if (coldbootG && gotGasReading) {
    if (needToInitLogVarsGas) {
      doInitLogVarsGas();
    }
    resetGasCount();
  }

  //  if (!CHK_FLAG(logFlags, hourFlag) && ( minute() == 10 || minute() == 20 ||
  //  minute() == 30 || minute() == 40 || minute() == 50) ) doHourlyLog();

  // if (!minFlag && second() > 55) doMinutelyLog();

  // if (!hourFlag && minute() > 58) doHourlyLog();
  // if (!dayFlag && (hour() == 23) && (minute() == 59)) doDailyLog();
  // if (!weekFlag && weekday() == 1 && hour() == 23 && minute() == 59)
  // doWeeklyLog(); // day of the week (1-7), Sunday is day 1 if (!monthFlag &&
  // day() == 28 && month() == 2 && hour() == 23 && minute() == 59 && year() !=
  // 2024 && year() != 2028 ) doMonthlyLog();
  //  if (!monthFlag && day() == 29 && month() == 2 && hour() == 23 && minute()
  //  == 59 ) doMonthlyLog(); // schrikkeljaren if (!monthFlag && day() == 30 &&
  //  (month() == 4 || month() == 6 || month()== 9 || month() == 11) && hour()
  //  == 23 && minute() == 59 ) doMonthlyLog();
  if (!monthFlag && day() == 31 &&
      (month() == 1 || month() == 3 || month() == 5 || month() == 7 ||
       month() == 8 || month() == 10 || month() == 12) &&
      hour() == 23 && minute() == 59)
    doMonthlyLog();
  if (!monthFlag && day() == 31 && month() == 12 && hour() == 23 &&
      minute() == 59)
    doYearlyLog();
}

void resetFlags() {

  if (minFlag && (second() > 5) && (second() < 10))
    minFlag = false;
  if (checkMinute >= 59)
    checkMinute = 0;
  if (hourFlag &&
      ((minute() > (checkMinute + 2)) && (minute() < (checkMinute + 4))))
    hourFlag = false; // if (hourFlag &&  (minute() > 24)) hourFlag = false; //
                      // CLR_FLAG(logFlags, hourFlag);
  if (dayFlag && (day() > 0))
    dayFlag = false;
  if (weekFlag && (weekday() > 1))
    weekFlag = false;
  if (monthFlag && (day() > 0))
    monthFlag = false;
  if (yearFlag && (day() == 1) && month() == 1)
    yearFlag = false;
}

void doWatchDogs() {
  char payload[60];
  if (ESP.getFreeHeap() < 2000)
    ESP.reset(); // watchdog, in case we still have a memery leak
  if (millis() - LastSample > 300000) {
    Serial.flush();
    sprintf(payload, "No data in 300 sec, restarting monitoring: %s",
            string2char(timestampkaal()));
    if (MQTT_debug)
      send_mqtt_message("p1wifi/logging", payload);
    hourFlag = false;
    nextUpdateTime = millis() + 10000;
    OEstate = false;
    state = WAITING;
    monitoring = true;
  }
  if (softAp && (millis() - APtimer > 600000))
    ESP.reset(); // we have been in AP mode for 600 sec.
  // if (minute() == 23) hourFlag = false; // clear all flags at a safe
  // timeslot. if (minute() == 43) hourFlag = false; // clear all flags at a
  // safe timeslot. if (!monitoring && (minute() == 16 || minute() == 31 ||
  // minute() == 46)  ) monitoring = true; // kludge to make sure we keep
  // monitoring
}

char *string2char(String str) {
  if (str.length() != 0) {
    char *p = const_cast<char *>(str.c_str());
    return p;
  }
  return (char *)"";
}
