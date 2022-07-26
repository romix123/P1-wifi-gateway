/*
 * 
 * Debug functions
 * 
 * 
 */

 void PrintConfigData(){
  debug("SSID : ");
    debugln(user_data.ssid);
  debug("password : ");
    debugln(user_data.password);
  debug("DomoticxIP : ");
    debugln(user_data.domoticzIP);
  debug("DomoticzPort : ");
    debugln(user_data.domoticzPort);
  debug("domotixzGasIdx : ");
   debugln(user_data.domoticzGasIdx);
  debug("domotixzEnergyIdx : ");
   debugln(user_data.domoticzEnergyIdx);

  debug("mqttHost : ");
    debugln(user_data.mqttIP);
  debug("mqttPort : ");
    debugln(user_data.mqttPort);
  debug("mqttUser : ");
    debugln(user_data.mqttUser);
  debug("mqttPass : ");
    debugln(user_data.mqttPass);
  debug("mqttTopic : ");
    debugln(user_data.mqttTopic);

  debug("interval : ");
    debugln(user_data.interval);
  debug("json : ");
    debugln(user_data.domo);
  debug("mqtt : ");
    debugln(user_data.mqtt);
  debug("watt : ");
      debugln(user_data.watt);

}
