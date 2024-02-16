#pragma once

#include <Print.h>
#include <PubSubClient.h>

class MQTTPrinter : public Print {
public:
  MQTTPrinter(PubSubClient *mqttClient, const char *topic, Print *backupPrinter)
      : mMQTTClient(NULL), mTopic(topic), mBackupPrinter(backupPrinter) {
    mMQTTClient = mqttClient;
    iBuffIndex = 0;
  };

  size_t write(uint8_t c) override {
    if (mqtt_client.connected()) {
      if (c == '\r' || c == '\n') {
        size_t length = iBuffIndex;
        bool success = mMQTTClient->publish(mTopic, mBuff.data(), length);
        std::fill(mBuff.begin(), mBuff.end(), 0);
        iBuffIndex = 0;
        if (success) {
          return length;
        }
        return 0;
      } else {
        mBuff[iBuffIndex] = c;
        iBuffIndex += 1;
      }
    } else if (mBackupPrinter) {
      return mBackupPrinter->write(c);
    }

    return 0;
  }

private:
  PubSubClient *mMQTTClient;
  const char *mTopic;
  Print *mBackupPrinter;
  int iBuffIndex;
  std::array<uint8_t, MQTT_MAX_PACKET_SIZE> mBuff;
};
