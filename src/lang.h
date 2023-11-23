/*
 * localisations
 *
 */

#ifdef ENGLISH
#define HELP "/Help' target='_blank'>Help</a>"
#define TIMESTAMP "%d days %d hours %d minutes"
#define LAST_SAMPLE " last sample: "
#define FIRMWARE_REV " firmware version: "

#define MAIN_METER_VALUES
#define MAIN_GRAPHS
#define MAIN_SETUP
#define MAIN_UPDATE

#define SETUP_SAVED_TITLE
#define SETUP_SAVED_TEXT

#define FIRMWARE_UPDATE_SUCCESS_TITLE
#define FIRMWARE_UPDATE_SUCCESS_TEXT

#define HELP_TITLE
#define HELP_TEXT
#endif

#ifdef NEDERLANDS

#define HELP "/Help' target='_blank'>Help</a>"
#define TIMESTAMP "%d dagen %d uren %d minuten"
#define LAST_SAMPLE " laatste sample: "
#define FIRMWARE_REV " firmware versie: "

#define MAIN_METER_VALUES                                                      \
  "<form action='/P1' method='post'><button type='p1' class='button "          \
  "bhome'>Meterdata</button></form>"
#define MAIN_GRAPHS                                                            \
  "<form action='/Grafieken' method='GET'><button "                            \
  "type='submit'>Grafieken</button></form>"
#define MAIN_SETUP_TITLE "<fieldset><legend><b> Setup </b></legend>"
#define MAIN_SETUP                                                             \
  "<form action='/Setup' method='post'><button "                               \
  "type='Setup'>Setup</button></form>"
#define MAIN_UPDATE                                                            \
  "<form action='/Update' method='GET'><button type='submit'>Update "          \
  "firmware</button></fieldset></form>"

#define SETUP_SAVED_TITLE                                                      \
  "<fieldset><legend><b>Wifi en module setup</b></legend>"
#define SETUP_SAVED_TEXT                                                       \
  "<p><b>De instellingen zijn succesvol bewaard</b><br><br>De module zal nu herstarten. \
  Dat duurt ongeveer een minuut.</p><br><p>De led zal langzaam knipperen tijdens koppelen aan uw WiFi netwerk.</p><p>\
  Als de blauwe led blijft branden is de instelling mislukt en zult u opnieuw moeten koppelen met Wifi netwerk 'P1_Setup'.</p>"

#define FIRMWARE_UPDATE_SUCCESS_TITLE                                          \
  "<fieldset><legend><b>Firmware update</b></legend>"
#define FIRMWARE_UPDATE_SUCCESS_TEXT                                           \
  "<p><b>De firmware is succesvol bijgewerkt</b><br><br><p>De module zal nu herstarten. \
  Dat duurt ongeveer 30 sec</p><p>De blauwe Led zal 2x oplichten wanneer de module klaar is met opstarten</p><p>\
  De led zal langzaam knipperen tijdens koppelen aan uw WiFi netwerk.</p><p>Als de blauwe led blijft branden is de instelling mislukt \
  en zult u opnieuw moeten koppelen met Wifi netwerk 'P1_Setup'.</p>"

#define HELP_TITLE "<fieldset><legend><b>Help</b></legend>"
#define HELP_TEXT1                                                             \
  "<p><b>De P1 wifi-gateway kan op verschillende manieren data "               \
  "afleveren.</b><br><br><p>Altijd beschikbaar zijn de kernwaarden via "       \
  "<b>meterstanden</b> of via"
#define HELP_RAW "<p>De ruwe data is beschikbaar via "
#define HELP_TEXT2                                                             \
  "<p>Meer gangbaar is het gebruik van de gateway met een domotica systeem.</p>\
    <p><b>Domoticz</b> accepteert json berichten. Geef hiertoe het IP adres van <br>\
    je Domoticz server op en het poortnummer waarop deze kan worden benaderd (doorgaans 8080).</p>\
    De benodigde Idx voor gas en electra verkrijgt u door eerst in Domoticz virtule sensors \
    voor beiden aan te maken. Na creatie verschijnen de Idxen onder het tabblad 'devices'.</p><br><br>\
    Data kan ook (door Domoticz bijvoorbeeld) worden uitgelezen via poort 23 van de module (p1wifi.local:23).\
    Dit is de manier waarop Domoticz hardware device [P1 Smart Meter with LAN interface] data kan ophalen.\
    Aan de p1wifi kant hoeft daarvoor niets te worden ingesteld (geen vinkjes bij json en mqtt).<br><br>\
    Voor andere systemen kan je gebruik maken van een MQTT broker. Vul de gegevens van de broker in, en stel het root topic in. \
    Voor Home Assistant is 'dsmr' handig omdat dan de 'DSMR Reader' integration in HA kan worden gebruikt. Die verweerkt de data van \
    de module keurig.</p>\
    Geef met de checkboxes aan welke rapportage methode(n) je wilt gebruiken.</p>"

#endif
