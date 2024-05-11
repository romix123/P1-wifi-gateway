// std
#include <iomanip>
#include <math.h>
#include <string.h>
#include <stdio.h>
// libraries
#include <ArduinoLog.h>
// project
#include "format.h"
#include "SmartMeterSerialMock.h"

unsigned long SEC_UNTIL_2000 = 946681200UL;  // Unix timestamp at 1/1/2000
unsigned long SEC_UNTIL_2015 = 1420066800UL; // Unix timestamp at 1/1/2015
unsigned long SEC_PER_YEAR =
    31557600UL; // Seconds in 1 year (365,25 * 24 * 60 * 60)

int SmartMeterSerialMock::available(void) {
  int state = digitalRead(mReqPin);
  // Each 10.000 ms write mock datagram;
  if (state == HIGH && mNextMockTime < millis() && timeLibReady()) {
    Log.traceln("Start writing mock datagram");
    writeDatagram();
    mNextMockTime = millis() + 10000;
  }
  return mBuffWrite - mBuffRead;
}

int SmartMeterSerialMock::read(void) {
  if (available()) {
    if (mBuffRead < mBuffMax) {
      int res = mBuff[mBuffRead++];
      if (mBuffRead == mBuffWrite) {
        Log.traceln("All of mock buffer red. Resetting to 0.");
        flush();
      }
      return res;
    } else {
      Log.errorln("Read Buffer at max");
    }
  }
  return -1;
}

void SmartMeterSerialMock::flush(void) {
  Log.traceln("SmartMeterSerialMock::flush");
  static_cast<void>(mBuff.empty());
  mBuffRead = 0;
  mBuffWrite = 0;
}

void SmartMeterSerialMock::writeDatagram() {
  Log.traceln("DSMR5Mock::writeDatagram");
  std::string body = getDatagramBody();
  uint16_t crc = calculateCRC(body.c_str(), body.size());
  std::string message = fmt::format("{}!{:04X}\r\n", body, crc);
  write(message);
}

size_t SmartMeterSerialMock::write(const uint8_t val) {
  if (mBuffWrite < mBuffMax) {
    mBuff[mBuffWrite++] = val;
    return 1;
  }
  Log.errorln("Write Buffer at max");
  return 0;
}

size_t SmartMeterSerialMock::write(const uint8_t *buffer, size_t size) {
  IAMSLOW();
  Log.traceln("Writing buffer of %d characters.", size);
  Log.verboseln("%s", buffer);

  size_t n = 0;
  while (size--) {
    size_t ret = write(pgm_read_byte(buffer++));
    if (ret == 0) {
      // Write of last byte didn't complete, abort additional processing
      break;
    }
    n += ret;
  }
  return n;
}

/**
 * Mock data calculated with usage starting at 1/1/2000 with 700 kWh / year.
 * Does not take high tarif times into account.
 * @return amount of Wh delivered since 2000.
 */
uint32_t SmartMeterSerialMock::getEnergyDeliveredHigh() {
  return calculateReading(SEC_UNTIL_2000, 700 * 1000);
}

/**
 * Mock data calculated with usage starting at 1/1/2000 with 500 kWh / year.
 * Does not take low tarif times into account.
 * @return amount of Wh delivered since 2000.
 */
uint32_t SmartMeterSerialMock::getEnergyDeliveredLow() {
  return calculateReading(SEC_UNTIL_2000, 500 * 1000);
}

/**
 * Mock data calculated with return starting at 1/1/2015 with 600 kWh / year.
 * Does not take high tarif times into account.
 * @return amount of Wh returned since 2015.
 */
uint32_t SmartMeterSerialMock::getEnergyReturnedHigh() {
  return calculateReading(SEC_UNTIL_2015, 600 * 1000);
}

/**
 * Mock data calculated with return starting at 1/1/2015 with 200 kWh / year.
 * Does not take low tarif times into account.
 * @return amount of Wh returned since 2015.
 */
uint32_t SmartMeterSerialMock::getEnergyReturnedLow() {
  return calculateReading(SEC_UNTIL_2015, 200 * 1000);
}

/**
 * Mock data calculated with return starting at 1/1/2000 with 800 m3 / year.
 * @return amount of m3*1000 returned since 2015.
 */
uint32_t SmartMeterSerialMock::getGasDelivered() {
  return calculateReading(SEC_UNTIL_2015, 800 * 1000);
}

std::string SmartMeterSerialMock::getTimeStamp() {
  std::time_t t = std::time(nullptr);
  std::string datetime(13, 0);
  datetime.resize(std::strftime(&datetime[0], datetime.size(), "%y%m%d%H%M%S",
                                std::gmtime(&t)));
  return datetime;
}

uint32_t SmartMeterSerialMock::calculateReading(unsigned long secZero,
                                                uint32_t yearUsage) {
  if (!timeLibReady()) {
    Log.warningln("TimeLib not ready yet.");
    return 0;
  }

  uint32 currSeconds = std::time(nullptr);
  unsigned long secondsSince2000 = currSeconds - secZero;
  float years = float(secondsSince2000) / SEC_PER_YEAR;
  return std::round(years * yearUsage);
}

bool SmartMeterSerialMock::timeLibReady() {
  return std::time(nullptr) > SEC_PER_YEAR;
}

uint16_t SmartMeterSerialMock::calculateCRC(const char *body, size_t size) {
  Log.traceln("SmartMeterSerialMock::calculateCRC of size %d", size);
  // Calculate CRC of the body
  uint16_t crc = 0;
  crc = CRC16(crc, (unsigned char *)body, size);
  unsigned char excl = '!';
  crc = CRC16(crc, &excl, 1);
  // for (unsigned int i = 0; i < size; i++) {
  //   if (body[i] == '\0') {
  //     crc = CRC16(crc, '!', 1);
  //     break;
  //   }
  //   crc = CRC16(crc, body, 1);
  // }
  return crc;
}