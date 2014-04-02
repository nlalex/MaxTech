// Current Arduino -> James's setup
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true;

const float lowTemp = 71.0;
const float highTemp = 73.0;

float settings_high[6];
float settings_low[6];

const float hubTempAdjust = 0;
const float hubHumAdjust = 0;

#include <SPI.h>
#include <WiFi.h>
#include <XBee.h>
#include <Node.h>

XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

Node hub = Node(HUB_ADDR, HUB_NUM);
Node node2 = Node(addr2, 2);
Node node3 = Node(addr3, 3);
Node node4 = Node(addr4, 4);
Node node5 = Node(addr5, 5);
Node node6 = Node(addr6, 6);
Node nodes[] = {
  hub, node2, node3, node4, node5, node6}; //Array containing previously defined Nodes
//Node nodes[] = {hub, node2, node3, node4, node5, node6};
int nodeCount = 6;

unsigned long last_time; //Used for timing routines
unsigned long send_time = 90000; //SET TO 90000 -> amount of time program sits collecting data before moving on
unsigned long wait_time = 20000; //maximum wait time for calibration routine

int status = WL_IDLE_STATUS;
WiFiClient client;

char ssid[] = "ticklish_chickens";      //  your network SSID (name)
char pass[] = "esoom@!owl";   // your network password

boolean vOpen = false;

void setup()
{
  //analogReference(EXTERNAL);
  if(DEBUG) Serial.begin(9600); //For communication to/from computer

  Serial1.begin(9600); //For communication to/from XBee with Mega
  xbee.setSerial(Serial1);


  if(DEBUG) Serial.println("Initializing & closing all vents");
  for(int i=0; i<nodeCount; i++) { //may need dummy pins defined for padding
    pinMode(pVentPos[i], OUTPUT);
    pinMode(pVentNeg[i], OUTPUT);
    pinMode(pVentEnable[i], OUTPUT);
  }

  if(digitalRead(pCAL) == HIGH) {
    setEqual();
  }

  // attempt to connect to Wifi network:
  connectWifi();
  if(DEBUG) printWifiStatus();

  last_time = millis();
}


void loop() {
  if(millis()-last_time <= send_time) { //Timed loop functions
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {
      if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
        xbee.getResponse().getZBRxIoSampleResponse(response);
        for(int i=0; i < nodeCount; i++) {
          if(nodes[i].matchAddress(response)) {
            nodes[i].stashConvert(response);
            if(DEBUG) nodes[i].printAllCompact();
            if(DEBUG) {
              nodes[0].stashConvertHub();
              nodes[0].printAllCompact();
            }
          }
        }
      } 
      else {
        if(DEBUG) {
          Serial.print("Packet API ID not recognized: ");
          Serial.println(xbee.getResponse().getApiId(), HEX);
        }
      }
    }
  } else {  
    nodes[0].stashConvertHub();

    unsigned long start_send = millis();
    if(DEBUG) Serial.println("Sending data...");
    for(int i=0; i < nodeCount; i++) {
      if(DEBUG) nodes[i].printAllCompact();
      sendData(i);
    }     
    if(DEBUG) {
      Serial.print("All data sent in ");
      Serial.print((millis()-start_send)/1000.);
      Serial.println(" seconds");
    }

    last_time = millis();
  }
}


void control1(float referenceTemp) { //control scheme using single reference temperature (hub temp)
  if(DEBUG)  Serial.println("Beginning deadband control checks with single reference...");

  // hub.stashConvertHub();

  if(nodes[0].trip) {
    if(referenceTemp > highTemp) {
      //actuate to lower all temps
      if(DEBUG) Serial.println("Temperatures need lowered.");

      //heatersOFF();
      ventsOpen();

    } 
    else if(referenceTemp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Temperatures need raised.");

      //heatersON();
      ventsClose();

    } 
    else {
      //do nothing
      if(DEBUG) Serial.println("Temperatures are good.");
    }
  } 
  else {
    //data not received from hub
    if(DEBUG) Serial.println("Hub contains null data.");
  }

  if(DEBUG) Serial.println("Control checks completed.");
}


void control2() { //control scheme using individual node temperatures
  if(DEBUG)  Serial.println("Beginning deadband control checks with individual references...");

  if(hub.trip) {
    if(hub.temp > highTemp) {
      //actuate to lower temp
      if(DEBUG) Serial.println("Hub temperature needs lowered.");
    } 
    else if(hub.temp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Hub temperature needs raised.");
    } 
    else {
      //do nothing
      if(DEBUG) Serial.println("Hub temperature is good.");
    }
  } 
  else {
    //data not received from hub
    if(DEBUG) Serial.println("Hub contains null data.");
  }


  for(int i=0; i < nodeCount; i++) {
    if(DEBUG) {
      Serial.print("Node ");
      Serial.print(i+2);
    }
    if(nodes[i].trip) {
      if(nodes[i].temp > highTemp) {
        //actuate to lower temp
        if(DEBUG)  Serial.println(" temperature needs lowered.");
      } 
      else if(nodes[i].temp < lowTemp) {
        //actuate to raise temp
        if(DEBUG) Serial.println(" temperature needs raised.");
      } 
      else {
        //do nothing
        if(DEBUG) Serial.println(" temperature is good.");
      }
    } 
    else {
      //data not received from node
      if(DEBUG) Serial.println(" contains null data.");
    }
  }

  if(DEBUG) {
    Serial.println("Control checks completed.");
    Serial.println("");
  }
}

void printCSV() {
  for(int i=0; i<nodeCount; i++) {
    Serial.print(nodes[i].temp);
    Serial.print(",");
  }
}

void heatersOFF() {
  for(int i=0; i < nodeCount; i++) {
    //    delay(10);
    //    digitalWrite(pHeaters[i], HIGH); //high turns heaters off
    nodes[i].actuatedOFF();
  }
  for(int i=0; i < nodeCount+2; i++) {
    digitalWrite(pHeaters[i], HIGH); //high turns heaters off
    delay(10);
  }
}

void heatersON() {
  for(int i=0; i < nodeCount; i++) {
    //    delay(10);
    //    digitalWrite(pHeaters[i], LOW); //low turns heaters on
    nodes[i].actuatedON();
  }
  for(int i=0; i < nodeCount+2; i++) {
    digitalWrite(pHeaters[i], LOW); //low turns heaters on
    delay(10);
  }
}

void connectWifi() {
  while ( status != WL_CONNECTED) {
    if(DEBUG) Serial.print("Attempting to connect to SSID: ");
    if(DEBUG) Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void ventsHold() {
  for(int i=0; i<nodeCount; i++) {
    digitalWrite(pVentPos[i], LOW);
    digitalWrite(pVentNeg[i], LOW);
  }
}

void ventOpen(int p) {
  digitalWrite(pVentEnable[p], HIGH);
  delay(500);
  digitalWrite(pVentPos[p], HIGH);
  digitalWrite(pVentNeg[p], LOW);
  delay(tVentWait);
  digitalWrite(pVentPos[p], LOW);
  delay(500);
  digitalWrite(pVentEnable[p], LOW);
  nodes[p].actuated = 0;
}

void ventClose(int p) {
  digitalWrite(pVentEnable[p], HIGH);
  delay(500);
  digitalWrite(pVentPos[p], LOW);
  digitalWrite(pVentNeg[p], HIGH);
  delay(tVentWait);
  digitalWrite(pVentNeg[p], LOW);
  delay(500);
  digitalWrite(pVentEnable[p], LOW);
  nodes[p].actuated = 0;
}

void ventsClose() {
  if(vOpen == true) {
    for(int i=0; i<nodeCount; i++) {
      ventClose(i);
    }
  }
  vOpen = false;
}

void ventsOpen() {
  if(vOpen == false) {
    for(int i=0; i<nodeCount; i++) {
      ventOpen(i);
    }
  }
  vOpen = true;
}

void sendData(int i) {
  int sendCheck = 5;
  client.flush();
  client.stop();
  if(client.connect(server, 80)) {
    if(nodes[i].trip) {
      while(!client.connected()) { // && millis()-tStart<tSendTimeout
        client.stop();
        if(DEBUG) Serial.println("Problems connecting.  Trying again...");
        client.flush();
        client.connect(server, 80);
        delay(1000);
      }

      client.print("GET /hook2.php?node=");
      client.print(nodes[i].num);
      client.print("&temp=");
      client.print(nodes[i].temp);
      client.print("&humidity=");
      client.print(nodes[i].hum);
      client.print("&light1=");
      client.print(nodes[i]._ldr1);
      client.print("&light2=");
      client.print(nodes[i]._ldr2);
      client.print("&motion=");
      client.print(nodes[i]._pir);
      client.print("&heat=");
      client.print(nodes[i].actuated);
      client.print("&crt=");
      client.print(nodes[i].ct);
      client.println(" HTTP/1.1");
      client.println("Host: mesh.org.ohio-state.edu");
      client.println("User-Agent: ArduinoWiFi/1.1");
      client.println("Connection: close");
      client.println();
      //Serial.print("Data send to node ");
      //Serial.println(i+1);
      sendCheck = 0;
      delay(3000);
    } 
    else sendCheck = 1;
  } 
  else {
    //Serial.println("connection failed");
    client.stop();
    sendCheck = 2;
  }  
  if(DEBUG) {
    if(sendCheck == 0) {
      Serial.print(nodes[i].num);
      Serial.println(" sent successfully");
    } 
    else if(sendCheck == 1) {
      Serial.print(nodes[i].num);
      Serial.println(" did not send -> contains null data");
    } 
    else if(sendCheck == 2) {
      Serial.print(nodes[i].num);
      Serial.println(" did not send -> could not connect");
    } 
    else {
      Serial.print(nodes[i].num);
      Serial.println(" had unknown error");
    }
  }
  nodes[i].flush();
  //sendCheck = 5;
  //delay(tWaitSend);
}

//void getSettings() {
//  String http_response = "";
//  int response_start = 0;
//  int response_end = 0;
//  char c[] = "";
//  char buffer[10];
//
//  if (client.connect(server, 80)) {
//    Serial.println("connecting...");
//    // send the HTTP PUT request:
//    client.println("GET /settings2.php?checksettingsa=true HTTP/1.1");
//    client.println("Host: mesh.org.ohio-state.edu");
//    client.println("User-Agent: ArduinoWiFi/1.1");
//    client.println("Connection: close");
//    client.println();
//    delay(10000);
//  }
//  while (client.available()) {     // change 1000 if your query is larger than 1000 characters
//    char c = client.read();
//    http_response += c;   // We store the response in a string
//  }
//  response_start = http_response.indexOf("<data>")+6; 
//  response_end = http_response.indexOf("</data>");
//
//  char httpParse[response_end-response_start];
//  for(int i=0; i<(response_end-response_start); i++){
//    httpParse[i] = http_response.charAt(i+response_start);
//  }
//
//  for(int i=0; i<6; i++){
//    settings_high[i] = (httpParse[3+9*i]-48)*10+(httpParse[4+9*i]-48); 
//  }    
//
//  for(int i=0; i<6; i++){
//    settings_low[i] = (httpParse[6+9*i]-48)*10+(httpParse[7+9*i]-48); 
//  }  
//
//  if(DEBUG) {
//    Serial.println("Low/high temp settings: ");
//    for(int i=0; i<nodeCount; i++) {
//      Serial.print(settings_low[i]);
//      Serial.print("/");
//      Serial.println(settings_high[i]);
//    }
//  }
//}





