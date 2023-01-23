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
void blink(int t){
  for (int i=0 ; i <=t; i++){
    LEDon       // Signaal naar laag op ESP-M3
    delay(200); // wacht 200 millisec
    LEDoff;     // LEDoff, signaal naar hoog op de ESP-M3
  }
}

void RTS_on(){            // switch on Data Request
  digitalWrite(OE, LOW);  // enable buffer
  digitalWrite(DR, HIGH); // turn on Data Request
  OEstate = true;
   debug("Data request on at ");
   debugln(millis());
}

void RTS_off(){           // switch off Data Request
  digitalWrite(DR, LOW);  // turn off Data Request 
  digitalWrite(OE, HIGH); // put buffer in Tristate mode
  OEstate = false;
  nextUpdateTime = millis() + interval;
   debug("Data request off at ");
   debug(millis());
   debug(" nextUpdateTime: ");
   debugln(nextUpdateTime);
   
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

void settime(){
  //(hr,min,sec,day,mnth,yr);
  //YYMMDDhhmmssX
  setTime(NUM(6,10) + NUM(7,1), NUM(8, 10) + NUM(9, 1), NUM(10, 10) + NUM(11, 1), NUM(4, 10)+ NUM(5, 1), NUM(2,10) + NUM(3, 1), NUM(0, 10) + NUM(1,1)); 
  debug(timestamp());
}

String timestamp(){
  return (String) "time: " + hour() + ":" + minute() + ":" + second();
}

String timestampkaal(){
    return (String) hour() + ":" + minute() + ":" + second();
}

void createToken(){
  char HexList[] ="1234567890ABCDEF";

  debug("Token: ");
  for (int i = 0; i < 16; i++){
    setupToken[i] = HexList[random(0, 16)];
    debug(setupToken[i]);
  }
  debugln();
  setupToken[16] = '\0';
}

void listDir(const char * dirname) {
  debugf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    debug("  FILE: ");
    debug(root.fileName());
    debug("  SIZE: ");
    debug(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
   Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
   Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}


void readFile(const char * path) {
  debugf("Reading file: %s\n", path);
  debugln();
  File file = LittleFS.open(path, "r");
  if (!file) {
    debugln("Failed to open file for reading");
    return;
  }

  debugln("Read from file: ");
  while (file.available()) {
    debug(String((char)file.read()));
  }
  debugln();
  file.close();
}

void writeFile(const char * path, const char * message) {
  debugf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    debugln("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    debugln("File written");
  } else {
    debugln("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void appendFile(const char * path, const char * message) {
  debugf("Appending to file: %s\n", path);

  File file = LittleFS.open(path, "a");
  if (!file) {
    debugln("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    debug("Message appended: ");
    debugln(message);
  } else {
    debugln("Append failed");
  }
  file.close();
}

void renameFile(const char * path1, const char * path2) {
  debugf("Renaming file %s ", path1);
  debugf("to %s\n", path2);
  if (LittleFS.rename(path1, path2)) {
    debugln("File renamed");
  } else {
    debugln("Rename failed");
  }
}

void deleteFile(const char * path) {
  debugf("Deleting file: %s\n", path);
  if (LittleFS.remove(path)) {
    debugln("File deleted");
  } else {
    debugln("Delete failed");
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
size_t readField(File* file, char* str, size_t size, char* delim) {
  char ch;
  size_t n = 0;

  
  while ((n + 1) < size){ // && filel.available()) {
    ch = file->read(); 
    debug(ch);
      // Delete CR.
      if (ch == '\r') {
      continue;
    }
    str[n++] = ch;
    if (strchr(delim, ch)) {
        break;
    }
  }
  str[n] = '\0';
  return n;
}



int numLines(const char * path){
  int numberOfLines = 0;
  char ch;
  
  debugln("counting lines in file");
    File file = LittleFS.open(path, "r");
  if (!file) {
    debugln("Failed to open file for reading");
    return 0;
  }

  debugln("counting lines …");
  while (file.available()) {
    ch = file.read();
    if (ch == '\n')  numberOfLines++;
  }
  debugln(numberOfLines);
  file.close();
  return numberOfLines;
}


boolean MountFS(){
    debugln("Mount LittleFS");
  if (!LittleFS.begin()) {
    debugln("LittleFS mount failed");
    return false;
  }
  return true;
}


static void handleNotFound() {
  String path = server.uri();
  if (!LittleFS.exists(path)) {
    server.send(404, "text/plain", "Path " + path + " not found. Please double-check the URL");
    return;
  }
  String contentType = "text/plain";
  if (path.endsWith(".css")) {
    contentType = "text/css";
  }
  else if (path.endsWith(".html")) {
    contentType = "text/html";
  }
  else if (path.endsWith(".js")) {
    contentType = "application/javascript";
  } else if (path.endsWith(".log")) {
    contentType = "text/plain";
  }
  File file = LittleFS.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
}


void zapFiles(){
  debug("Cleaning out logfiles ... ");

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
debugln("done.");
}

void zapConfig(){
  debug("Cleaning out logData files ... ");
  deleteFile("/logData.txt");
  deleteFile("/logData-1.txt");
debugln("done.");
}
