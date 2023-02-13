
  P1 wifi gateway 2022
  
  Deze software brengt verschillende componenten bij elkaar in een handzaam pakket.
  
  De software draait op een ESP8285 of ESP8266 en heeft een bescheiden voetafdruk.
  
  De data processing is gebaseerd op: http://domoticx.com/p1-poort-slimme-meter-data-naar-domoticz-esp8266/
  De captive portal is gebaseerd op een van de ESP8266 voorbeelden, de OTA eenheid eveneens.
  
  De module levert data aan een host via JSON of MQTT messages, maar kan ook via de webinterface van de module of via telnet worden uitgelezen
  
  De module zal bij opstarten eerst de buffercondensator laden. 
  Vervolgens knippert hij 2x en zal eerst proberen te koppelen aan je wifi netwerk. Gedurende dat proces zal de LED knipperen
  met een periode van 0.5 Hz.
  Wanneer geen verbinding kan worden gemaakt start de modiule een Access point onder de naam P1_Setup
  Verbind met dit netwerk en surf naar 192.168.4.1
  
  Het menuscherm van de module verschijnt dan. Vul daar SSID en wachtwoord van je wifi netwerk in. 
  Daaronder vul je gegevens van de ontvangede server in (IP en poortnummer). Dit kan je DOmotociz server zijn
  In dat geval moet je ook het IDx dat Domoticz maakt voor een Gas device en een Energy device invoeren.
  Tenslotte geef je aan per hoeveel seconde je data naar de server wilt laten sturen.
  DSMR4 meters hanteren een meetinterval van 10 sec. SMR5 meters leveren iedere seconde data. Dat lijkt me wat veel en 
  kost de module ook veel stroom waardoor het maar de vraag is of hij parasitair (door de meter gevoed) kan werken.
  Ik raad een interval van 15 sec of hoger aan. Het interval dat je hier invoert zal niet altijd synchroon lopen met
  het data interval van de meter, ga er dus niet van uit dat je exact iedere x seconden een meetwaarde ziet in DOmoticz. 
  Tussen metingen 'slaapt' de module (de modem wordt afgeschakeld waardoor het stroomverbruik van zo'n 70mA terugvalt naar 15 mA). 
  Dit geeft de bufferelco tijd om op te laden voor de stroompiekjes die de wifi zender van de module produceert 
  (en het bespaart hoe dan ook wat stroom (die weliswaar door je energieleverancier wordt betaald, maar toch). Alle kleine 
  beetjes helpen..
 
   informatie, vragen, suggesties ed richten aan romix@macuser.nl 
   
   
     
   versie: 1.1adc 
   datum:  12 Feb 2023
   auteur: Ronald Leenes
   
   1.1adc: implemented extensive logging
   			updated Swedish version to align with Dutch version (German and French versions lack security features on setup/update)
   1.1aa: bug fixes
   1.1: implemented graphs
   		fixed reporting tariffIndicatorElectricity
   
   ud: fixed refreshheaders
   ua: fixed setup field issue  
   u: - added password protection for Setup and Update Firmware menus. On first boot of version 1.0u, go to Setup menu 
        within 60 seconds to bypass password protection.
      - added out of the box support for Home Assistant "DSMR Reader" integration. Use 'dsmr' as MQTT prefix and data will
        automatically be delivered in "DSMR Reader" format to the MQTT broker.
      - fixed MQTT reconnect, it is now non-blocking.
      - fixed hardboot daycounters reset
      - fixed sending empty MQTT messages
      - overall improved stability
      
   t: improvements on powermanagement, overall (minor) cleanup
   ta: fix for Telnet reporting
 		added French localisation
   
   s: added German localisation
         Added mqtt output for Swedish specific OBIS codes
         
   r: speed improvements and streamlining of the parser
       localisations for: NL, SE
       
   q: added daily values
   p: incorporated equipmentID in mqtt set
   o: fixed gas output, fixed mqtt reconnect
   n: empty call to SetupSave now redirects to main menu instead of resetting settings ;-)
       fixed kWh/Wh inversion
   m: setupsave fix, relocate to p1wifi.local na 60 sec 
       mqtt - kw/W fix
   l: wifireconnect after wifi loss
   k: fixed big BUG, softAP would not produce accessible webserver.
   j: raw data on port 23
       major code rewrite
       implemented data reporting methods: 
         parsed data: json, mqtt, p1wifi.local/P1
         raw data: telnet on port 23, p1wifi.local/Data
         
   i:  extended max line length for TD210-D meters, which have a really loong 1-0:99.97.0 line
   h:  extended mqtt set with instant powers, voltages, actual tarif group. power outages, power drops
   g: fixed mqtt
   
   Generic ESP8285 module 
	Flash Size: 2mb (FS: 128Kb, OTA: –960Kb) 
   
   needed files: 
   this one (obv), 
   CRC16.h
   JSON.ino
   MQTT.ino
   TELNET.ino
   debug.ino
   decoder.ino
   functions.ino
   graph.ino
   webserver.ino
   webserverDE.ino 
   webserverSE.ino
   webserverNL.ino
   wifi.ino 
