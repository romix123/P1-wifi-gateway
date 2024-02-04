#pragma once

#include <Print.h>

#define LOG_PRINTER_NULL 0
#define LOG_PRINTER_SERIAL 1
#define LOG_PRINTER_TELNET 2
#define LOG_PRINTER_MQTT 3

#ifndef PRINTER
#define PRINTER LOG_PRINTER_SERIAL
#endif

#if PRINTER == LOG_PRINTER_NULL
#define DISABLE_LOGGING
#endif
#if PRINTER == LOG_PRINTER_SERIAL
#include <HardwareSerial.h>
#endif
#if PRINTER == LOG_PRINTER_TELNET
#include "TelnetPrinter.h"
#endif
#if PRINTER == LOG_PRINTER_MQTT
#include "MQTTPrinter.h"
#endif

class LogPrinterCreator {
public:
  /**
   * Creates a Print instance corresponding to the given settings.
   *
   * @return Pointer to a Print instance.
   */
  Print *createLogPrinter() {
#if PRINTER == LOG_PRINTER_SERIAL
    mLogPrinter = &Serial;
#endif
#if PRINTER == LOG_PRINTER_TELNET
    mLogPrinter = new TelnetPrinter(24, &Serial);
#endif
#if PRINTER == LOG_PRINTER_MQTT
    mLogPrinter = new MQTTPrinter(&mqtt_client, "p1wifi/logging", &Serial);
#endif
    return mLogPrinter;
  };

private:
  Print *mLogPrinter;
};
