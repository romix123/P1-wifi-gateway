#pragma once
// std
#include <array>
#include <time.h>
// libraries
#include <HardwareSerial.h>
// project
#include "CRC16.h"

class SmartMeterSerialMock : public HardwareSerial {
public:
  SmartMeterSerialMock(uint8_t reqPin)
      : HardwareSerial(UART_NO), mReqPin(reqPin){};
  void begin(unsigned long baud) {}
  int available(void) override;
  int read(void) override;
  void flush(void) override;
  size_t write(uint8_t) override;
  size_t write(const char *str) {
    if (str == NULL)
      return 0;
    return write((const uint8_t *)str, strlen_P(str));
  }
  size_t write(const char *buffer, size_t size) {
    return write((const uint8_t *)buffer, size);
  }
  size_t write(const uint8_t *buffer, size_t size);
  size_t write(std::string str) {
    return write(str.c_str(), str.size());
  }

  uint32_t getEnergyDeliveredHigh();
  uint32_t getEnergyDeliveredLow();
  uint32_t getEnergyReturnedHigh();
  uint32_t getEnergyReturnedLow();
  uint32_t getGasDelivered();
  std::string getTimeStamp();

protected:
  const char *mEquipmentId = "1234567890123456789012345678901234";
  const char *mGasEquipmentId = "9876543210987654321098765432109876";
  void writeDatagram();
  virtual std::string getDatagramBody() = 0;
  bool timeLibReady() ;
  uint16_t calculateCRC(const char *body, size_t size);
  uint32_t calculateReading(unsigned long secZero, uint32_t yearUsage);

private:
  uint8_t mReqPin; // Request pin
  int mBuffMax = 2048;
  int mBuffRead = 0;
  int mBuffWrite = 0;
  uint32_t mNextMockTime = 0;
  std::array<uint8_t, 2048> mBuff;
};
