// void doMQTT(){
//   bool response =  mqtt_connect();
//   if (response) MQTT_reporter();
// }

// // void mqtt_connect(){
// //   if (!mqtt_client.connected()) {
// //      MqttConnected = false;
// //      debugln("Reconnecting to mqtt broker …");
// //      mqtt_reconnect();
// //   } else MqttConnected = true;
// // }

// // void mqtt_reconnect(){
// //   if (millis() > nextMQTTreconnectAttempt) {
// //     debugln("Attempting MQTT connection...");
// //     statusMsg = "Attempting MQTT connection...";
// //     // Create a random client ID
// //     String clientId = "P1 Smart Meter – ";
// //     clientId += String(random(0xffff), HEX);
// //     // Attempt to connect
// //     if (mqtt_client.connect(HOSTNAME, config_data.mqttUser,
// config_data.mqttPass)){
// //       debugln("   connected to broker");
// //       statusMsg += "   connected to broker";
// //       // Once connected, publish an announcement...
// //       mqtt_client.publish("outTopic", "p1 gateway running");
// //       // ... and resubscribe
// //       mqtt_client.subscribe("inTopic");
// //       MqttConnected = true;
// //     } else {
// //       debug("failed, rc=");
// //       statusMsg += "failed, rc=";
// //       debug(mqtt_client.state());
// //       debugln(" trying again later (non blocking)");
// //       statusMsg += " trying again later (non blocking)";
// //       nextMQTTreconnectAttempt = millis() + 15000; // try again in 15
// seconds
// //       MqttConnected = false;
// //     }
// //   }
// // }

// // void callback(char* topic, byte* payload, unsigned int length) {
// //   debug("Message arrived [");
// //   debug(topic);
// //   debug("] ");
// //   for (int i = 0; i < length; i++) {
// //     debug((char)payload[i]);
// //   }
// //   debugln();
// // }

// void mqtt_connect(){
// byte i = 0;
//   while (!mqttClient.connected() && (i < 3))
//   {
//     debugln("Attempting MQTT connection");
//     // Attempt to connect
//     boolean ret;

//     ret = mqtt_client.connect(HOSTNAME, config_data.mqttUser,
//     config_data.mqttPass); if (ret)
//     {
//       debugln("Connected to MQTT");
//       MqttConnected = true;
//       mqtt_client.publish("outTopic", "P1 gateway");
//       mqtt_client.subscribe("inTopic");
//       return true;
//     }
//     else
//     {
//       MqttConnected = false;
//       debug("Failed MQTT connection, return code:");

//       int Status = mqttClient.state();

//       switch (Status)
//       {
//       case -4:
//         debugln("Connection timeout");
//         break;

//       case -3:
//         debugln("Connection lost");
//         break;

//       case -2:
//         debugln("Connect failed");
//         break;

//       case -1:
//         debugln("Disconnected");
//         break;

//       case 1:
//         debugln("Bad protocol");
//         break;

//       case 2:
//         debugln("Bad client ID");
//         break;

//       case 3:
//         debugln("Unavailable");
//         break;

//       case 4:
//         Cdebugln("Bad credentials");
//         break;

//       case 5:
//         debugln("Unauthorized");
//         break;
//       }
//       if (i == 2) debugln("Retrying MQTT connection next cycle");
//       i++;
//       delay(1000);
//     }
//   }
//   return false;
// }
