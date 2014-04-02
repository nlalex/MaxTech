// CurrentArduino -> enCORE setup
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true; //enable for debugging purposes

//hard-coded temperature thresholds
const float lowTemp = 71.0;
const float highTemp = 73.0;

//non-standard library inclusions
#include <SPI.h>
#include <WiFi.h>
#include <XBee.h>
#include <Node.h>

//XBee placeholder variables
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

//Node object creation -> create one per node, including hub
Node hub = Node(HUB_ADDR, HUB_NUM);
Node node2 = Node(addr2, 2);
Node node3 = Node(addr3, 3);
Node node4 = Node(addr4, 4);
Node node5 = Node(addr5, 5);
Node node6 = Node(addr6, 6);
Node nodes[] = {
  hub, node2, node3, node4, node5, node6}; //array containing previously defined Nodes
int nodeCount = 6; //number of objects in above array

//define timing routine constants
unsigned long last_time; //placeholder for timing routines
unsigned long send_time = 90000; //amount of time program sits collecting data before moving on

//WiFi placeholder variables
int status = WL_IDLE_STATUS;
WiFiClient client;

//WiFi network settings
char ssid[] = "enCORE_OSU"; //your network SSID (name)
char pass[] = "20solardec11"; // your network password


void setup()
{
  //analogReference(EXTERNAL); //only enable if analog reference other than 5V is used
  if(DEBUG) Serial.begin(9600); //for communication to/from computer

  Serial1.begin(9600); //for communication to/from XBee with Mega
  xbee.setSerial(Serial1);

  //set all heater pins as outputs & turn all off initially
  if(DEBUG) Serial.println("Initializing & turning all heaters off");
  for(int i=0; i<nodeCount+2; i++) {
    pinMode(pHeaters[i], OUTPUT);
  }
  heatersOFF();

  //attempt to connect to Wifi network:
  connectWifi();
  if(DEBUG) printWifiStatus();

  //initialize timing
  last_time = millis();
}


void loop() {
  if(millis()-last_time <= send_time) { //check to make sure timeout has not been reached
    xbee.readPacket(); //try to recieve XBee packet
    if (xbee.getResponse().isAvailable()) { //if packet was recieved
      if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) { //if packet recieved matches correct formatting
        xbee.getResponse().getZBRxIoSampleResponse(response); //store packet
        for(int i=0; i < nodeCount; i++) { //cycle through each node to see if address matches any nodes
          if(nodes[i].matchAddress(response)) { //if match is detected
            nodes[i].stashConvert(response); //save data to node
            if(DEBUG) nodes[i].printAllCompact();
          }
        }
      }
    }
  } 
  else {  //if timeout occurs

    nodes[0].stashConvertHub(); //save hub data

    //send data to database
    unsigned long start_send = millis();
    if(DEBUG) Serial.println("Sending data...");
    for(int i=0; i < nodeCount; i++) {
      if(DEBUG) nodes[i].printAllCompact();
      sendData(i); //also flushes data
    }
    if(DEBUG) {
      Serial.print("All data sent in ");
      Serial.print((millis()-start_send)/1000.);
      Serial.println(" seconds");
    }

    getSettings(); //gets temp settings from database

    //run control routine
    control1();

    last_time = millis(); //reset timing for next iteration
  }
}


//non-zoned control using hub as reference
void control1() {
  float referenceTemp = nodes[0].temp;
  
  if(DEBUG)  Serial.println("Beginning deadband control checks with single reference...");

  if(nodes[0].trip) {
    if(referenceTemp > highTemp) {
      //actuate to lower all temps
      if(DEBUG) Serial.println("Temperatures need lowered.");

      heatersOFF();

    } 
    else if(referenceTemp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Temperatures need raised.");

      heatersON();

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

//non-zoned control using weighted reference temperature
void control2() { 
  if(DEBUG)  Serial.println("Beginning deadband control checks with single reference...");

  float referenceCoeffs[] = {.10, .15, .25, .15, .10, .25};
  float referenceTemp;
  for(i=0; i<nodeCount; i++) {
    referenceTemp += referenceCoeffs[i] * nodes[i].temp;
  }
  
  if(DEBUG) {
    Serial.print("Reference temp: ");
    Serial.println(referenceTemp);
  }
  
  if(nodes[0].trip) {
    if(referenceTemp > highTemp) {
      //actuate to lower all temps
      if(DEBUG) Serial.println("Temperatures need lowered.");

      heatersOFF();

    } 
    else if(referenceTemp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Temperatures need raised.");

      heatersON();

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

//control scheme using individual node temperatures
void control3() { 
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

void sendData(int i) {
  int sendCheck = 5;
  client.flush();
  client.stop();
  if(client.connect(server, 80)) {
    if(nodes[i].trip) {
      while(!client.connected()) { // && millis()-tStart<tSendTimeout
        client.stop();
        client.flush();
        client.connect(server, 80);
        delay(100);
      }

      client.print("GET /hook1.php?node="); //needs modified for differing databases
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
      //            client.print("&crt=");
      //            client.print(nodes[i].ct);
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

void getSettings() {
  String http_response = "";
  int response_start = 0;
  int response_end = 0;
  char c[] = "";
  char buffer[10];

  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /settings1.php?checksettingsa=true HTTP/1.1"); //needs modified for differing databases
    client.println("Host: mesh.org.ohio-state.edu");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    delay(10000);
  }
  while (client.available()) {     // change 1000 if your query is larger than 1000 characters
    char c = client.read();
    http_response += c;   // We store the response in a string
  }
  response_start = http_response.indexOf("<data>")+6; 
  response_end = http_response.indexOf("</data>");

  char httpParse[response_end-response_start];
  for(int i=0; i<(response_end-response_start); i++){
    httpParse[i] = http_response.charAt(i+response_start);
  }

  for(int i=0; i<6; i++){
    settings_high[i] = (httpParse[3+9*i]-48)*10+(httpParse[4+9*i]-48); 
  }    

  for(int i=0; i<6; i++){
    settings_low[i] = (httpParse[6+9*i]-48)*10+(httpParse[7+9*i]-48); 
  }  

  if(DEBUG) {
    Serial.println("Low/high temp settings: ");
    for(int i=0; i<nodeCount; i++) {
      Serial.print(settings_low[i]);
      Serial.print("/");
      Serial.println(settings_high[i]);
    }
  }
}

