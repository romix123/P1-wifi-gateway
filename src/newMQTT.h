// void doMQTT(){
//   bool response =  mqtt_connect();
//   if (response) MQTT_reporter();
// }

// // void mqtt_connect(){
// //   if (!mqtt_client.connected()) {
// //      MqttConnected = false;
// //      Log.verboseln("Reconnecting to mqtt broker …");
// //      mqtt_reconnect();
// //   } else MqttConnected = true;
// // }

// // void mqtt_reconnect(){
// //   if (millis() > nextMQTTreconnectAttempt) {
// //     Log.verboseln("Attempting MQTT connection...");
// //     statusMsg = "Attempting MQTT connection...";
// //     // Create a random client ID
// //     String clientId = "P1 Smart Meter – ";
// //     clientId += String(random(0xffff), HEX);
// //     // Attempt to connect
// //     if (mqtt_client.connect(HOSTNAME, config_data.mqttUser,
// config_data.mqttPass)){
// //       Log.verboseln("   connected to broker");
// //       statusMsg += "   connected to broker";
// //       // Once connected, publish an announcement...
// //       mqtt_client.publish("outTopic", "p1 gateway running");
// //       // ... and resubscribe
// //       mqtt_client.subscribe("inTopic");
// //       MqttConnected = true;
// //     } else {
// //       Log.verbose("failed, rc=");
// //       statusMsg += "failed, rc=";
// //       Log.verbose(mqtt_client.state());
// //       Log.verboseln(" trying again later (non blocking)");
// //       statusMsg += " trying again later (non blocking)";
// //       nextMQTTreconnectAttempt = millis() + 15000; // try again in 15
// seconds
// //       MqttConnected = false;
// //     }
// //   }
// // }

// // void callback(char* topic, byte* payload, unsigned int length) {
// //   Log.verbose("Message arrived [");
// //   Log.verbose(topic);
// //   Log.verbose("] ");
// //   for (int i = 0; i < length; i++) {
// //     Log.verbose((char)payload[i]);
// //   }
// //   Log.verboseln();
// // }

// void mqtt_connect(){
// byte i = 0;
//   while (!mqttClient.connected() && (i < 3))
//   {
//     Log.verboseln("Attempting MQTT connection");
//     // Attempt to connect
//     boolean ret;

//     ret = mqtt_client.connect(HOSTNAME, config_data.mqttUser,
//     config_data.mqttPass); if (ret)
//     {
//       Log.verboseln("Connected to MQTT");
//       MqttConnected = true;
//       mqtt_client.publish("outTopic", "P1 gateway");
//       mqtt_client.subscribe("inTopic");
//       return true;
//     }
//     else
//     {
//       MqttConnected = false;
//       Log.verbose("Failed MQTT connection, return code:");

//       int Status = mqttClient.state();

//       switch (Status)
//       {
//       case -4:
//         Log.verboseln("Connection timeout");
//         break;

//       case -3:
//         Log.verboseln("Connection lost");
//         break;

//       case -2:
//         Log.verboseln("Connect failed");
//         break;

//       case -1:
//         Log.verboseln("Disconnected");
//         break;

//       case 1:
//         Log.verboseln("Bad protocol");
//         break;

//       case 2:
//         Log.verboseln("Bad client ID");
//         break;

//       case 3:
//         Log.verboseln("Unavailable");
//         break;

//       case 4:
//         Cdebugln("Bad credentials");
//         break;

//       case 5:
//         Log.verboseln("Unauthorized");
//         break;
//       }
//       if (i == 2) Log.verboseln("Retrying MQTT connection next cycle");
//       i++;
//       delay(1000);
//     }
//   }
//   return false;
// }
