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

#include "TelnetMgr.h"

TelnetMgr::TelnetMgr(settings &currentConf) : conf(currentConf), telnet(TELNETPORT)
{
    telnet.setNoDelay(true);
    telnet.begin();
}

void TelnetMgr::DoMe(String Diagram)
{
    if (telnet.hasClient()) // avons nous une nouvelle connexion
    {
        // find free/disconnected spot
        int i;
        for (i = 0; i < MAX_SRV_CLIENTS; i++)
        {
            if (!telnetClients[i]) // equivalent to !serverClients[i].connected()
            {
                telnetClients[i] = telnet.accept();
                telnetClients[i].printf("You are connected to the telnet. Your id session is %d\n", i);
                SendDebugPrintf("Telnet : New session (Id:%d)", i);
                break;
            }
        }
        
        // no free/disconnected spot so reject
        if (i == MAX_SRV_CLIENTS)
        {
            telnet.accept().printf("Server Telnet is busy with %d active connections.\n", MAX_SRV_CLIENTS);
            SendDebugPrintf("Server Telnet is busy with %d active connections", MAX_SRV_CLIENTS);
        }
    }
    TelnetReporter(Diagram);
}

void TelnetMgr::SendDebug(String payload)
{
    if (!conf.debugToTelnet)
    {
        return;
    }

    char result[100];
    printf(result, "%s %s", "[DEBUG]", payload.c_str());

    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    {
        if (telnetClients[i])
        {
            if (telnetClients[i].availableForWrite() > 0)
            {
                telnetClients[i].println(result);
            }
        }
    }
}

void TelnetMgr::TelnetReporter(String Diagram)
{
    if (millis() < NextReportTime)
    {
        return;
    }

    NextReportTime = millis() + (TELNET_REPPORT_INTERVAL_SEC * 1000);

    int maxToTcp = 0;
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    {
        if (telnetClients[i])
        {
            int afw = telnetClients[i].availableForWrite();
            if (afw)
            {
                if (!maxToTcp)
                {
                    maxToTcp = afw;
                }
                else
                {
                    maxToTcp = std::min(maxToTcp, afw);
                }
            }
            else
            {
                // warn but ignore congested clients
                SendDebugPrintf("[TELNET] Client %d is congested, kill connection.", i);
                telnetClients[i].flush();
                telnetClients[i].stop();
            }
        }
    }

    int len = Diagram.length();

    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    {
        if (telnetClients[i].availableForWrite() >= 1)
        {
            telnetClients[i].write(Diagram.c_str(), len);
        }
        else
        {
            SendDebugPrintf("[TELNET] Client %d is not available for writing.", i);
        }
    }  
    yield();
}