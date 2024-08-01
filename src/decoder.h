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
       Log.verboseln("\n\rStart found!");
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
     char *telegramCRCStart = telegram + endChar + 1;
     std::string messageCRC(telegramCRCStart, telegramCRCStart + 4);
     if (datagram.length() < 2048){
       for(int cnt=0; cnt<len;cnt++) datagram += telegram[cnt];
       datagram += "\r";
       datagram += "\n";
     } else  {
         Log.verboseln("Buffer overflow???");
         state = FAULT;
         return;
       }

     validCRCFound = (std::stoul(messageCRC, NULL, 16) == currentCRC);

       if(validCRCFound) {
         Log.infoln("\nVALID CRC FOUND!");
         state = DONE;
         datagramValid = true;
         dataFailureCount = 0;
         LastSample = millis();
 //      gotPowerReading = true; // we at least got electricty readings. Not all setups have a gas meter attached, so gotGasReading is handled when we actually get gasIds coming in
         return;
       }    else {
         Log.errorln("\n===INVALID CRC FOUND!===");
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

   Log.verboseln("how did we get here??");
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
        dsmrData.p1_version = readFirstParenthesisVal(i, len);
        break;
      case 100:
        dsmrData.timestamp = readFirstParenthesisVal(i, len);
        Log.verbose("timestamp: ");
        Log.verboseln(dsmrData.timestamp.c_str());
        break;
      case 96140:
        dsmrData.electricity_tariff = readFirstParenthesisVal(i, len);
        Log.verbose("tarief: ");
        Log.verboseln(dsmrData.electricity_tariff.c_str());
        break;
      case 9611:
        dsmrData.equipment_id = readFirstParenthesisVal(i, len);
        Log.verbose("meter id: ");
        Log.verboseln(dsmrData.equipment_id.c_str());
        break;
      case 19610:
        dsmrData.gas_equipment_id = readFirstParenthesisVal(i, len);
        Log.verbose("meter id2: ");
        Log.verboseln(dsmrData.gas_equipment_id.c_str());
        break;
      case 9810:
        Log.verboseln("storingen");
        break;
      case 10170: // 1-0:1.7.0 – actualElectricityPowerDelivered
        dsmrData.power_delivered = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:1.7.x > ");
        Log.verboseln("%F", dsmrData.power_delivered.val());
        break;
      case 10270: // 1-0:1.7.0 – actualElectricityPowerReturned
        dsmrData.power_returned = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:2.7.0 > ");
        Log.verboseln("%F", dsmrData.power_returned.val());
        break;
      case 10180: // 1-0:1.8.0 – actualElectricityPowerDelivered
        Log.verbose("non existent ");
        dsmrData.power_delivered = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:1.7.x > ");
        Log.verboseln("%F", dsmrData.power_delivered.val());
        break;
      case 10181:  // 1-0:1.8.1(000992.992*kWh) Elektra verbruik laag tarief
        dsmrData.energy_delivered_tariff1 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:1.8.1 > ");
        Log.verboseln("%F", dsmrData.energy_delivered_tariff1.val());
        break;
      case 10182: // 1-0:1.8.2(000560.157*kWh) = Elektra verbruik hoog tarief
        dsmrData.energy_delivered_tariff2 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:1.8.2 > ");
        Log.verboseln("%F", dsmrData.energy_delivered_tariff2.val());
        break;
      case 10281: // 1-0:2.8.1(000348.890*kWh) Elektra opbrengst laag tarief
        dsmrData.energy_returned_tariff1 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:2.8.1 > ");
        Log.verboseln("%F", dsmrData.energy_returned_tariff1.val());
        break;
      case 10282: // 1-0:2.8.2(000859.885*kWh) Elektra opbrengst hoog tarief
        dsmrData.energy_returned_tariff2 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:2.8.2 > ");
        Log.verboseln("%F", dsmrData.energy_returned_tariff2.val());
        break;
      case 103170:  // 1-0:31.7.0(002*A) Instantane stroom Elektriciteit L1
        dsmrData.current_l1 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:31.7.0 > ");
        Log.verboseln("%F", dsmrData.current_l1.val());
        break;
      case 105170:  // 1-0:51.7.0(002*A) Instantane stroom Elektriciteit L2
        dsmrData.current_l2 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:51.7.0 > ");
        Log.verboseln("%F", dsmrData.current_l2.val());
        break;
      case 107170:  // 1-0:71.7.0(002*A) Instantane stroom Elektriciteit L3
        dsmrData.current_l3 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:71.7.0 > ");
        Log.verboseln("%F", dsmrData.current_l3.val());
        break;
      case 103270: // 1-0:32.7.0(232.0*V) Voltage L1
        dsmrData.voltage_l1 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:32.7.0 > ");
        Log.verboseln("%F", dsmrData.voltage_l1.val());
        break;
      case 105270: // 1-0:52.7.0(232.0*V) Voltage L2
        dsmrData.voltage_l2 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:32.7.0 > ");
        Log.verboseln("%F", dsmrData.voltage_l2.val());
        break;
      case 107270: // 1-0:72.7.0(232.0*V) Voltage L3
        dsmrData.voltage_l3 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:72.7.0 > ");
        Log.verboseln("%F", dsmrData.voltage_l3.val());
        break;
      case 102170:  // 1-0:21.7.0(002*A) Instantane stroom Elektriciteit L1
        dsmrData.power_delivered_l1 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:21.7.0 > ");
        Log.verboseln("%F", dsmrData.power_delivered_l1.val());
        break;
      case 104170:  // 1-0:41.7.0(002*A) Instantane stroom Elektriciteit L2
        dsmrData.power_delivered_l2 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:41.7.0 > ");
        Log.verboseln("%F", dsmrData.power_delivered_l2.val());
        break;
      case 106170:  // 1-0:61.7.0(002*A) Instantane stroom Elektriciteit L3
        dsmrData.power_delivered_l3 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:61.7.0 > ");
        Log.verboseln("%F", dsmrData.power_delivered_l3.val());
        break;
      case 102270: // 1-0:22.7.0(232.0*V) Voltage L1
        dsmrData.power_returned_l1 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:22.7.0 > ");
        Log.verboseln("%F", dsmrData.power_returned_l1.val());
        break;
      case 104270: // 1-0:42.7.0(232.0*V) Voltage L2
        dsmrData.power_returned_l2 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:42.7.0 > ");
        Log.verboseln("%F", dsmrData.power_returned_l2.val());
        break;
      case 106270: // 1-0:62.7.0(232.0*V) Voltage L3
        dsmrData.power_returned_l3 = FixedValue(readUntilStar(i, len));
        Log.verbose("1-0:62.7.0 > ");
        Log.verboseln("%F", dsmrData.power_returned_l3.val());
        break;
      case 1032320: // Aantal korte spanningsdalingen Elektriciteit in fase 1
        dsmrData.electricity_sags_l1 = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("1-0:32.32.0 > ");
        Log.verboseln("%d", dsmrData.electricity_sags_l1);
        break;
      case 1052320: // Aantal korte spanningsdalingen Elektriciteit in fase 2
        dsmrData.electricity_sags_l2 = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("1-0:52.32.0 > ");
        Log.verboseln("%d", dsmrData.electricity_sags_l2);
        break;
      case 1072320: // Aantal korte spanningsdalingen Elektriciteit in fase 3
        dsmrData.electricity_sags_l3 = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("1-0:72.32.0 > ");
        Log.verboseln("%d", dsmrData.electricity_sags_l3);
        break;
      case 1032360: // 1-0:32.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 1
        dsmrData.electricity_swells_l1 = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("1-0:32.36.0 > ");
        Log.verboseln("%d", dsmrData.electricity_swells_l1);
        break;
      case 1052360: // 1-0:52.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 2
        dsmrData.electricity_swells_l2 = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("1-0:52.36.0 > ");
        Log.verboseln("%d", dsmrData.electricity_swells_l2);
        break;
      case 1072360: // 1-0:72.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 3
        dsmrData.electricity_swells_l3 = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("1-0:72.36.0 > ");
        Log.verboseln("%d", dsmrData.electricity_swells_l3);
        break;
      case 12421: // gas
        dsmrData.gas_delivered = readBetweenDoubleParenthesis(i, len);
        Log.verbose("gas: 0-1:24.2.1 > ");
        Log.verbose(dsmrData.gas_delivered.c_str());
        break;
      case 96721: // 0-0:96.7.21(00051)  Number of power failures in any phase
        dsmrData.electricity_failures = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("0-0:96.7.21 > ");
        Log.verboseln("%d", dsmrData.electricity_failures);
        break;
      case 9679: // 0-0:96.7.9(00007) Number of long power failures in any phase
        dsmrData.electricity_long_failures = readFirstParenthesisVal(i, len).toInt();
        Log.verbose("0-0:96.7.21 > ");
        Log.verboseln("%d", dsmrData.electricity_long_failures);
        break;
      case 1099970: //1-0:99.97.0(6) Power Failure Event Log (long power failures)
        dsmrData.electricity_failure_log = String(telegram[i], len);
        break;
      default:
        Log.verbose(" onbekende OBIS: ");
        statusMsg = "Onbekende OBIS: ";
        while (i < len) {
          Log.verbose("%c" ,(char)telegram[i]);
          statusMsg += (char)telegram[i];
          i++;
        }
        Log.verboseln("");
        break;
    }
      // clear the string for new input:
      inString = "";
}
