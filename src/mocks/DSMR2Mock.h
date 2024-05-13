#pragma once
// std
#include <string>
// libraries
#include <ArduinoLog.h>
// project
#include "SmartMeterSerialMock.h"
#include "format.h"

class DSMR2Mock : public SmartMeterSerialMock {
public:
  DSMR2Mock(uint8_t reqPin) : SmartMeterSerialMock(reqPin) {}

protected:
  std::string getDatagramBody() {
    Log.traceln("DSMR2Mock::getDatagramBody");
    return fmt::format("/KMP5 ZABF001552883211\r\n"
                       "\r\n"
                       "0-0:96.1.1({equipment_id})\r\n"        // equipment_id
                       "1-0:1.8.1({:.3f}*kWh)\r\n" // energy_delivered_tariff1
                       "1-0:1.8.2({:.3f}*kWh)\r\n" // energy_delivered_tariff2
                       "1-0:2.8.1({:.3f}*kWh)\r\n" // energy_returned_tariff1
                       "1-0:2.8.2({:.3f}*kWh)\r\n" // energy_returned_tariff2
                       "0-0:96.14.0(0002)\r\n"     // electricity_tariff
                       "1-0:1.7.0(0001.60*kW)\r\n" // power_delivered
                       "1-0:2.7.0(0000.00*kW)\r\n" // power_returned
                       "0-0:96.13.1()\r\n"         // message_short
                       "0-0:96.13.0()\r\n"         // message_long
                       "0-1:24.1.0(3)\r\n"         // gas_device_type
                       "0-1:96.1.0({})\r\n"        // gas_equipment_id
                       "0-1:24.3.0({})(00)(60)(1)(0-1:24.2.1)(m3) "
                       "({:.3f})\r\n", // gas_delivered_text
                       mEquipmentId, getEnergyDeliveredHigh() / float(1000),
                       getEnergyDeliveredLow() / float(1000),
                       getEnergyReturnedHigh() / float(1000),
                       getEnergyReturnedLow() / float(1000), mGasEquipmentId,
                       getTimeStamp().c_str(), getGasDelivered() / float(1000));
  }
};
