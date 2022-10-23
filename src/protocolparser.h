/*
 * 
 void parseLine(unsigned char *telegram, int len){
  long val  = 0;
  long val2 = 0;
  int pos4;
  
  if ((telegram[4] >= '0') && (telegram[4] <= '9'))
    pos4 = (int)(telegram[4])-48;
  else 
    pos4 = 10;
    
  if (devicestate == CONFIG) {
    // 0-0:96.1.1 equipmentId                         (xxxxxxxxxxxx) 
    if (strncmp(telegram, "0-1:96.1.0", strlen("0-1:96.1.0")) == 0) 
    getStr(equipmentId,telegram, len, '(', ')');

    // 1-3:0.2.8(42) or 1-3:0.2.8(50) // protocol version
    if (strncmp(telegram, "1-3:0.2.8", strlen("1-3:0.2.8")) == 0) {
        getStr(P1version,telegram, len, '(', ')');
        if (P1version[0] =='4') P1prot = 4; else P1prot = 5;
    }    

    if (equipmentId[0] == '\0') devicestate = RUNNING;
  }
debugln(pos4);
debugln(telegram);

  switch (pos4){
    case 1:
          // 0-0:1.0.0.255 datestamp YYMMDDhhmmssX
          if (strncmp(telegram, "0-0:1.0.0", strlen("0-0:1.0.0")) == 0) 
              getStr12(P1timestamp, telegram, len, '(');

         // 1-0:1.7.0(00.424*kW) Actueel verbruik 
         // 1-0:1.7.x = Electricity consumption current usage
          if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0) 
              getValue(actualElectricityPowerDelivered, telegram, len, '(', '*');

          //1-0:1.8.0 Mätarställning Aktiv Energi Uttag.  (Svenska)
          if (strncmp(telegram, "1-0:1.8.0", strlen("1-0:1.8.0")) == 0) 
              getValue(electricityUsedTariff1, telegram, len, '(', '*');
              
          // 1-0:1.8.1(000992.992*kWh) Elektra verbruik laag tarief
          if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0) 
              getValue(electricityUsedTariff1, telegram, len, '(', '*');

          // 1-0:1.8.2(000560.157*kWh) = Elektra verbruik hoog tarief
          if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0) 
              getValue(electricityUsedTariff2, telegram, len, '(', '*');    
          break;
          
    case 2:
          // 1-0:2.7.0(0000.000*kW) (Svenska)
          if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
              getValue(actualElectricityPowerReturned, telegram, len, '(', '*');

          //1-0:2.8.0  Mätarställning Aktiv Energi Inmatning (Svenska)
          if (strncmp(telegram, "1-0:2.8.0", strlen("1-0:2.8.0")) == 0) 
              getValue(electricityUsedTariff2, telegram, len, '(', '*');

          // 1-0:2.8.1(000348.890*kWh) Elektra opbrengst laag tarief
          if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0) 
              getValue(electricityReturnedTariff1, telegram, len, '(', '*');

          // 1-0:2.8.2(000859.885*kWh) Elektra opbrengst hoog tarief
          if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0) 
              getValue(electricityReturnedTariff2, telegram, len, '(', '*');

          // 1-0:21.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
          if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
              getValue(activePowerL1P, telegram, len, '(', '*');

          // 1-0:22.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
          if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
              getValue(activePowerL1NP, telegram, len, '(', '*');

          // 0-1:24.2.1(150531200000S)(00811.923*m3)
          // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter, other meters do (number)(gas value)
          if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0) 
             getGasValue(gasReceived5min, telegram, len, '(', ')');

         // 0-1:24.2.1(150531200000S)(00811.923*m3)
        // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter, other meters do (number)(gas value)
         if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0) 
            getDomoticzGasValue(gasDomoticz, telegram, len, '(', ')');

                      
        // (0-1:24.2.1)(m3) is gas designator for dsmr 2.2. The actual gasvalue is on next line, so first we place a flag, then we can test the next line on the next iteration of line parsing.
        if (strncmp(telegram, "0-1:24.3.0", strlen("0-1:24.3.0")) == 0) gas22Flag = true;
        if (gas22Flag && strncmp(telegram, "(", strlen(")")) == 0)
            getGas22Value(gasReceived5min, telegram, len, '(', ')');

          // 1-0:23.7.0(0000.000*kvar) Instantaneous reactive power L1 (+P)
//          if (strncmp(telegram, "1-0:23.7.0", strlen("1-0:23.7.0")) == 0)
//              getValue(reactivePowerL1NP, telegram, len, '(', '*');
              
          // 1-0:24.7.0(0000.000*kvar) Instantaneous reactive power L1 (-P)
//          if (strncmp(telegram, "1-0:24.7.0", strlen("1-0:24.7.0")) == 0)
//              getValue(reactivePowerL1NP, telegram, len, '(', '*');
          
          break;
          
    case 3:
          // 1-0:3.7.0(0000.000*kvar) reactive power delivered to client
          if (strncmp(telegram, "1-0:3.7.0", strlen("1-0:3.7.0")) == 0)
             getValue(instantaneousCurrentL1, telegram, len, '(', '*');

             
          // 1-0:31.7.0(002*A) Instantane stroom Elektriciteit L1
          if (strncmp(telegram, "1-0:31.7.0", strlen("1-0:31.7.0")) == 0)
             getValue(instantaneousCurrentL1, telegram, len, '(', '*');
             
          // 1-0:32.7.0(232.0*V) Voltage L1
         if (strncmp(telegram, "1-0:32.7.0", strlen("1-0:32.7.0")) == 0)
             getStr(instantaneousVoltageL1, telegram, len, '(', '*');   

        // 1-0:32.32.0(00000) Aantal korte spanningsdalingen Elektriciteit in fase 1
        if (strncmp(telegram, "1-0:32.32.0", strlen("1-0:32.32.0")) == 0)
             getValue(numberVoltageSagsL1, telegram, len, '(', ')');

        // 1-0:32.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 1
        if (strncmp(telegram, "1-0:32.36.0", strlen("1-0:32.36.0")) == 0)
              getValue(numberVoltageSwellsL1, telegram, len, '(', ')');
        
          break;
          
    case 4:
         // 1-0:4.7.0(0000.000*kvar) Reaktiv Effekt Inmatning Momentan trefaseffekt (Svensk)
 //         if (strncmp(telegram, "1-0:4.7.0", strlen("1-0:4.7.0")) == 0)
 //            getValue(electricityReturnedTariff1, telegram, len, '(', '*');
 
         // 1-0:4.8.0(00000127.239*kvarh) Mätarställning Reaktiv Energi Inmatning  
 //         if (strncmp(telegram, "1-0:4.8.0", strlen("1-0:4.8.0")) == 0)
 //            getValue(electricityReturnedTariff2, telegram, len, '(', '*');
    
        // 1-0:41.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L2
        if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
           getValue(activePowerL2P, telegram, len, '(', '*');
           
        // 1-0:42.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L2
        if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
           getValue(activePowerL2NP, telegram, len, '(', '*');

        // 1-0:43.7.0(0000.000*kvar) L2 Reaktiv Effekt Uttag Momentan effekt
        if (strncmp(telegram, "1-0:43.7.0", strlen("1-0:43.7.0")) == 0)
           getValue(activePowerL2P, telegram, len, '(', '*');
           
        // 1-0:44.7.0(0000.053*kvar) L2 Reaktiv Effekt Inmatning Momentan effekt
        if (strncmp(telegram, "1-0:44.7.0", strlen("1-0:44.7.0")) == 0)
           getValue(activePowerL2NP, telegram, len, '(', '*');
        
        break;
          
    case 5:
        // 1-0:52.32.0(00000) voltage sags L1
        if (strncmp(telegram, "1-0:52.32.0", strlen("1-0:52.32.0")) == 0)
           getValue(numberVoltageSagsL1, telegram, len, '(', ')');
           
        // 1-0:52.36.0(00000) voltage swells L1
        if (strncmp(telegram, "1-0:52.36.0", strlen("1-0:52.36.0")) == 0)
           getValue(numberVoltageSwellsL1, telegram, len, '(', ')');
               
        // 1-0:51.7.0(002*A) Instantane stroom Elektriciteit L2
        if (strncmp(telegram, "1-0:51.7.0", strlen("1-0:51.7.0")) == 0)
           getValue(instantaneousCurrentL2, telegram, len, '(', '*');

        // 1-0:52.7.0(232.0*V) Voltage L2
        if (strncmp(telegram, "1-0:52.7.0", strlen("1-0:52.7.0")) == 0)
           getStr(instantaneousVoltageL2, telegram, len, '(', '*');
                 
          break;
          
    case 6:
        // 1-0:61.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L3
        if (strncmp(telegram, "1-0:61.7.0", strlen("1-0:61.7.0")) == 0)
            getValue(activePowerL3P, telegram, len, '(', '*');

        // 1-0:62.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L3
        if (strncmp(telegram, "1-0:62.7.0", strlen("1-0:62.7.0")) == 0)
            getValue(activePowerL3NP, telegram, len, '(', '*');

        // 1-0:63.7.0(00.378*kvar) L3 Reaktiv Effekt Uttag Momentan effekt
        if (strncmp(telegram, "1-0:63.7.0", strlen("1-0:63.7.0")) == 0)
            getValue(reactivePowerL3P, telegram, len, '(', '*');

        // 1-0:64.7.0(00.378*kvar) L3 Reaktiv Effekt Inmatning Momentan effekt
        if (strncmp(telegram, "1-0:64.7.0", strlen("1-0:64.7.0")) == 0)
            getValue(reactivePowerL3NP, telegram, len, '(', '*');
          break;
          
    case 7:
        // 1-0:71.7.0(002*A) Instantane stroom Elektriciteit L3
        if (strncmp(telegram, "1-0:71.7.0", strlen("1-0:71.7.0")) == 0)
           getValue(instantaneousCurrentL3, telegram, len, '(', '*');

        // 1-0:72.7.0(232.0*V) Voltage L3
        if (strncmp(telegram, "1-0:72.7.0", strlen("1-0:72.7.0")) == 0)
           getStr(instantaneousVoltageL3, telegram, len, '(', '*');
           
       // 1-0:72.32.0(00000) voltage sags L3
        if (strncmp(telegram, "1-0:72.32.0", strlen("1-0:72.32.0")) == 0)
           getValue(numberVoltageSagsL3, telegram, len, '(', ')');
           
        // 1-0:72.36.0(00000) voltage swells L3
        if (strncmp(telegram, "1-0:72.36.0", strlen("1-0:72.36.0")) == 0)
           getValue(numberVoltageSwellsL3, telegram, len, '(', ')');
  
          break;
          
    case 9:
         // 0-0:96.14.0(0001) Actual Tarif
        if (strncmp(telegram, "0-0:96.14.0", strlen("0-0:96.14.0")) == 0)
           getValue(tariffIndicatorElectricity, telegram, len, '(', ')');

        // 0-0:96.7.21(00003) Aantal onderbrekingen Elektriciteit
        if (strncmp(telegram, "0-0:96.7.21", strlen("0-0:96.7.21")) == 0)
           getValue(numberPowerFailuresAny, telegram, len, '(', ')');


        // 0-0:96.7.9(00001) Aantal lange onderbrekingen Elektriciteit
        if (strncmp(telegram, "0-0:96.7.9", strlen("0-0:96.7.9")) == 0)
           getValue(numberLongPowerFailuresAny, telegram, len, '(', ')');

        if (strncmp(telegram, "0-1:96.1.1", strlen("0-1:96.1.1")) == 0) 
            getStr(equipmentId,telegram, len, '(', ')');

        break;
          
    default:
          break;  
    }
 }
 */
