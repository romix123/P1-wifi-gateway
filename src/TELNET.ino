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
 * @file TELNET.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 *
 * @brief This file contains the TELNET interface functions 
 *
 * @see http://esp8266thingies.nl
 */

 
void setupTelnet() {  
  // passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  
  // passing a lambda function
  telnet.onInputReceived([](String str) {
    // checks for a certain command
    if (str == "ping") {
      telnet.println("> pong");
      debugln("- Telnet: pong");
    // disconnect the client
    } else if (str == "bye") {
      telnet.println("> disconnecting you...");
      telnet.disconnectClient();
      }
  });

  debug("- Telnet: ");
  if (telnet.begin(port)) {
    debugln("running");
  } else {
    debugln("error.");
  }
}


void TelnetReporter(){
  if (telnetConnected){
    telnet.print(datagram);
    debugln("Raw Data Published to Telnet connection.");
   // debugln(datagram);
  }
}



void onTelnetConnect(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln(" connected");
  telnetConnected = true;
 }

void onTelnetDisconnect(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln(" disconnected");
  telnetConnected = false;
}

void onTelnetReconnect(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln(" reconnected");
  telnetConnected = true;
}

void onTelnetConnectionAttempt(String ip) {
  debug("- Telnet: ");
  debug(ip);
  debugln("  tried to connect ");
}
