bool decodeTelegram(int len) {
  //need to check for start
  int startChar = FindCharInArrayRev(telegram, '/', len);
  int endChar = FindCharInArrayRev(telegram, '!', len);
  bool validCRCFound = false;

 if (state == WAITING) {      // we're waiting for a valid start sequence, if this line is not it, just return
  if(startChar>=0)
  {
    //start found. Reset CRC calculation
    currentCRC=CRC16(0x0000,(unsigned char *) telegram+startChar, len-startChar);
    // and reset datagram 
    datagram ="";
    datagramValid = false;
    dataEnd = false;

      for(int cnt=startChar; cnt<len-startChar;cnt++){
        debug(telegram[cnt]);
        datagram += telegram[cnt];
      }    
    debugln("Start found!");
    state = READING;
    return false;
  } else {
    currentCRC = 0;
    return false;        // We're waiting for a valid start char, if we're in the middle of a datagram, just return.
  }
 }
 
  if (state == READING) {     
  if(endChar>=0)   // we have found the endchar !
  {
    state = CHECKSUM;
    //add to crc calc 
    dataEnd = true;         // we're at the end of the data stream, so mark (for raw data output) We don't know if the data is valid, we will test this below.
    gas22Flag=false;        // assume we have also collected the Gas value
    currentCRC=CRC16(currentCRC,(unsigned char*)telegram+endChar, 1);
    char messageCRC[4];
    strncpy(messageCRC, telegram + endChar + 1, 4);
      for(int cnt=0; cnt<len;cnt++) {
        datagram += telegram[cnt];
      }
      datagram += "\r";
      datagram += "\n";
          
    validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);
    debug("   calculated CRC:");
        debugln(currentCRC);
    if(validCRCFound) {
      debugln("\nVALID CRC FOUND!"); 
      datagramValid = true;
      state = DONE;
      return true;
    }
    else {
      debugln("\n===INVALID CRC FOUND!===");
      state = FAILURE;
      currentCRC = 0;
      return false;
    }
  } 
  
  else  { // normal line, process
    currentCRC=CRC16(currentCRC, (unsigned char*)telegram, len);
      for(int cnt=0; cnt<len;cnt++){
        debug(telegram[cnt]);
        datagram += telegram[cnt];
      }
  
  long val  = 0;
  long val2 = 0;

  // 1-0:1.8.1(000992.992*kWh)
  // 1-0:1.8.1 = Elektra verbruik laag tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0) 
    getValue(electricityUsedTariff1, telegram, len, '(', '*');
  
  // 1-0:1.8.2(000560.157*kWh)
  // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0) 
    getValue(electricityUsedTariff2, telegram, len, '(', '*');

  // 1-0:2.8.1(000348.890*kWh)
  // 1-0:2.8.1 = Elektra opbrengst laag tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0) 
    getValue(electricityReturnedTariff1, telegram, len, '(', '*');
    
  // 1-0:2.8.2(000859.885*kWh)
  // 1-0:2.8.2 = Elektra opbrengst hoog tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0) 
    getValue(electricityReturnedTariff2, telegram, len, '(', '*');
    
  // 1-0:1.7.0(00.424*kW) Actueel verbruik
  // 1-0:2.7.0(00.000*kW) Actuele teruglevering
  // 1-0:1.7.x = Electricity consumption actual usage (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0) 
    getValue(actualElectricityPowerDelivered, telegram, len, '(', '*');

  if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
    getValue(actualElectricityPowerReturned, telegram, len, '(', '*');

   // 1-0:21.7.0(00.378*kW)
    // 1-0:21.7.0 = Instantaan vermogen Elektriciteit levering L1
    if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
        getValue(activePowerL1P, telegram, len, '(', '*');

    // 1-0:41.7.0(00.378*kW)
    // 1-0:41.7.0 = Instantaan vermogen Elektriciteit levering L2
    if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
           getValue(activePowerL2P, telegram, len, '(', '*');

    // 1-0:61.7.0(00.378*kW)
    // 1-0:61.7.0 = Instantaan vermogen Elektriciteit levering L3
    if (strncmp(telegram, "1-0:61.7.0", strlen("1-0:61.7.0")) == 0)
         getValue(activePowerL3P, telegram, len, '(', '*');

    // 1-0:31.7.0(002*A)
    // 1-0:31.7.0 = Instantane stroom Elektriciteit L1
    if (strncmp(telegram, "1-0:31.7.0", strlen("1-0:31.7.0")) == 0)
             getValue(instantaneousCurrentL1, telegram, len, '(', '*');

    // 1-0:51.7.0(002*A)
    // 1-0:51.7.0 = Instantane stroom Elektriciteit L2
    if (strncmp(telegram, "1-0:51.7.0", strlen("1-0:51.7.0")) == 0)
                getValue(instantaneousCurrentL2, telegram, len, '(', '*');

    // 1-0:71.7.0(002*A)
    // 1-0:71.7.0 = Instantane stroom Elektriciteit L3
    if (strncmp(telegram, "1-0:71.7.0", strlen("1-0:71.7.0")) == 0)
                getValue(instantaneousCurrentL3, telegram, len, '(', '*');


    // 1-0:32.7.0(232.0*V)
    // 1-0:32.7.0 = Voltage L1
    if (strncmp(telegram, "1-0:32.7.0", strlen("1-0:32.7.0")) == 0)
                getValue(instantaneousVoltageL1, telegram, len, '(', '*');

    // 1-0:52.7.0(232.0*V)
    // 1-0:52.7.0 = Voltage L2
    if (strncmp(telegram, "1-0:52.7.0", strlen("1-0:52.7.0")) == 0)
                 getValue(instantaneousVoltageL2, telegram, len, '(', '*');

    // 1-0:72.7.0(232.0*V)
    // 1-0:72.7.0 = Voltage L3
    if (strncmp(telegram, "1-0:72.7.0", strlen("1-0:72.7.0")) == 0)
                getValue(instantaneousVoltageL3, telegram, len, '(', '*');


      // 0-0:96.14.0(0001)
    // 0-0:96.14.0 = Actual Tarif
    if (strncmp(telegram, "0-0:96.14.0", strlen("0-0:96.14.0")) == 0)
                getValue(tariffIndicatorElectricity, telegram, len, '(', ')');


    // 0-0:96.7.21(00003)
    // 0-0:96.7.21 = Aantal onderbrekingen Elektriciteit
    if (strncmp(telegram, "0-0:96.7.21", strlen("0-0:96.7.21")) == 0)
                  getValue(numberPowerFailuresAny, telegram, len, '(', ')');


    // 0-0:96.7.9(00001)
    // 0-0:96.7.9 = Aantal lange onderbrekingen Elektriciteit
    if (strncmp(telegram, "0-0:96.7.9", strlen("0-0:96.7.9")) == 0)
                  getValue(numberLongPowerFailuresAny, telegram, len, '(', ')');


    // 1-0:32.32.0(00000)
    // 1-0:32.32.0 = Aantal korte spanningsdalingen Elektriciteit in fase 1
    if (strncmp(telegram, "1-0:32.32.0", strlen("1-0:32.32.0")) == 0)
                    getValue(numberVoltageSagsL1, telegram, len, '(', ')');


    // 1-0:32.36.0(00000)
    // 1-0:32.36.0 = Aantal korte spanningsstijgingen Elektriciteit in fase 1
    if (strncmp(telegram, "1-0:32.36.0", strlen("1-0:32.36.0")) == 0)
                      getValue(numberVoltageSwellsL1, telegram, len, '(', ')');

    
    
  // 0-1:24.2.1(150531200000S)(00811.923*m3)
  // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter, other meters do (number)(gas value)
  if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0) 
                      getGasValue(gasReceived5min, telegram, len, '(', ')');

  // (0-1:24.2.1)(m3) is gas designator for dsmr 2.2. The actual gasvalue is on next line, so first we place a flag, then we can test the next line on the next iteration of line parsing.
  if (strncmp(telegram, "0-1:24.3.0", strlen("0-1:24.3.0")) == 0) gas22Flag = true;
  if (gas22Flag && strncmp(telegram, "(", strlen(")")) == 0)
          getGas22Value(gasReceived5min, telegram, len, '(', ')');
          
  // DSMR 2.2 does not seem to have a proper terminator. But on the test data I have, the final row is this one:
  // if (strncmp(telegram, "0-1:24.4.0", strlen("0-1:24.4.0")) == 0)  dataEnd = true;

  }
  return validCRCFound;       // true if valid CRC found
  } //state = reading
}
