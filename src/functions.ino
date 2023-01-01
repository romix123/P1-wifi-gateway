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
//  String tm;
//  tm = "time: ";
//  tm += hour();
//  tm += ":" + minute() + ":" + second();
  return (String) "time: " + hour() + ":" + minute() + ":" + second();
  
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
