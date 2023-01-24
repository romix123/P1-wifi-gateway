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
 * @file loggingoder.ino
 * @author Ronald Leenes
 * @date 19.1.2023
 *
 *
 * @brief This file contains the logging functions for the graphs
 *
 * @see http://esp8266thingies.nl
 */

/*
 * files:
 *    HourE1.log    DayE1.log   WeekE1.log  MonthE1.log YearE1.log         
 *    HourE2.log    DayE2.log   WeekE2.log  MonthE2.log YearE2.log
 *    HourR1.log    DayR1.log   WeekR1.log  MonthR1.log YearR1.log
 *    HourR2.log    DayR2.log   WeekR2.log  MonthR2.log YearR2.log
 *    HourG.log     DayG.log    WeekG.log   MonthG.log  YearG.log   
 *    HourTE.log    DayTE.log   WeekTE.log  MonthTE.log YearTE.log   
 *    Hour.TR.log   DayTR.log   WeekTR.log  MonthTR.log YearTR.log
 *    
 *    Hour logs keep data of current 24 hours -> rotates out via hourX-1
 *    day
 *    week keeps track of daily readings for current week -> rotates out via weekX-1
 *    month keeps track of daily readings for current month -> rotates out via monthX-1
 *    year keeps track of daily readings for current year
 *    E D/W/ logs  > ['%s', %s],\n", (String)hour(), value
 *    E M logs > 
 *    
 *    DayG.log > [new Date(%s, %s, %s), %s],\n", year(), month(), day(), value);
 */

void doInitLogVars(){
  char value[12];
  // init all vars on current (first) reading
debug("Initialising log vars ... ");
  strcpy(log_data.hourE1, electricityUsedTariff1);
  strcpy(log_data.dayE1, electricityUsedTariff1);
  strcpy(log_data.weekE1, electricityUsedTariff1);
  strcpy(log_data.monthE1, electricityUsedTariff1);
  strcpy(log_data.yearE1, electricityUsedTariff1);
  
  strcpy(log_data.hourE2, electricityUsedTariff2);
  strcpy(log_data.dayE2, electricityUsedTariff2);
  strcpy(log_data.weekE2, electricityUsedTariff2);
  strcpy(log_data.monthE2, electricityUsedTariff2);
  strcpy(log_data.yearE2, electricityUsedTariff2);

  strcpy(log_data.hourR1, electricityReturnedTariff1);
  strcpy(log_data.dayR1, electricityReturnedTariff1);
  strcpy(log_data.weekR1, electricityReturnedTariff1);
  strcpy(log_data.monthR1, electricityReturnedTariff1);
  strcpy(log_data.yearR1, electricityReturnedTariff1);

  strcpy(log_data.hourR2, electricityReturnedTariff2);
  strcpy(log_data.dayR2, electricityReturnedTariff2);
  strcpy(log_data.weekR2, electricityReturnedTariff2);
  strcpy(log_data.monthR2, electricityReturnedTariff2);
  strcpy(log_data.yearR2, electricityReturnedTariff2);

  dtostrf((atof(log_data.hourE1) + atof(log_data.hourE2)), 6, 2, value);
  strcpy(log_data.hourTE, value );
  strcpy(log_data.dayTE, value );
  strcpy(log_data.weekTE, value );
  strcpy(log_data.monthTE, value );
  strcpy(log_data.yearTE, value );

  dtostrf((atof(log_data.hourR1) + atof(log_data.hourR2)), 6, 2, value);
  strcpy(log_data.hourTR, value );
  strcpy(log_data.dayTR, value );
  strcpy(log_data.weekTR, value );
  strcpy(log_data.monthTR, value );
  strcpy(log_data.yearTR, value );

  needToInitLogVars = false;
  debugln("done.");
}

void doInitLogVarsGas(){
  debugln("Initialising log GAS vars ... ");

  strcpy(log_data.hourG, gasReceived5min);
  strcpy(log_data.dayG, gasReceived5min);
  strcpy(log_data.weekG, gasReceived5min);
  strcpy(log_data.monthG, gasReceived5min);
  strcpy(log_data.yearG, gasReceived5min);  
  needToInitLogVarsGas = false;
  debugln("done.");
}


void doHourlyLog(){
  /*
   * cur - hour
   * append
   * hour = cur
   * set flag
   */
  // if (hourFlag) return;
   
   debug("Hourly log started ... ");

  char buffer[55];
  char value[12];

  dtostrf((atof(electricityUsedTariff1) - atof(log_data.hourE1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourE1.log", buffer);
  strcpy(log_data.hourE1, electricityUsedTariff1);

  dtostrf((atof(electricityUsedTariff2) - atof(log_data.hourE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourE2.log", buffer);
  strcpy(log_data.hourE2, electricityUsedTariff2);

  dtostrf((atof(electricityReturnedTariff1) - atof(log_data.hourR1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourR1.log", buffer);
  strcpy(log_data.hourR1, electricityReturnedTariff1);

  dtostrf((atof(electricityReturnedTariff2) - atof(log_data.hourR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourR2.log", buffer);
  strcpy(log_data.hourR2, electricityReturnedTariff2);
  
  dtostrf((atof(gasReceived5min) - atof(log_data.hourG)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourG.log", buffer);
  strcpy(log_data.hourG, gasReceived5min);

  dtostrf((atof(log_data.hourE1) + atof(log_data.hourE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourTE.log", buffer);
  strcpy(log_data.hourTE, value );

  dtostrf((atof(log_data.hourR1) + atof(log_data.hourR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", (String)hour(), value);
  appendFile("/HourTR.log", buffer);
  strcpy(log_data.hourTR, value );

  // save state to file
  deleteFile("/logData1.txt");
  renameFile("/logData.txt", "/logData1.txt");
  File file = LittleFS.open("/logData.txt", "w");
  file.write((byte *)&log_data, sizeof(log_data));
  file.close();

  SET_FLAG(logFlags, hourFlag);
  //hourFlag = true;
  debugln("completed.");
}

void doDailyLog(){
  /*
   * today = cur - day
   * append
   * week = week + today
   * month = month + today
   * year = year + today
   * day = cur
   * set flag
   */
   monitoring = false;
   debug("Dayly log started ... ");

   String str ="";
   char buffer[60];
   char value[12];

if (month() < 10) str += "0";
  str += month();
  if ((day()) < 10) str += "0";
  str += day();
  
  
  dtostrf((atof(electricityUsedTariff1) - atof(log_data.dayE1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayE1.log", buffer);
  appendFile("/WeekE1.log", buffer);
  appendFile("/MonthE1.log", buffer);
  appendFile("/YearE1.log", buffer);
  strcpy(log_data.dayE1, electricityUsedTariff1);

  dtostrf((atof(electricityUsedTariff2) - atof(log_data.dayE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayE2.log", buffer);
  appendFile("/WeekE2.log", buffer);
  appendFile("/MonthE2.log", buffer);
  appendFile("/YearE2.log", buffer);
  strcpy(log_data.dayE2, electricityUsedTariff2);

  dtostrf((atof(electricityReturnedTariff1) - atof(log_data.dayR1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayR1.log", buffer);
  appendFile("/WeekR1.log", buffer);
  appendFile("/MonthR1.log", buffer);
  appendFile("/YearR1.log", buffer);
  strcpy(log_data.dayR1, electricityReturnedTariff1);

  dtostrf((atof(electricityReturnedTariff2) - atof(log_data.dayR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayR2.log", buffer);
  appendFile("/WeekR2.log", buffer);
  appendFile("/MonthR2.log", buffer);
  appendFile("/YearR2.log", buffer);
  strcpy(log_data.dayR2, electricityReturnedTariff2);
  
  dtostrf((atof(gasReceived5min) - atof(log_data.dayG)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayG.log", buffer); 
  appendFile("/WeekG.log", buffer);
  appendFile("/MonthG.log", buffer);
  appendFile("/YearG.log", buffer);  
  sprintf_P(buffer, PSTR("(%d,%d,%d), %s],\n"), year(), month()-1, day(), value);
 // sprintf(buffer, "(%s , %s , %s), %s],\n", year(), 0, day(), value); // Google graph format uses month 0 for Jan!!
  appendFile("/YearGc.log", buffer);
  strcpy(log_data.dayG, gasReceived5min);

  dtostrf((atof(log_data.dayE1) + atof(log_data.dayE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayTE.log", buffer);
  appendFile("/WeekTE.log", buffer);
  appendFile("/MonthTE.log", buffer);
  appendFile("/YearTE.log", buffer);  
  strcpy(log_data.dayTE, value );

  dtostrf((atof(log_data.dayR1) + atof(log_data.dayR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
//  appendFile("/DayTR.log", buffer);
  appendFile("/WeekTR.log", buffer);
  appendFile("/MonthTR.log", buffer);
  appendFile("/YearTR.log", buffer);  
  strcpy(log_data.dayTR, value );


  deleteFile("/HourE1.log");
  deleteFile("/HourE2.log");
  deleteFile("/HourR1.log");
  deleteFile("/HourR2.log");
  deleteFile("/HourTE.log");
  deleteFile("/HourTR.log");
  deleteFile("/HourG.log");
  
  SET_FLAG(logFlags, dayFlag);
  debugln("completed.");
monitoring = true;
}

void doWeeklyLog(){
  /*  
   *   append week
   *   week cur
   *   set flag
   */
  debug("Weekly log started. It was a Sunday evening ... ");
   String str ="";
   char buffer[55];
   char value[12];
   
  if (month() < 10) str += "0";
  str += month();
  if ((day()) < 10) str += "0";
  str += day();


  deleteFile("/WeekE1-1.log");
  renameFile("/WeekE1.log", "/WeekE1-1.log");
  dtostrf((atof(electricityUsedTariff1) - atof(log_data.weekE1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksE1.log", buffer);
  strcpy(log_data.weekE1, electricityUsedTariff1);

  deleteFile("/WeekE2-1.log");
  renameFile("/WeekE2.log", "/WeekE2-1.log");
  dtostrf((atof(electricityUsedTariff2) - atof(log_data.weekE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksE2.log", buffer);
  strcpy(log_data.weekE2, electricityUsedTariff2);

  deleteFile("/WeekR1-1.log");
  renameFile("/WeekR1.log", "/WeekR1-1.log");
  dtostrf((atof(electricityReturnedTariff1) - atof(log_data.weekR1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksR1.log", buffer);
  strcpy(log_data.weekR1, electricityReturnedTariff1);

  deleteFile("/WeekR2-1.log");
  renameFile("/WeekR2.log", "/WeekR2-1.log");
  dtostrf((atof(electricityReturnedTariff2) - atof(log_data.weekR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksR2.log", buffer);
  strcpy(log_data.weekR2, electricityReturnedTariff2);

  deleteFile("/WeekTE-1.log");
  renameFile("/WeekTE.log", "/WeekTE-1.log");
  dtostrf((atof(electricityUsedTariff1) - atof(log_data.weekE1)) + (atof(electricityUsedTariff2) - atof(log_data.weekE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksTE.log", buffer);
  strcpy(log_data.weekTE, value);

  deleteFile("/WeekTR-1.log");
  renameFile("/WeekTR.log", "/WeekTR-1.log");
  dtostrf((atof(electricityReturnedTariff1) - atof(log_data.weekR1)) + (atof(electricityReturnedTariff2) - atof(log_data.weekR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksTR.log", buffer);
  strcpy(log_data.weekTR, value);

  deleteFile("/WeekG-1.log");
  renameFile("/WeekG.log", "/WeekG-1.log");
  dtostrf((atof(gasReceived5min) - atof(log_data.dayG)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/WeeksG.log", buffer);
  strcpy(log_data.weekG, value);
  
  SET_FLAG(logFlags, weekFlag);
  debugln("completed.");
}

void doMonthlyLog(){
  /*
   * append month
   * month = cur
   * set flag
   */

    debug("Monthlyly log started ... ");
   String str ="";
   char buffer[55];
   char value[12];

  if (month() < 10) str += "0";
  str += month();
  if ((day()) < 10) str += "0";
  str += day();


  deleteFile("/MonthE1-1.log");
  renameFile("/MonthE1.log", "/MonthE1-1.log");
  dtostrf((atof(electricityUsedTariff1) - atof(log_data.monthE1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsE1.log", buffer);
  strcpy(log_data.monthE1, electricityUsedTariff1);

  deleteFile("/MonthE2-1.log");
  renameFile("/MonthE2.log", "/MonthE2-1.log");
  dtostrf((atof(electricityUsedTariff2) - atof(log_data.monthE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsE2.log", buffer);
  strcpy(log_data.monthE2, electricityUsedTariff2);

  deleteFile("/MonthR1-1.log");
  renameFile("/MonthR1.log", "/MonthR1-1.log");
  dtostrf((atof(electricityReturnedTariff1) - atof(log_data.monthR1)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsR1.log", buffer);
  strcpy(log_data.monthR1, electricityReturnedTariff1);

  deleteFile("/MonthR2-1.log");
  renameFile("/MonthR2.log", "/MonthR2-1.log");
  dtostrf((atof(electricityReturnedTariff2) - atof(log_data.monthR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsR2.log", buffer);
  strcpy(log_data.monthR2, electricityReturnedTariff2);

  deleteFile("/MonthTE-1.log");
  renameFile("/MonthTE.log", "/MonthTE-1.log");
  dtostrf((atof(electricityUsedTariff1) - atof(log_data.monthE1)) + (atof(electricityUsedTariff2) - atof(log_data.monthE2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsTE.log", buffer);
  strcpy(log_data.monthTE, value);

  deleteFile("/MonthTR-1.log");
  renameFile("/MonthTR.log", "/MonthTR-1.log");
  dtostrf((atof(electricityReturnedTariff1) - atof(log_data.monthR1)) + (atof(electricityReturnedTariff2) - atof(log_data.monthR2)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsTR.log", buffer);
  strcpy(log_data.monthTR, value);

  deleteFile("/MonthG-1.log");
  renameFile("/MonthG.log", "/MonthG-1.log");
  dtostrf((atof(gasReceived5min) - atof(log_data.dayG)), 6, 2, value);
  sprintf(buffer, "['%s', %s],\n", str, value);
  appendFile("/MonthsG.log", buffer);
  strcpy(log_data.monthG, value);
  
  SET_FLAG(logFlags, monthFlag);
  debugln("completed.");
}

void doYearlyLog(){
  /*
   * append year
   * year = cur
   * set flag
   */
}



void resetEnergyDaycount(){
#ifdef SWEDISH  
      strcpy(log_data.dayE1, cumulativeActiveImport);
      strcpy(log_data.dayE2, cumulativeReactiveImport);
      strcpy(log_data.dayR1, cumulativeActiveExport);
      strcpy(log_data.dayR2, cumulativeReactiveExport);
#else
      strcpy(log_data.dayE1, electricityUsedTariff1);
      strcpy(log_data.dayE2, electricityUsedTariff2);
      strcpy(log_data.dayR1, electricityReturnedTariff1);
      strcpy(log_data.dayR2, electricityReturnedTariff2);
#endif
   coldbootE = false;
}

void resetEnergyMonthcount() {
#ifdef SWEDISH  
      strcpy(log_data.monthE1, cumulativeActiveImport);
      strcpy(log_data.monthE2, cumulativeReactiveImport);
      strcpy(log_data.monthR1, cumulativeActiveExport);
      strcpy(log_data.monthR2, cumulativeReactiveExport);
#else
      strcpy(log_data.monthE1, electricityUsedTariff1);
      strcpy(log_data.monthE2, electricityUsedTariff2);
      strcpy(log_data.monthR1, electricityReturnedTariff1);
      strcpy(log_data.monthR2, electricityReturnedTariff2);
#endif
}

void resetGasCount(){
   strcpy(log_data.dayG, gasReceived5min);
   coldbootG = false;
}

void DirListing(){
  String str; 
  char buffer[55];
  str += F("<html>\n<head>\n");
  str += F("<title>Slimme meter</title>");
  str += F("</head><body>\n");

Dir root = LittleFS.openDir("/");
 while (root.next()) {
    File file = root.openFile("r");
    str += ("  FILE: ");
    
    str += (root.fileName());
    str +=("  SIZE: ");
    str +=((String)file.size());
    str +=("<br>");

   // time_t cr = file.getCreationTime();
   // time_t lw = file.getLastWrite();
    file.close();
  //  struct tm * tmstruct = localtime(&cr);
  // sprintf(buffer, "    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
 //  server.sendContent(buffer);
 //   tmstruct = localtime(&lw);
 //  sprintf(buffer, "  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  //   server.sendContent(buffer); 
  }
  str += F("</body></html>\n");
  server.send(200, "text/html", str);
}
