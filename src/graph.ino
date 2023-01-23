/*
 * Copyright (c) 2022 Ronald Leenes
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
 */

/**
 * @file graph.ino
 * @author Ronald Leenes
 * @date 28.12.2022
 * @version 1.0u 
 *
 * @brief This file contains the graphing functions
 *
 * @see http://esp8266thingies.nl
 */

// graph.js Ref: https://stackoverflow.com/q/59951311/9112798

#if GRAPH == 1


void handleGraphMenu(){
  String str="";
 
  addHead(str);
  addIntro(str);
  
  str += F("<html>\n<head>\n");
  str += F("<title>Slimme meter</title>");
  str += F("</head><body>\n");

  str += F("<fieldset><legend><b> Grafieken </b></legend>");
  str += F("<form action='/SelectGraph' method='POST'>");
  str += F("<input type='radio' id='day' name='period' value='day'> vandaag <input type='radio' id='week' name='period' value='week'> deze week ");
  str += F("<input type='radio' id='month' name='period' value='month'> deze maand <input type='radio' id='year' name='period' value='year'> dit jaar</br>");
  
//  str += F("<input type='radio' id='day' name='period' value='day'><label for='day'>vandaag</label>");
//  str += F("<input type='radio' id='week' name='period' value='week'><label for='week'>deze week</label>");
//  str += F("<input type='radio' id='month' name='period' value='month'><label for='month'>deze maand</label>");
//  str += F("<input type='radio' id='year' name='period' value='year'><label for='year'>dit jaar</label></br>");

  
  str += F("<p><button name='graphtype' value='1' type='submit'>Energieverbruik T1/T2</button>");
  str += F("<p><button name='graphtype' value='2' type='submit'>Energie retour T1/T2</button>");
  str += F("<p><button name='graphtype' value='3' type='submit'>Energie totaal/retour</button>");
  str += F("<p><button name='graphtype' value='4' type='submit'>Gasverbruik</button></form>");
  str += F("</fieldset>");
  
  str += F("<fieldset><legend><b> Jaaroverzicht</b></legend>");
  str += F("<form action='/GasCalendar' method='POST'>");
  str += F("<p><button type='submit'>Gasverbruik dit jaar</button></form>");
  str += F("</fieldset>");

//  str += F("<form action='/Dir' method='GET'><button type='submit'>Directory</button></form>");
//  str += F("<a href=\"/DumpLogs\">Dump log files</a><br>");

  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
  addFootBare(str); 
  server.send(200, "text/html", str);
}



void DumpDataFiles(){
  listDir("/");
  debugln();
  

}



void selectGraph(){
char switcher[3];
    strncpy(switcher, server.arg("graphtype").c_str(), server.arg("graphtype").length() );
    switcher[1] ='\0';

char period[6];
    strncpy(period, server.arg("period").c_str(), server.arg("period").length() );
    period[server.arg("period").length()] ='\0';
      
  debug("Selected graph: ");
  debugln(switcher);
    debug("Selected period: ");
  debugln(period);
  
  if (server.method() == HTTP_POST){
  //  int switcher = atoi(server.arg("datatype").c_str());
    debugln(switcher);
   switch (atoi(switcher)){
    case 1: //delivered T1/T2
      theGraph("E1.log", "E2.log", "Elektriciteit gebruik T1", "Elektriciteit gebruik T2", "[{label:'uur', type:'number'},{label:'kWh', type:'number'}],", period );
      break;
    case 2: // returned T1/T2
      theGraph("R1.log", "R2.log","Elektriciteit retour T1", "Elektriciteit retour T2", "[{label:'uur', type:'number'},{label:'kWh', type:'number'}],", period);
      break;
    case 3: //Total E
      theGraph("TE.log", "TR.log", "Elektriciteit totaal gebruik", "Elektriciteit totaal retour", "[{label:'uur', type:'number'},{label:'kWh', type:'number'}],", period);
      break; 
    case 4: //Gas
      theGraph("G.log", "","Gas","", "[{label:'uur', type:'number'},{label:'kubieke meter', type:'number'}],", period);
      break;  
    default:
      break; 
   }
  }
}

void theGraph(const char * type1, const char * type2, String title1, String title2, String label, String period ){
  monitoring = false;
String str ="";
char buffer[64];
char path1[20];
char path2[20];
String pageTitle ="";

if (period =="day") {
  strcpy(path1, "/Hour");
  strcat(path1, type1);
  strcpy(path2, "/Hour");
  strcat(path2, type2);
  pageTitle = " Vandaag ";
} else if (period =="week") {
    strcpy(path1, "/Week");
    strcat(path1, type1);
    strcpy(path2, "/Week");
    strcat(path2, type2);
    pageTitle = " Deze week ";
}else if (period =="month") {
    strcpy(path1, "/Month");
    strcat(path1, type1);
    strcpy(path2, "/Month");
    strcat(path2, type2);
    pageTitle = " Deze maand ";

} else if (period =="year") {
    strcpy(path1, "/Year");
    strcat(path1, type1);
    strcpy(path2, "/Year");
    strcat(path2, type2);
    pageTitle = " Dit jaar ";
}

debug("file1 to read: ");
debugln(path1);
debug("file2 to read: ");
debugln(path2);

File file1 = LittleFS.open(path1, "r");
File file2 = LittleFS.open(path2, "r");

addGraphHead(str);
str += F("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
str += F("<script type=\"text/javascript\">google.charts.load('current', {'packages':['corechart']});");
str += F("google.charts.setOnLoadCallback(drawChart1);");
if (type2[0] != '\0'){
  str += F("google.charts.setOnLoadCallback(drawChart2);");
}

server.sendContent(str);
debugln(str);

str = F("function drawChart1() { var data = new google.visualization.arrayToDataTable([");
str += label; //F("[\"uur\", \"m^3\"], ");
server.sendContent(str);
debugln(str);

  if (!file1) {
    debugln("Failed to open file for reading");
    server.sendContent("['0', 0],");
    debug("['0', 0], ");
  } else {
    while (file1.available()) {
      int l = file1.readBytesUntil('\n', buffer, sizeof(buffer));
      buffer[l] = 0;
      server.sendContent(buffer);
      debugln(buffer);
    }
    file1.close();
  }
 delay(200);
 str = F("]);");
 str += F("var options = {title: '");
 str += title1; 
 str += F(" '};"); //'Gasverbruik per uur in m^3'};");
 str += F("var chart = new google.visualization.ColumnChart(document.getElementById('Chart1'));");
 str += F("chart.draw(data, options); }");
 server.sendContent( str);
 debugln(str);

 delay(200);
 str ="";

if (type2[0] != '\0'){
  str = F("function drawChart2() { var data = new google.visualization.arrayToDataTable([");
  str += label; //F("[\"uur\", \"m^3\"], ");
  server.sendContent( str);
  debugln(str);

  if (!file2) {
    debugln("Failed to open file for reading");
    server.sendContent("['0', 0],");
    debug("['0', 0], ");
  } else {
    while (file2.available()) {
      int l = file2.readBytesUntil('\n', buffer, sizeof(buffer));
      buffer[l] = 0;
      server.sendContent(buffer);
      debug(str);
    }
    file2.close();
    delay(200);
  }
  str = F("]);");
  str += F("var options = {title: '");
  str += title2; //'Gasverbruik per uur in m^3'
  str += F(" '};"); 
  str += F("var chart = new google.visualization.ColumnChart(document.getElementById('Chart2'));");
  str += F("chart.draw(data, options); }");
  server.sendContent ( str);
  debugln(str);
}  //only if we have a second file to display

  str = F("</script>");
  str += F("</head><body>");
  str +=("<div style='text-align:left;display:inline-block;color:#000000;width:600px;'><div style='text-align:center;color:#000000;'><h2>P1 wifi-gateway</h2></div><br>");

  str += F("<fieldset><legend><b>");
  str += pageTitle;
  str += F("</b></legend>");
  str += F("<div id=\"Chart1\" style=\"width:100%; max-width:1200px; height:500px\"></div><br>");
  str += F("Totaal deze periode: ");
  str += "nog te implementeren</br><hr>";
if (type2[0] != '\0'){
  str += F("<div id=\"Chart2\" style=\"width:100%; max-width:1200px; height:500px\"></div><br>");
  str += F("Totaal deze periode: ");
  str += "nog te implementeren</br></fieldset>";
}
  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
  addFootBare(str);   
  server.sendContent ( str);
  debugln(str);
  server.sendContent(F(""));
 monitoring = true;
}

void calendarGas(){
String str ="";
char buffer[64];
File file = LittleFS.open("/YearGc.log", "r");
monitoring = false;


addGraphHead(str);

str += F("<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script><script type='text/javascript'>google.charts.load('current', {packages:['calendar']});google.charts.setOnLoadCallback(drawChart);");
str += F("function drawChart() {var dataTable=new google.visualization.DataTable(); dataTable.addColumn({ type: 'date', id: 'Datum' }); dataTable.addColumn({type: 'number', id: 'Gebruik'});");
str += F("dataTable.addRows([");

server.sendContent(str);
debugln(str);

  if (!file) {
    debugln("Failed to open file for reading");
    server.sendContent("[ new Date(2023,0,1), 0],");
  } else {
  while (file.available()) {
   int l = file.readBytesUntil('\n', buffer, sizeof(buffer));
   buffer[l] = 0;
   server.sendContent("[ new Date");
   server.sendContent(buffer);
  }
  file.close();
  delay(200);
 }
 str = "";
 str += F("]); var chart = new google.visualization.Calendar(document.getElementById('calendar_gas')); var options = { title: 'Gas gebruiksintensiteit', height: 350};");
 str += F("chart.draw(dataTable, options);}</script></head><body><div id='calendar_gas' style='width: 1000px; height: 350px;'></div>");

  str += F("<form action='/' method='POST'><button class='button bhome'>Menu</button></form>");
  addFootBare(str);   
  server.sendContent (str);
  debugln(str);
  server.sendContent(F(""));
 
  monitoring = true;
}







#endif // GRAPH
