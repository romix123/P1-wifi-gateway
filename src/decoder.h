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

/*
      P1_pos = inputString.indexOf("1-0:1.7.0", pos282 + 1);
      P1 = inputString.substring(P1_pos + 10, P1_pos + 17);
*/

/**
 * @file decoder.ino
 * @author Ronald Leenes
 *
 * @brief This file contains the OBIS parser functions
 *
 * @see http://esp8266thingies.nl
 */
 void decodeLine(int len) {
  int startChar = FindCharInArray(telegram, '/', len);
  int endChar = FindCharInArray(telegram, '!', len);
  bool validCRCFound = false;

   if (state == WAITING) {      // we're waiting for a valid start sequence, if this line is not it, just return
     if(startChar >= 0) { //start found. Reset CRC calculation
       currentCRC=CRC16(0x0000,(unsigned char *) telegram+startChar, len-startChar);
       // and reset datagram
         datagram = "";
         datagramValid = false;
         dataEnd = false;
       debugln("\n\rStart found!");
       state = READING;
       nextUpdateTime = millis() + interval; // set trigger for next round

       for(int cnt=startChar; cnt<len-startChar;cnt++) datagram += telegram[cnt];
       if (!meternameSet) identifyMeter();
       return;
     } else {
       return;        // We're waiting for a valid start char, if we're in the middle of a datagram, just return.
     }
   }

   if (state == READING) {
   if(endChar>=0){   // we have found the endchar !
     state = CHECKSUM;
     //add to crc calc
     dataEnd = true;         // we're at the end of the data stream, so mark (for raw data output) We don't know if the data is valid, we will test this below.
   //  gas22Flag=false;        // assume we have also collected the Gas value
     currentCRC=CRC16(currentCRC,(unsigned char*)telegram+endChar, 1);
     char messageCRC[4];
     strncpy(messageCRC, telegram + endChar + 1, 4);
     if (datagram.length() < 2048){
       for(int cnt=0; cnt<len;cnt++) datagram += telegram[cnt];
       datagram += "\r";
       datagram += "\n";
     } else  {
         debugln("Buffer overflow???");
         state = FAULT;
         return;
       }

     validCRCFound = (strtoul(messageCRC, NULL, 16) == currentCRC);

       if(validCRCFound) {
         debugln("\nVALID CRC FOUND!");
         state = DONE;
         datagramValid = true;
         dataFailureCount = 0;
         LastSample = millis();
 //      gotPowerReading = true; // we at least got electricty readings. Not all setups have a gas meter attached, so gotGasReading is handled when we actually get gasIds coming in
         return;
       }    else {
         debugln("\n===INVALID CRC FOUND!===");
         dataFailureCount++;
         state = FAILURE;
         return;
       }
     }   else  { // no endchar, so normal line, process
       currentCRC=CRC16(currentCRC, (unsigned char*)telegram, len);
       for(int cnt=0; cnt<len;cnt++) datagram += telegram[cnt];
       OBISparser(len);
     }
     return;
   } //state = reading

   debugln("how did we get here??");
   return;
 }

 String readUntilStar(int start, int end){
   String value = "";
   int i = start + 1;
   bool trailingZero = true;
   while ((telegram[i] != '*') && (i < end)) {
       if (trailingZero && telegram[i] != '0' ) trailingZero = false;
       if (telegram[i] == '0' && telegram[i+1] == '.') {
        // value += (char)telegram[i];
         trailingZero = false;
       }
       if (!trailingZero) value += (char)telegram[i];
       i++;
   }
   return value;
 }

String readBetweenDoubleParenthesis(int start, int end){
   String value = "";
   int i = start + 1;
   while ((telegram[i] != ')') && (telegram[i+1] != '(')) i++; // we have found the intersection of the command and data
                                                               // 0-1:24.2.1(231029141500W)(05446.465*m3)
   i++;
   value = readUntilStar(i, end);
   return value;
 }

String readFirstParenthesisVal(int start, int end){
   String value = "";
   int i = start + 1;
   bool trailingZero = true;
   while ((telegram[i] != ')') && (i < end)) {
       if (trailingZero && telegram[i] == '0' ) {
         i++;
       } else {
         value += (char)telegram[i];
         trailingZero = false;
         i++;
       }
   }
   return value;
 }

 void OBISparser(int len){
  int i;
  String value;
  String inString = "";
  int idx;

  for (i = 0; i < len; i++){
      if (telegram[i]== '(') break;
      if (isDigit(telegram[i])){
         // convert the incoming byte to a char and add it to the string:
      inString += (char)telegram[i];
      }
    }
    idx = inString.toInt();
    switch (idx) {
      case 0:
        break;
      case 13028: // device type
        readFirstParenthesisVal(i, len).toCharArray(P1version, sizeof(P1version));
                if (P1version[0] =='4') P1prot = 4; else P1prot = 5;
        break;
      case 100:
        readFirstParenthesisVal(i, len).toCharArray(P1timestamp, sizeof(P1timestamp));
        debug("timestamp: ");
        debugln(P1timestamp);
        break;

      case 96140:
        readFirstParenthesisVal(i, len).toCharArray(tariffIndicatorElectricity, sizeof(tariffIndicatorElectricity));
        debug("tarief: ");
        debugln(tariffIndicatorElectricity);
        break;
      case 9611:
        readFirstParenthesisVal(i, len).toCharArray(equipmentId, sizeof(equipmentId));
        debug("meter id: ");
        debugln(equipmentId);
        break;
      case 19610:
        readFirstParenthesisVal(i, len).toCharArray(equipmentId2, sizeof(equipmentId2));
        debug("meter id2: ");
        debugln(equipmentId2);
        break;
      case 9810:
        debugln("storingen");
        break;
      case 10170: // 1-0:1.7.0 – actualElectricityPowerDelivered
        readUntilStar(i, len).toCharArray(actualElectricityPowerDeli, sizeof(actualElectricityPowerDeli));
        debug("1-0:1.7.x > ");
        debugln(actualElectricityPowerDeli);
        break;
      case 10270: // 1-0:1.7.0 – actualElectricityPowerReturned
        readUntilStar(i, len).toCharArray(actualElectricityPowerRet, sizeof(actualElectricityPowerRet));
        debug("1-0:2.7.0 > ");
        debugln(actualElectricityPowerRet);
        break;

      case 10180: // 1-0:1.8.0 – actualElectricityPowerDelivered
        debug("non existent ");
        readUntilStar(i, len).toCharArray(actualElectricityPowerDeli, sizeof(actualElectricityPowerDeli));
        debug("1-0:1.7.x > ");
        debugln(actualElectricityPowerDeli);
        break;
      case 10181:  // 1-0:1.8.1(000992.992*kWh) Elektra verbruik laag tarief
        readUntilStar(i, len).toCharArray(electricityUsedTariff1, sizeof(electricityUsedTariff1));
        debug("1-0:1.8.1 > ");
        debugln(electricityUsedTariff1);
        break;
      case 10182: // 1-0:1.8.2(000560.157*kWh) = Elektra verbruik hoog tarief
        readUntilStar(i, len).toCharArray(electricityUsedTariff2, sizeof(electricityUsedTariff2));
        debug("1-0:1.8.2 > ");
        debugln(electricityUsedTariff2);
        break;
      case 10281: // 1-0:2.8.1(000348.890*kWh) Elektra opbrengst laag tarief
        readUntilStar(i, len).toCharArray(electricityReturnedTariff1, sizeof(electricityReturnedTariff1));
        debug("1-0:2.8.1 > ");
        debugln(electricityReturnedTariff1);
        break;
      case 10282: // 1-0:2.8.2(000859.885*kWh) Elektra opbrengst hoog tarief
        readUntilStar(i, len).toCharArray(electricityReturnedTariff2, sizeof(electricityReturnedTariff2));
        debug("1-0:2.8.2 > ");
        debugln(electricityReturnedTariff2);
        break;

       case 103170:  // 1-0:31.7.0(002*A) Instantane stroom Elektriciteit L1
        readUntilStar(i, len).toCharArray(instantaneousCurrentL1, sizeof(instantaneousCurrentL1));
        debug("1-0:31.7.0 > ");
        debugln(instantaneousCurrentL1);
        break;
       case 105170:  // 1-0:51.7.0(002*A) Instantane stroom Elektriciteit L2
        readUntilStar(i, len).toCharArray(instantaneousCurrentL2, sizeof(instantaneousCurrentL2));
        debug("1-0:51.7.0 > ");
        debugln(instantaneousCurrentL2);
        break;
       case 107170:  // 1-0:71.7.0(002*A) Instantane stroom Elektriciteit L3
        readUntilStar(i, len).toCharArray(instantaneousCurrentL3, sizeof(instantaneousCurrentL3));
        debug("1-0:71.7.0 > ");
        debugln(instantaneousCurrentL3);
        break;


       case 103270: // 1-0:32.7.0(232.0*V) Voltage L1
        readUntilStar(i, len).toCharArray(instantaneousVoltageL1, sizeof(instantaneousVoltageL1));
        debug("1-0:32.7.0 > ");
        debugln(instantaneousVoltageL1);
        break;
       case 105270: // 1-0:52.7.0(232.0*V) Voltage L2
        readUntilStar(i, len).toCharArray(instantaneousVoltageL2, sizeof(instantaneousVoltageL2));
        debug("1-0:32.7.0 > ");
        debugln(instantaneousVoltageL2);
        break;
       case 107270: // 1-0:72.7.0(232.0*V) Voltage L3
        readUntilStar(i, len).toCharArray(instantaneousVoltageL3, sizeof(instantaneousVoltageL3));
        debug("1-0:72.7.0 > ");
        debugln(instantaneousVoltageL3);
        break;


       case 102170:  // 1-0:21.7.0(002*A) Instantane stroom Elektriciteit L1
        readUntilStar(i, len).toCharArray(activePowerL1P, sizeof(activePowerL1P));
        debug("1-0:21.7.0 > ");
        debugln(activePowerL1P);
        break;
       case 104170:  // 1-0:41.7.0(002*A) Instantane stroom Elektriciteit L2
        readUntilStar(i, len).toCharArray(activePowerL2P, sizeof(activePowerL2P));
        debug("1-0:41.7.0 > ");
        debugln(activePowerL2P);
        break;
      case 106170:  // 1-0:61.7.0(002*A) Instantane stroom Elektriciteit L3
        readUntilStar(i, len).toCharArray(activePowerL3P, sizeof(activePowerL3P));
        debug("1-0:61.7.0 > ");
        debugln(activePowerL3P);
        break;

       case 102270: // 1-0:22.7.0(232.0*V) Voltage L1
        readUntilStar(i, len).toCharArray(activePowerL1NP, sizeof(activePowerL1NP));
        debug("1-0:22.7.0 > ");
        debugln(activePowerL1NP);
        break;
       case 104270: // 1-0:42.7.0(232.0*V) Voltage L2
        readUntilStar(i, len).toCharArray(activePowerL2NP, sizeof(activePowerL2NP));
        debug("1-0:42.7.0 > ");
        debugln(activePowerL2NP);
        break;
       case 106270: // 1-0:62.7.0(232.0*V) Voltage L3
        readUntilStar(i, len).toCharArray(activePowerL3NP, sizeof(activePowerL3NP));
        debug("1-0:62.7.0 > ");
        debugln(activePowerL3NP);
        break;


       case 1032320: // Aantal korte spanningsdalingen Elektriciteit in fase 1
        readFirstParenthesisVal(i, len).toCharArray(numberVoltageSagsL1, sizeof(numberVoltageSagsL1));
        debug("1-0:32.32.0 > ");
        debugln(numberVoltageSagsL1);
        break;
       case 1052320: // Aantal korte spanningsdalingen Elektriciteit in fase 2
        readFirstParenthesisVal(i, len).toCharArray(numberVoltageSagsL2, sizeof(numberVoltageSagsL2));
        debug("1-0:52.32.0 > ");
        debugln(numberVoltageSagsL2);
        break;
       case 1072320: // Aantal korte spanningsdalingen Elektriciteit in fase 3
        readFirstParenthesisVal(i, len).toCharArray(numberVoltageSagsL3, sizeof(numberVoltageSagsL3));
        debug("1-0:72.32.0 > ");
        debugln(numberVoltageSagsL3);
        break;

       case 1032360: // 1-0:32.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 1
        readFirstParenthesisVal(i, len).toCharArray(numberVoltageSwellsL1, sizeof(numberVoltageSwellsL1));
        debug("1-0:32.36.0 > ");
        debugln(numberVoltageSwellsL1);
        break;
       case 1052360: // 1-0:52.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 2
        readFirstParenthesisVal(i, len).toCharArray(numberVoltageSwellsL2, sizeof(numberVoltageSwellsL2));
        debug("1-0:52.36.0 > ");
        debugln(numberVoltageSwellsL2);
        break;
       case 1072360: // 1-0:72.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 3
        readFirstParenthesisVal(i, len).toCharArray(numberVoltageSwellsL3, sizeof(numberVoltageSwellsL3));
        debug("1-0:72.36.0 > ");
        debugln(numberVoltageSwellsL3);
        break;

       case 12421: // gas
        readBetweenDoubleParenthesis(i, len).toCharArray(gasReceived5min, sizeof(gasReceived5min));
        readBetweenDoubleParenthesis(i, len).toCharArray(gasDomoticz, sizeof(gasDomoticz));
        debug("gas: 0-1:24.2.1 > ");
        debugln(gasReceived5min);
        break;
      case 96721: // 0-0:96.7.21(00051)  Number of power failures in any phase
        readFirstParenthesisVal(i, len).toCharArray(numberPowerFailuresAny, sizeof(numberPowerFailuresAny));
        debug("0-0:96.7.21 > ");
        debugln(numberPowerFailuresAny);
        break;
      case 9679: // 0-0:96.7.9(00007) Number of long power failures in any phase
        readFirstParenthesisVal(i, len).toCharArray(numberLongPowerFailuresAny, sizeof(numberLongPowerFailuresAny));
        debug("0-0:96.7.21 > ");
        debugln(numberLongPowerFailuresAny);
        break;
      case 1099970: //1-0:99.97.0(6) Power Failure Event Log (long power failures)
        longPowerFailuresLog = "";
        while (i < len) {
          longPowerFailuresLog += (char)telegram[i];
          i++;
        }
        break;
      default:
        debug(" onbekende OBIS: ");
        statusMsg = "Onbekende OBIS: ";
        while (i < len) {
          debug((char)telegram[i]);
          statusMsg += (char)telegram[i];
          i++;
        }
        debugln();
        break;
    }
      // clear the string for new input:
      inString = "";
}
