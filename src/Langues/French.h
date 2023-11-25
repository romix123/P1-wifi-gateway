/*
 * Copyright (c) 2023 Jean-Pierre sneyers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Additionally, please note that the original source code of this file
 * may contain portions of code derived from (or inspired by)
 * previous works by:
 *
 * Ronald Leenes (https://github.com/romix123/P1-wifi-gateway and http://esp8266thingies.nl)
 * MQTT part based on https://github.com/daniel-jong/esp8266_p1meter/blob/master/esp8266_p1meter/esp8266_p1meter.ino
 */

#ifdef FRENCH
#endif

const char LANG[] = R"=====(
{
    "H1Welcome" : "Merci de définir un mot de passe",
    "PSWD1" : "Mot de passe",
    "PSWD2" : "le retaper",
    "PSWDERROR" : "Les deux mot de passe ne sont pas identique.",
    "HEADERLG" : "fr",
    "MENU" : "Menu",
    "MENUP1" : "Valeurs mesurées",
    "MENUConf" : "Configuration",
    "MENUOTA" : "Mettre à jour le micrologiciel",
    "MENURESET" : "Retour aux paramètres d'usine",
    "H1DATA" : "Données",
    "ConfH1": "Wifi et configuration du module",
    "ACTIONSAVE": "Sauvegarder",
    "Conf-Saved": "Les paramètres ont été enregistrés avec succès.",
    "ConfReboot": "Le module va maintenant redémarrer. Cela prendra environ une minute.",
    "ConfLedStart": "La Led bleue s'allumera 2x lorsque le module aura fini de démarrer.",
    "ConfLedError" : "Si la LED bleue reste allumée, c'est que le réglage a échoué. Reconnectez vous alors au réseau WiFi 'P1_Setup' pour corriger les paramètres.",
    "ConfWIFIH2" : "Paramètres Wi-Fi",
    "ConfSSID" : "SSID",
    "ConfWIFIPWD" : "Mot de passe",
    "ConfDMTZH2" : "Paramètres Domoticz",
    "ConfDMTZBool" : "Envoyer à Domoticz ?",
    "ConfDMTZIP" : "Adresse IP Domoticz",
    "ConfDMTZPORT" : "Port Domoticz",
    "ConfDMTZGIdx" : "Domoticz Gaz Idx",
    "ConfDMTZEIdx" : "Domoticz Energy Idx",
    "ConfMQTTH2" : "Paramètres MQTT",
    "ConfMQTTBool" : "Activer le protocole MQTT ?",
    "ConfMQTTIP" : "Adresse IP du serveur MQTT",
    "ConfMQTTPORT" : "Port du serveur MQTT",
    "ConfMQTTUsr" : "Utilisateur MQTT",
    "ConfMQTTPSW" : "Mot de passe MQTT",
    "ConfMQTTRoot" : "Rubrique racine MQTT",
    "ConfMQTTIntr" : "Intervalle de mesure en sec",
    "ConfMQTTKW" : "Rapport en watts (au lieu de kWh)",
    "ConfTLNETBool" : "Activer le port Telnet (23)",
    "ConfTLNETDBG" : "Debug via Telnet ?",
    "ConfMQTTDBG" : "Debug via MQTT ?",
    "ConfSave" : "Sauvegarder",
    "OTAH1" : "Mettre à jour le micrologiciel",
    "OTAFIRMWARE" : "Micrologiciel",
    "OTABTUPDATE" : "Mettre à jour",
    "OTASUCCESS1" : "Le micrologiciel a été mis à jour avec succès.",
    "OTASUCCESS2" : "Le module va redémarrer. Cela prend environ 30 secondes.",
    "OTASUCCESS3" : "La LED bleue s'allumera deux fois une fois que le module aura terminé son démarrage.",
    "OTASUCCESS4" : "La LED clignotera lentement pendant la connexion à votre réseau WiFi.",
    "OTASUCCESS5" : "Si la LED bleue reste allumée, la configuration a échoué et vous devrez refaire la connexion avec le réseau WiFi 'P1_Setup",
    "DATAH1" : "Valeurs mesurées",
    "DATAFullL" : "Consommation heures creuses: total",
    "DATAFullProdL" : "Production heures creuses: total",
    "DATACurAmp" : "Consommation instantanée de courant",
    "DATAFullH" : "Consommation heures pleines: total",
    "DATAFullProdH" : "Production heures pleines: total",
    "DATACurProdAmp" : "Production instantanée de courant",
    "DATATODAY" : "aujourd'hui",
    "DATAUL1" : "Tension: L1",
    "DATAUL2" : "Tension: L2",
    "DATAUL3" : "Tension: L3",
    "DATAAL1" : "Ampérage: L1",
    "DATAAL2" : "Ampérage: L2",
    "DATAAL3" : "Ampérage: L3",
    "DATAGFull" : "Consommation de gaz: total",
    "HLPH1" : "Aide",
    "HLPTXT1" : "La passerelle WiFi P1 peut fournir des données de 4 manières.",
    "HLPTXT2" : "Les valeurs fondamentales sont toujours disponibles via le menu P1",
    "HLPTXT3" : "Le plus courante est l'utilisation de la passerelle avec un système domotique.",
    "HLPTXT4" : "Domoticz accepte les messages json. Pour ce faire, entrez l'adresse IP de votre serveur Domoticz et le numéro de port sur lequel il est accessible (généralement 8080).",
    "HLPTXT5" : "Vous obtenez les Idx nécessaires pour le gaz et l'électricité en créant d'abord des capteurs virtuels pour les deux dans Domoticz. Une fois créés, les Idx apparaîtront sous l'onglet 'devices'.",
    "HLPTXT6" : "Les données peuvent également être lues (par Domoticz par exemple) via le port 23 du module (p1wifi.local:23).C'est ainsi que le dispositif matériel Domoticz (P1 Smart Meter avec interface LAN) peut récupérer des données, il n'a rien à régler (ne pas cochesr le json et le MQTT). Pour les autres systèmes, vous pouvez utiliser un serveur MQTT. Remplissez les paramètres du serveur et définissez le sujet racine. Pour Home Assistant, il s'agit de 'capteurs/alimentation/p1mètre'.",
    "HLPTXT7" : "Daniel de Jong décrit sur son github comment configurer davantage HA. <br>Utilisez les cases à cocher pour indiquer la ou les méthodes de déclaration que vous souhaitez utiliser.",
    "UPTIME" :  "%d jours %d heures %d minutes",
    "H1ADMIN" : "Identification",
    "PSWDLOGIN" : "Identifiant",
    "ADMINPSD" : "Mot de passe",
    "ACTIONLOGIN" : "Se connecter",
    "H1WRONGPSD" : "Erreur",
    "WRONGPSDTXT" : "Le mot de passe que vous avez entré est incorrecte.",
    "WRONGPSDBACK" : "Retour",
    "RF_RESTARTH1" : "Opération réussie",
    "RF_RESTTXT" : "Merci de vous connecter au Wifi du module une fois qu'il aura redémarré",
    "ASKCONFIRM" : "Êtes-vous sûr de vouloir réaliser cette action ?",
    }
)=====";