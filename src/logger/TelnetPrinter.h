#pragma once

#include <ArduinoLog.h>
#include <Print.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#ifndef MAX_TELNET_PRINTER_CLIENTS
#define MAX_TELNET_PRINTER_CLIENTS 5
#endif

static WiFiServer telnetServer(24);
static WiFiClient telnetPrinterClients[MAX_TELNET_PRINTER_CLIENTS];

void logPrinterTelnetloop() {
  int iClient;

  // Search for new clients and accept those clients
  if (telnetServer.hasClient()) {
    Log.verboseln("Telnet has new client");
    for (iClient = 0; iClient < MAX_TELNET_PRINTER_CLIENTS; iClient++) {
      WiFiClient telnetClient = telnetPrinterClients[iClient];
      if (!telnetClient.connected()) {
        telnetPrinterClients[iClient] = telnetServer.accept();
        Log.verboseln("TelnetPrinter: New client at index %d", iClient);
        break;
      }
    }
  }

  // Clear all incoming data. Logger is sending only
  for (iClient = 0; iClient < MAX_TELNET_PRINTER_CLIENTS; iClient++) {
    WiFiClient telnetClient = telnetPrinterClients[iClient];
    telnetClient.flush();
  }
}

class TelnetPrinter : public Print {
public:
  TelnetPrinter(const int port, Print *backupPrinter)
      : mBackupPrinter(backupPrinter) {
    telnetServer.setNoDelay(true);
    telnetServer.begin(port);

    mTelnetClients = telnetPrinterClients;
  };

  size_t write(uint8_t c) override {
    // If there are Telnet clients connected, write to the Telnet clients.
    // If not, write to the backup printer if exists.
    if (hasClients()) {
      return writeAllClients(c);
    } else if (mBackupPrinter) {
      return mBackupPrinter->write(c);
    }
    return 0;
  }

private:
  WiFiClient *mTelnetClients;
  Print *mBackupPrinter;

  /**
   * Checks if there are Telnet clients connected and are available for writing.
   * 
   * @return bool: true if there are Telnet clients, false otherwise.
  */
  bool hasClients() {
    int iClient;
    for (iClient = 0; iClient < MAX_TELNET_PRINTER_CLIENTS; iClient++) {
      WiFiClient telnetClient = mTelnetClients[iClient];
      if (telnetClient.availableForWrite() > 0) {
        return true;
      }
    }
    return false;
  }

  /**
   * Writes the same date to all the active Telnet clients.
   * 
   * @param c: Data to write.
   * @return size_t: Total size of the written characters to all clients.
  */
  size_t writeAllClients(uint8_t c) {
    int iClient;
    size_t totalSize = 0;
    for (iClient = 0; iClient < MAX_TELNET_PRINTER_CLIENTS; iClient++) {
      WiFiClient telnetClient = mTelnetClients[iClient];
      if (telnetClient.availableForWrite() > 0) {
        totalSize += telnetClient.write(c);
      }
    }
    return totalSize;
  }
};
