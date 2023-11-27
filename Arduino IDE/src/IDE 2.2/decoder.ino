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
 
 * @file decoder.ino
 * @author Ronald Leenes

 *
 * @brief This file contains the OBIS parser functions
 *
 * @see http://esp8266thingies.nl
 */

// long getValidVal(long valNew, long valOld, long maxDiffer)
// {
//   //check if the incoming value is valid
//       if(valOld > 0 && ((valNew - valOld > maxDiffer) && (valOld - valNew > maxDiffer)))
//         return valOld;
//       return valNew;
// }

// void getValue(char *theValue, char *buffer, int maxlen, char startchar, char endchar){
//   String cleanres="";
//   int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
//   int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;

//     char res[16];
//     memset(res, 0, sizeof(res));
    


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

    validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);
    
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
      newDecoder(len);
    }
    return;
  } //state = reading

  debugln("how did we get here??");
  return;
}
