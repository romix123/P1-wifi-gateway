/*
 * Copyright (c) 2023 Jean-Pierre sneyers
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
 * Additionally, please note that the original source code of this file
 * may contain portions of code derived from (or inspired by)
 * previous works by:
 *
 * Ronald Leenes (https://github.com/romix123/P1-wifi-gateway and http://esp8266thingies.nl)
 */

#include "P1Reader.h"

P1Reader::P1Reader(settings& currentConf) : conf(currentConf)
{
    devicestate = CONFIG;
    Serial.begin(115200);
    datagram.reserve(1500);
}

void P1Reader::settime()
{
    //(hr,min,sec,day,mnth,yr);
    // YYMMDDhhmmssX
    setTime(NUMP1TIMER(6, 10) + NUMP1TIMER(7, 1), NUMP1TIMER(8, 10) + NUMP1TIMER(9, 1), NUMP1TIMER(10, 10) + NUMP1TIMER(11, 1), NUMP1TIMER(4, 10) + NUMP1TIMER(5, 1), NUMP1TIMER(2, 10) + NUMP1TIMER(3, 1), NUMP1TIMER(0, 10) + NUMP1TIMER(1, 1));
}

void P1Reader::RTS_on() // switch on Data Request
{
    digitalWrite(OE, LOW);  // enable buffer
    digitalWrite(DR, HIGH); // turn on Data Request
    OEstate = true;

    SendDebug("[P1] Data requested");
}

void P1Reader::RTS_off() // switch off Data Request
{
    digitalWrite(DR, LOW);  // turn off Data Request
    digitalWrite(OE, HIGH); // put buffer in Tristate mode
    OEstate = false;
    nextUpdateTime = millis() + conf.interval * 1000;
    SendDebugPrintf("[P1] Data end request. Next action in %dms", nextUpdateTime);
}

void P1Reader::ResetnextUpdateTime()
{
    nextUpdateTime = millis() + 10000;
    state = WAITING;
    OEstate = false;
}

int P1Reader::FindCharInArrayRev(const char array[], char c, int len)
{
    for (int i = len - 1; i >= 0; i--)
    {
        if (array[i] == c)
        {
            return i;
        }
    }
    return -1;
}

int P1Reader::FindCharInArrayRev2(const char array[], char c, int len)
{
    for (int i = len - 1; i >= 0; i--)
    {
        if (array[i] == c && array[i + 1] == '(')
        {
            return i;
        }
    }
    return -1;
}

void P1Reader::getStr(char* theValue, char* buffer, int maxlen, char startchar, char endchar)
{
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;
    char res[102];
    memset(res, 0, sizeof(res));

    if (strncpy(res, buffer + s + 1, l))
    {
        if (isNumber(res, l))
        {
            strncpy(theValue, res, l);
        }
        theValue[l + 1] = 0;
    }
}

bool P1Reader::isNumber(const char* res, const int len)
{
    for (int i = 0; i < len; i++)
    {
        if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0))
        {
            return false;
        }
    }
    return true;
}

unsigned int P1Reader::CRC16(unsigned int crc, unsigned char* buf, int len)
{
    for (int pos = 0; pos < len; pos++)
    {
        crc ^= (unsigned int)buf[pos]; // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--) // Loop over each bit
        {
            if ((crc & 0x0001) != 0) // If the LSB is set
            {
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else           // Else LSB is not set
                crc >>= 1; // Just shift right
        }
    }
    return crc;
}

String P1Reader::identifyMeter(String Name)
{
    if (Name.indexOf("ISK5\\2M550E-1011") != -1)
    {
        return "ISKRA AM550e-1011";
    }
    if (Name.indexOf("KFM5KAIFA-METER") != -1)
    {
        return "Kaifa  (MA105 of MA304)";
    }
    if (Name.indexOf("XMX5LGBBFG10") != -1)
    {
        return "Landis + Gyr E350";
    }
    if (Name.indexOf("XMX5LG") != -1)
    {
        return "Landis + Gyr";
    }
    if (Name.indexOf("Ene5\\T210-D") != -1)
    {
        return "Sagemcom T210-D";
    }

    return "UNKNOW";
}

bool P1Reader::decodeTelegram(int len)
{
    int pos4;
    // need to check for start
    int startChar = FindCharInArrayRev(telegram, '/', len);
    int endChar = FindCharInArrayRev(telegram, '!', len);
    bool validCRCFound = false;

    if (state == WAITING)
    {
        // we're waiting for a valid start sequence, if this line is not it, just return
        if (startChar >= 0)
        {
            // start found. Reset CRC calculation
            currentCRC = CRC16(0x0000, (unsigned char*)telegram + startChar, len - startChar);
            // and reset datagram
            datagram = "";
            datagramValid = false;
            dataEnd = false;

            for (int cnt = startChar; cnt < len - startChar; cnt++)
            {
                // debug(telegram[cnt]);
                datagram += telegram[cnt];
                if (devicestate == CONFIG)
                {
                    meterId += telegram[cnt]; // we only need to collect the meterId once
                }
            }
            if (devicestate == CONFIG)
            {
                meterName = identifyMeter(meterId);
            }
            state = READING;
            return false;
        }
        else
        {
            currentCRC = 0;
            return false; // We're waiting for a valid start char, if we're in the middle of a datagram, just return.
        }
    }

    if (state == READING)
    {
        if (endChar >= 0) // we have found the endchar !
        {
            nextUpdateTime = millis() + conf.interval * 1000; // set trigger for next round

            state = CHECKSUM;
            // add to crc calc
            dataEnd = true;    // we're at the end of the data stream, so mark (for raw data output) We don't know if the data is valid, we will test this below.
            gas22Flag = false; // assume we have also collected the Gas value
            currentCRC = CRC16(currentCRC, (unsigned char*)telegram + endChar, 1);
            char messageCRC[4];
            strncpy(messageCRC, telegram + endChar + 1, 4);
            if (datagram.length() < 2048)
            {
                for (int cnt = 0; cnt < len; cnt++)
                {
                    datagram += telegram[cnt];
                }
                datagram += "\r";
                datagram += "\n";
            }
            else
            {
                datagram = ""; // prevent bufferoverflow
            }

            validCRCFound = (strtoul(messageCRC, NULL, 16) == currentCRC);
            // debug("   calculated CRC:");
            // debugln(currentCRC);

            if (validCRCFound)
            {
                LastSample = millis();
                datagramValid = true;
                gotPowerReading = true; // we at least got electricty readings. Not all setups have a gas meter attached, so gotGasReading is handled when we actually get gasIds coming in
                state = DONE;
                RTS_off();
                if (devicestate == GOTMETER)
                {
                    devicestate = RUNNING;
                }

                return true;
            }
            else
            {
                SendDebug("[P1] ERROR, invalid CRC");
                state = FAILURE;
                currentCRC = 0;
                RTS_off();
                return false;
            }
        }

        else
        { // normal line, process
            currentCRC = CRC16(currentCRC, (unsigned char*)telegram, len);
            for (int cnt = 0; cnt < len; cnt++)
            {
                datagram += telegram[cnt];
            }

            if ((telegram[4] >= '0') && (telegram[4] <= '9'))
                pos4 = (int)(telegram[4]) - 48;
            else
                pos4 = 10;

            if (devicestate == CONFIG)
            {
                // 0-0:96.1.1 equipmentId                         (xxxxxxxxxxxx)
                if (strncmp(telegram, "96.1.0", strlen("96.1.0")) == 0)
                {
                    getStr(DataReaded.equipmentId, telegram, len, '(', ')');
                    devicestate = GOTMETER;
                }
                // 1-3:0.2.8(42) or 1-3:0.2.8(50) // protocol version
                if (strncmp(telegram, "1-3:0.2.8", strlen("1-3:0.2.8")) == 0)
                {
                    getStr(DataReaded.P1version, telegram, len, '(', ')');
                    if (DataReaded.P1version[0] == '4')
                        DataReaded.P1prot = 4;
                    else
                        DataReaded.P1prot = 5;
                }
            }

            switch (pos4)
            {
            case 1:
                // 0-0:1.0.0.255 datestamp YYMMDDhhmmssX
                if (strncmp(telegram, "0-0:1.0.0", strlen("0-0:1.0.0")) == 0)
                {
                    getStr12(DataReaded.P1timestamp, telegram, len, '(');
                    if (timeStatus() == timeNotSet)
                    {
                        settime();
                    }

                    if ((hour() == 0) && (minute() == 30))
                    {
                        settime(); // resync every 24 hours
                    }
                    break;
                }

                // 1-0:1.7.0(00.424*kW) Actueel verbruik
                // 1-0:1.7.x = Electricity consumption current usage
                if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0)
                {
                    getValue(DataReaded.actualElectricityPowerDeli, telegram, len, '(', '*');
                    //sprintf(payload, "1-0:1.7.0", DataReaded.actualElectricityPowerRet);
                    break;
                }
#ifdef SWEDISH
                // 1-0:1.7.0 Aktiv Effekt Uttag  Momentan trefaseffekt
                if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0)
                    getValue(DataReaded.momentaryActiveImport, telegram, len, '(', '*');

                // 1-0:1.8.0 Mätarställning Aktiv Energi Uttag.  (Svenska)
                if (strncmp(telegram, "1-0:1.8.0", strlen("1-0:1.8.0")) == 0)
                    getValue(DataReaded.cumulativeActiveImport, telegram, len, '(', '*');
#endif

                // 1-0:1.8.1(000992.992*kWh) Elektra verbruik laag tarief
                if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0)
                {
                    getValue(DataReaded.electricityUsedTariff1, telegram, len, '(', '*');
                    break;
                }

                // 1-0:1.8.2(000560.157*kWh) = Elektra verbruik hoog tarief
                if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0)
                {
                    getValue(DataReaded.electricityUsedTariff2, telegram, len, '(', '*');
                    break;
                }

            case 2:
                if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
                {
                    getValue(DataReaded.actualElectricityPowerRet, telegram, len, '(', '*');
                    break;
                }
#ifdef SWEDISH
                // 1-0:2.7.0(0000.000*kW) (Svenska)
                if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
                    getValue(DataReaded.momentaryActiveExport, telegram, len, '(', '*');

                // 1-0:2.8.0  Mätarställning Aktiv Energi Inmatning (Svenska)
                if (strncmp(telegram, "1-0:2.8.0", strlen("1-0:2.8.0")) == 0)
                    getValue(DataReaded.cumulativeActiveExport, telegram, len, '(', '*');

                // 1-0:21.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
                    getValue(DataReaded.activePowerL1P, telegram, len, '(', '*');

                // 1-0:22.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
                    getValue(DataReaded.reactivePowerL1NP, telegram, len, '(', '*');

                // 1-0:23.7.0(0000.000*kvar) Instantaneous reactive power L1 (+P)
                if (strncmp(telegram, "1-0:23.7.0", strlen("1-0:23.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveImportL1, telegram, len, '(', '*');

                // 1-0:24.7.0(0000.000*kvar) Instantaneous reactive power L1 (-P)
                if (strncmp(telegram, "1-0:24.7.0", strlen("1-0:24.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveExportL1, telegram, len, '(', '*');
#endif

                // 1-0:21.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL1P, telegram, len, '(', '*');
                    break;
                }

                // 1-0:22.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL1NP, telegram, len, '(', '*');
                    break;
                }

                //        if (meterId.indexOf("ISK5\2M550E-1011") ==0){
                //          debugln("ISKRA exception");
                //            if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
                //                getValue(actualElectricityPowerDeli, telegram, len, '(', '*'); // kludge for ISKRA meter
                //                // 1-0:22.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                //            if (strncmp(telegram, "1-0:22.7.0", strlen("1-0:22.7.0")) == 0)
                //                getValue(actualElectricityPowerRet, telegram, len, '(', '*'); // kludge for ISKRA meter
                //        }

                // 1-0:2.8.1(000348.890*kWh) Elektra opbrengst laag tarief
                if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0)
                {
                    getValue(DataReaded.electricityReturnedTariff1, telegram, len, '(', '*');
                    break;
                }

                // 1-0:2.8.2(000859.885*kWh) Elektra opbrengst hoog tarief
                if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0)
                {
                    getValue(DataReaded.electricityReturnedTariff2, telegram, len, '(', '*');
                    break;
                }

                // 1-0:21.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL1P, telegram, len, '(', '*');
                    break;
                }

                // 1-0:22.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L1
                if (strncmp(telegram, "1-0:21.7.0", strlen("1-0:21.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL1NP, telegram, len, '(', '*');
                    break;
                }

                ////          // 0-1:24.2.1(150531200000S)(00811.923*m3)
                ////          // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter, other meters do (number)(gas value)
                if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0)
                {
                    getGasValue(DataReaded.gasReceived5min, telegram, len, '(', ')');
                    break;
                }
                ////
                ////         // 0-1:24.2.1(150531200000S)(00811.923*m3)
                ////        // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter, other meters do (number)(gas value)
                if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0)
                {
                    getDomoticzGasValue(DataReaded.gasDomoticz, telegram, len, '(', ')');
                    gotGasReading = true;
                    break;
                }

                break;

            case 3:

#ifdef SWEDISH
                // 1-0:3.7.0(0000.000*kvar) reactive power delivered to client
                if (strncmp(telegram, "1-0:3.7.0", strlen("1-0:3.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveImport, telegram, len, '(', '*');

                // 1-0:3.8.0(0000.000*kvar) reactive power delivered to client
                if (strncmp(telegram, "1-0:3.8.0", strlen("1-0:3.8.0")) == 0)
                    getValue(DataReaded.cumulativeReactiveImport, telegram, len, '(', '*');

#endif

                // 1-0:31.7.0(002*A) Instantane stroom Elektriciteit L1
                if (strncmp(telegram, "1-0:31.7.0", strlen("1-0:31.7.0")) == 0)
                {
                    getValue(DataReaded.instantaneousCurrentL1, telegram, len, '(', '*');
                    break;
                }

                // 1-0:32.7.0(232.0*V) Voltage L1
                if (strncmp(telegram, "1-0:32.7.0", strlen("1-0:32.7.0")) == 0)
                {
                    getStr(DataReaded.instantaneousVoltageL1, telegram, len, '(', '*');
                    break;
                }

                // 1-0:32.32.0(00000) Aantal korte spanningsdalingen Elektriciteit in fase 1
                if (strncmp(telegram, "1-0:32.32.0", strlen("1-0:32.32.0")) == 0)
                {
                    getValue(DataReaded.numberVoltageSagsL1, telegram, len, '(', ')');
                    break;
                }

                // 1-0:32.36.0(00000) Aantal korte spanningsstijgingen Elektriciteit in fase 1
                if (strncmp(telegram, "1-0:32.36.0", strlen("1-0:32.36.0")) == 0)
                {
                    getValue(DataReaded.numberVoltageSwellsL1, telegram, len, '(', ')');
                    break;
                }

                break;

            case 4:

#ifdef SWEDISH
                // 1-0:4.7.0(0000.000*kvar) Reaktiv Effekt Inmatning Momentan trefaseffekt (Svensk)
                if (strncmp(telegram, "1-0:4.7.0", strlen("1-0:4.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveExport, telegram, len, '(', '*');

                // 1-0:4.8.0(00000127.239*kvarh) Mätarställning Reaktiv Energi Inmatning
                if (strncmp(telegram, "1-0:4.8.0", strlen("1-0:4.8.0")) == 0)
                    getValue(DataReaded.cumulativeReactiveExport, telegram, len, '(', '*');

                // 1-0:43.7.0(0000.000*kvar) L2 Reaktiv Effekt Uttag Momentan effekt
                if (strncmp(telegram, "1-0:43.7.0", strlen("1-0:43.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveImportL2, telegram, len, '(', '*');

                // 1-0:44.7.0(0000.053*kvar) L2 Reaktiv Effekt Inmatning Momentan effekt
                if (strncmp(telegram, "1-0:44.7.0", strlen("1-0:44.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveExportL2, telegram, len, '(', '*');

#endif

                // 1-0:41.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L2
                if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL2P, telegram, len, '(', '*');
                    break;
                }

                // 1-0:42.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L2
                if (strncmp(telegram, "1-0:41.7.0", strlen("1-0:41.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL2NP, telegram, len, '(', '*');
                    break;
                }

                break;

            case 5:
                // 1-0:52.32.0(00000) voltage sags L1
                if (strncmp(telegram, "1-0:52.32.0", strlen("1-0:52.32.0")) == 0)
                {
                    getValue(DataReaded.numberVoltageSagsL1, telegram, len, '(', ')');
                    break;
                }

                // 1-0:52.36.0(00000) voltage swells L1
                if (strncmp(telegram, "1-0:52.36.0", strlen("1-0:52.36.0")) == 0)
                {
                    getValue(DataReaded.numberVoltageSwellsL1, telegram, len, '(', ')');
                    break;
                }

                // 1-0:51.7.0(002*A) Instantane stroom Elektriciteit L2
                if (strncmp(telegram, "1-0:51.7.0", strlen("1-0:51.7.0")) == 0)
                {
                    getValue(DataReaded.instantaneousCurrentL2, telegram, len, '(', '*');
                    break;
                }

                // 1-0:52.7.0(232.0*V) Voltage L2
                if (strncmp(telegram, "1-0:52.7.0", strlen("1-0:52.7.0")) == 0)
                {
                    getStr(DataReaded.instantaneousVoltageL2, telegram, len, '(', '*');
                    break;
                }

                break;

            case 6:
#ifdef SWEDISH

                // 1-0:63.7.0(00.378*kvar) L3 Reaktiv Effekt Uttag Momentan effekt
                if (strncmp(telegram, "1-0:63.7.0", strlen("1-0:63.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveImportL3, telegram, len, '(', '*');

                // 1-0:64.7.0(00.378*kvar) L3 Reaktiv Effekt Inmatning Momentan effekt
                if (strncmp(telegram, "1-0:64.7.0", strlen("1-0:64.7.0")) == 0)
                    getValue(DataReaded.momentaryReactiveExportL3, telegram, len, '(', '*');
#endif

                // 1-0:61.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L3
                if (strncmp(telegram, "1-0:61.7.0", strlen("1-0:61.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL3P, telegram, len, '(', '*');
                    break;
                }

                // 1-0:62.7.0(00.378*kW) Instantaan vermogen Elektriciteit levering L3
                if (strncmp(telegram, "1-0:62.7.0", strlen("1-0:62.7.0")) == 0)
                {
                    getValue(DataReaded.activePowerL3NP, telegram, len, '(', '*');
                    break;
                }

                break;

            case 7:
                // 1-0:71.7.0(002*A) Instantane stroom Elektriciteit L3
                if (strncmp(telegram, "1-0:71.7.0", strlen("1-0:71.7.0")) == 0)
                {
                    getValue(DataReaded.instantaneousCurrentL3, telegram, len, '(', '*');
                    break;
                }

                // 1-0:72.7.0(232.0*V) Voltage L3
                if (strncmp(telegram, "1-0:72.7.0", strlen("1-0:72.7.0")) == 0)
                {
                    getStr(DataReaded.instantaneousVoltageL3, telegram, len, '(', '*');
                    break;
                }

                // 1-0:72.32.0(00000) voltage sags L3
                if (strncmp(telegram, "1-0:72.32.0", strlen("1-0:72.32.0")) == 0)
                {
                    getValue(DataReaded.numberVoltageSagsL3, telegram, len, '(', ')');
                    break;
                }

                // 1-0:72.36.0(00000) voltage swells L3
                if (strncmp(telegram, "1-0:72.36.0", strlen("1-0:72.36.0")) == 0)
                {
                    getValue(DataReaded.numberVoltageSwellsL3, telegram, len, '(', ')');
                    break;
                }

                break;

            case 9:
                // 0-0:96.14.0(0001) Actual Tarif
                if (strncmp(telegram, "0-0:96.14.0", strlen("0-0:96.14.0")) == 0)
                {
                    getStr(DataReaded.tariffIndicatorElectricity, telegram, len, '(', ')');
                    break;
                }

                // 0-0:96.7.21(00003) Aantal onderbrekingen Elektriciteit
                if (strncmp(telegram, "0-0:96.7.21", strlen("0-0:96.7.21")) == 0)
                {
                    getStr(DataReaded.numberPowerFailuresAny, telegram, len, '(', ')');
                    break;
                }

                // 0-0:96.7.9(00001) Aantal lange onderbrekingen Elektriciteit
                if (strncmp(telegram, "0-0:96.7.9", strlen("0-0:96.7.9")) == 0)
                {
                    getStr(DataReaded.numberLongPowerFailuresAny, telegram, len, '(', ')');
                    break;
                }

                if (strncmp(telegram, "0-1:96.1.1", strlen("0-1:96.1.1")) == 0)
                {
                    getStr(DataReaded.equipmentId, telegram, len, '(', ')');
                    break;
                }

                break;

            default:
                break;
            }
        }
        return validCRCFound; // true if valid CRC found

    } // state = reading
    return false;
}

void P1Reader::getStr12(char* theValue, char* buffer, int maxlen, char startchar)
{
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = 12; // FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;
    char res[102];
    memset(res, 0, sizeof(res));

    if (strncpy(res, buffer + s + 1, l))
    {
        if (isNumber(res, l))
        {
            strncpy(theValue, res, l);
        }
        theValue[l + 1] = 0;
    }
}

void P1Reader::getValue(char* theValue, char* buffer, int maxlen, char startchar, char endchar)
{
    String cleanres = "";
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;

    char res[16];
    memset(res, 0, sizeof(res));

    if (strncpy(res, buffer + s + 1, l))
    {
        if (endchar == '*')
        {
            if (isNumber(res, l))
            {
                int flag = 1;
                for (int i = 0; i < l + 1; i++)
                {
                    if (flag == 1 && res[i] != '0')
                    {
                        flag = 0;
                    }

                    if (res[i] == '0' && res[i + 1] == '.')
                    {
                        flag = 0;
                    }

                    if (flag != 1)
                    {
                        if (!conf.watt) // BELGIQUE // report in Watts instead of KW
                        {
                            if (res[i] != '.')
                            {
                                cleanres += res[i];
                            }
                        }
                        else
                        {
                            cleanres += res[i];
                        }
                    }
                }
            }

            if (cleanres == "")
            {
                cleanres = "0"; // make sure there is a value to be be returned. 1.1.bea -19 Aug
            }

            cleanres.toCharArray(theValue, cleanres.length());
            theValue[cleanres.length() + 1] = 0;
        }
        else if (endchar == ')')
        {
            if (isNumber(res, l))
            {
                strncpy(theValue, res, l);
            }

            theValue[cleanres.length() + 1] = 0;
        }
    }
}

void P1Reader::getGasValue(char* theValue, char* buffer, int maxlen, char startchar, char endchar)
{
    String cleanres = "";
    int s = 0;

    if (FindCharInArrayRev2(buffer, ')', maxlen - 2) != -1) // some meters report the meterID in () before the section with actual gas value
    {
        s = FindCharInArrayRev2(buffer, ')', maxlen - 2) + 1;
    }
    else
    {
        s = FindCharInArrayRev(buffer, '(', maxlen - 2);
    }

    if (s < 8)
        return;
    if (s > 32)
        s = 32;
    int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
    if (l < 4)
        return;
    if (l > 12)
        return;
    char res[16];
    memset(res, 0, sizeof(res));
    if (strncpy(res, buffer + s + 1, l))
    {
        if (isNumber(res, l))
        {
            int flag = 1;
            for (int i = 0; i < l + 1; i++)
            {
                if (flag == 1 && res[i] != '0')
                {
                    flag = 0;
                }

                if (res[i] == '0' && res[i + 1] == '.')
                {
                    flag = 0;
                }

                if (flag != 1)
                {
                    if (!conf.watt)
                    {
                        if (res[i] != '.')
                        {
                            cleanres += res[i];
                        }
                    }
                    else
                    {
                        cleanres += res[i];
                    }
                }
            }
        }
        cleanres.toCharArray(theValue, cleanres.length());
        theValue[cleanres.length() + 1] = 0;
    }
}

void P1Reader::getDomoticzGasValue(char* theValue, char* buffer, int maxlen, char startchar, char endchar)
{
    String cleanres = "";
    int s = 0;

    if (FindCharInArrayRev2(buffer, ')', maxlen - 2) != -1) // some meters report the meterID in () before the section with actual gas value
    {
        s = FindCharInArrayRev2(buffer, ')', maxlen - 2) + 1;
    }
    else
    {
        s = FindCharInArrayRev(buffer, '(', maxlen - 2);
    }

    if (s < 8)
    {
        return;
    }

    if (s > 32)
    {
        s = 32;
    }

    int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
    if (l < 4)
    {
        return;
    }
    if (l > 12)
    {
        return;
    }

    char res[16];
    memset(res, 0, sizeof(res));
    if (strncpy(res, buffer + s + 1, l))
    {
        if (isNumber(res, l))
        {
            int flag = 1;
            for (int i = 0; i < l + 1; i++)
            {
                if (flag == 1 && res[i] != '0')
                {
                    flag = 0;
                }

                if (res[i] == '0' && res[i + 1] == '.')
                {
                    flag = 0;
                }
                if (flag != 1)
                {
                    if (res[i] != '.')
                    {
                        cleanres += res[i];
                    }
                }
            }
        }

        cleanres.toCharArray(theValue, cleanres.length());
        theValue[cleanres.length() + 1] = 0;
    }
}

unsigned long P1Reader::GetnextUpdateTime()
{
    return nextUpdateTime;
}

void P1Reader::DoMe()
{
    if ((millis() > nextUpdateTime))
    {
        if (!OEstate)
        {
            RTS_on();
            Serial.flush();
        }
    }

    if (OEstate)
    {
        readTelegram();
    }
}

void P1Reader::readTelegram()
{
    if (Serial.available())
    {
        memset(telegram, 0, sizeof(telegram));
        while (Serial.available())
        {
            int len = Serial.readBytesUntil('\n', telegram, MAXLINELENGTH);
            telegram[len] = '\n';
            telegram[len + 1] = 0;
            yield();
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            /*if (decodeTelegram(len + 1))
            { // test for valid CRC
                // ??????
                break;
            }
            else
            { // we don't have valid data, but still may need to report to Domo
                if (dataEnd && !CRCcheckEnaLED_BUILTIN)
                { // this is used for dsmr 2.2 meters
                  //  yield(); //state = WAITING;
                }
            }*/
        }
        digitalWrite(LED_BUILTIN, HIGH);
    }
}