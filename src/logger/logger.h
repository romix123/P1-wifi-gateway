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

static struct TerminalColors {
  // https://en.wikipedia.org/wiki/ANSI_escape_code
  const char *RESET = "\033[0m"; // Reset or normal 
  
  // 1-9 Font styles
  const char *STYLE_BOLD = "\033[1m"; // Bold or increased intensity
  const char *STYLE_FAINT = "\033[2m"; // Faint, decreased intensity, or dim 
  const char *STYLE_ITALIC = "\033[2m"; // Not widely supported. Sometimes treated as inverse or blink.[25]
  
  const char *FONT_DEFAULT = "\033[10m"; // Primary (default) font 

  // 30–37 Set foreground color 
  const char *TEXT_BLACK = "\033[30m";
  const char *TEXT_RED = "\033[31m";
  const char *TEXT_GREEN = "\033[32m";
  const char *TEXT_YELLOW = "\033[33m";
  const char *TEXT_BLUE = "\033[34m";
  const char *TEXT_MAGENTA = "\033[35m";
  const char *TEXT_CYAN = "\033[36m";
  const char *TEXT_WHITE = "\033[37m";
  const char *TEXT_DEFAULT = "\033[39m"; // DEFAULT

  // 40–47 Set background color 
  const char *BACK_BLACK = "\033[40m";
  const char *BACK_RED = "\033[41m";
  const char *BACK_GREEN = "\033[42m";
  const char *BACK_YELLOW = "\033[43m";
  const char *BACK_BLUE = "\033[44m";
  const char *BACK_MAGENTA = "\033[45m";
  const char *BACK_CYAN = "\033[46m";
  const char *BACK_WHITE = "\033[47m";
  const char *BACK_DEFAUKLT= "\033[49m"; // DEFAULT

  // 90–97  Set bright foreground color 
  const char *TEXT_ALT_BLACK = "\033[90m";
  const char *TEXT_ALT_RED = "\033[91m";
  const char *TEXT_ALT_GREEN = "\033[92m";
  const char *TEXT_ALT_YELLOW = "\033[93m";
  const char *TEXT_ALT_BLUE = "\033[94m";
  const char *TEXT_ALT_MAGENTA = "\033[95m";
  const char *TEXT_ALT_CYAN = "\033[96m";
  const char *TEXT_ALT_WHITE = "\033[97m";

  const char *FATAL = "\033[41m\033[37m"; // BACK_RED + TEXT_WHITE;
  
  std::map<int, const char *> logLevelToColor{
      {LOG_LEVEL_SILENT, ""},
      {LOG_LEVEL_FATAL, FATAL},
      {LOG_LEVEL_ERROR, TEXT_ALT_RED},
      {LOG_LEVEL_WARNING, TEXT_ALT_YELLOW},
      {LOG_LEVEL_INFO, TEXT_ALT_BLUE},
      {LOG_LEVEL_TRACE, TEXT_ALT_WHITE},
      {LOG_LEVEL_VERBOSE, ""}};

  const char *getColorFromLogLevel(int logLevel) {
    return logLevelToColor[logLevel];
  }
} terminalColors;

static void printPrefix(Print *_logOutput, int logLevel) {
  _logOutput->print(terminalColors.getColorFromLogLevel(logLevel));
}

static void printSuffix(Print *_logOutput, int logLevel) {
  _logOutput->print(terminalColors.RESET);
}

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
    Log.setPrefix(printPrefix);
    Log.setSuffix(printSuffix);
    Log.setShowLevel(false);
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
