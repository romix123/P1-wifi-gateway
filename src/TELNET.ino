/* 
 *  Telnet support routines
 *  
 */

 
/*
 *    Telnet support routines
 * 
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
