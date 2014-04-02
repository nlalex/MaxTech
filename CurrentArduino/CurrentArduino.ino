// Current Arduino -> James's setup
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true; //enable for debugging purposes

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
Node nodes[] = {hub, node2, node3, node4, node5, node6}; //Array containing previously defined Nodes
int nodeCount = 6; //number of objects in above array

//define timing routine constants
unsigned long last_time; //Used for timing routines
unsigned long send_time = 90000; //amount of time program sits collecting data before moving on

//WiFi placeholder variables
int status = WL_IDLE_STATUS;
WiFiClient client;

//WiFi network settings
char ssid[] = "ticklish_chickens";      //  your network SSID (name)
char pass[] = "esoom@!owl";   // your network password

float hourDecimal; //variable for current time in decimal hours

float settings_high[6] = {73.0,73.0,73.0,73.0,73.0,73.0}; //default high settings at 73
float settings_low[6] = {71.0,71.0,71.0,71.0,71.0,71.0}; //default low settings at 71

boolean vOpen = false;
int heat = 0;

String http_response = "";
int response_start = 0;
int response_end = 0;
char c[] = "";
char buffer[10];

String http_response1 = "";
int response_start1 = 0;
int response_end1 = 0;
char c1[] = "";
char buffer1[10];

void setup()
{
  analogReference(EXTERNAL); //3.3V analog reference is used to help with temperature readings
  if(DEBUG) Serial.begin(9600); //For communication to/from computer

  Serial1.begin(9600); //For communication to/from XBee with Mega
  xbee.setSerial(Serial1);

  if(DEBUG) Serial.println("Initializing & turning heater off");
  pinMode(pHEAT, OUTPUT);
  if(DEBUG) Serial.println("Initializing & closing all vents");
  for(int i=0; i<nodeCount; i++) { //may need dummy pins defined for padding
    pinMode(pVentPos[i], OUTPUT);
    pinMode(pVentNeg[i], OUTPUT);
    pinMode(pVentEnable[i], OUTPUT);
  }

  if(DEBUG) {
    ventsOpen();
    for(int i=0; i<nodeCount; i++) {
      delay(1000);
      ventClose(i);
    }
  } else {
    ventsClose();
  }

  // attempt to connect to Wifi network:
  connectWifi();
  if(DEBUG) printWifiStatus();
  
  getTime();
  
  if(DEBUG) Serial.println("All setup complete");

  //initialize timing
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
            if(DEBUG) {
              nodes[i].printAllCompact();
              nodes[0].stashConvertHub();
              nodes[0].printAllCompact();
            }
          }
        }
      } 
    }
  } 
  else {  
    nodes[0].stashConvertHub();

    getTime();
    float override = schedule2();
    getSettings();
    control1(override);
    
    //send data to database
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
  if(nodes[p].actuated != 1) {
    digitalWrite(pVentEnable[p], HIGH);
    delay(500);
    digitalWrite(pVentPos[p], HIGH);
    digitalWrite(pVentNeg[p], LOW);
    delay(tVentWait);
    digitalWrite(pVentPos[p], LOW);
    delay(500);
    digitalWrite(pVentEnable[p], LOW);
    if(DEBUG) {
      Serial.print("Vent ");
      Serial.print(p+1);
      Serial.println(" opened");
    }
  } else if(DEBUG) {
    Serial.print("Vent ");
      Serial.print(p+1);
      Serial.println(" already open");
  }
  nodes[p].isActuated(1);
}

void ventClose(int p) {
  if(nodes[p].actuated != 0) {
    digitalWrite(pVentEnable[p], HIGH);
    delay(500);
    digitalWrite(pVentPos[p], LOW);
    digitalWrite(pVentNeg[p], HIGH);
    delay(tVentWait);
    digitalWrite(pVentNeg[p], LOW);
    delay(500);
    digitalWrite(pVentEnable[p], LOW);
    if(DEBUG) {
      Serial.print("Vent ");
      Serial.print(p+1);
      Serial.println(" closed");
    }
  } else if(DEBUG) {
    Serial.print("Vent ");
    Serial.print(p+1);
    Serial.println(" already closed");
  }
  nodes[p].isActuated(0);
}

void ventsClose() {
  for(int i=0; i<nodeCount; i++) {
    ventClose(i);
  }
}

void ventsOpen() {
  for(int i=0; i<nodeCount; i++) {
    ventOpen(i);
  }
}

void sendData(int i) {
  int sendCheck = 5;
  client.flush();
  client.stop();
  if(client.connect(server, 80)) {
    if(nodes[i].trip) {
//      while(!client.connected()) { // && millis()-tStart<tSendTimeout
//        client.stop();
//        if(DEBUG) Serial.println("Problems connecting.  Trying again...");
//        client.flush();
//        client.connect(server, 80);
//        delay(1000);
//      }

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
      client.print(heat);
      client.print("&active=");
      client.print(nodes[i].active);
      client.print("&settinghigh=");
      client.print(settings_high[i]);
      client.print("&settinglow=");
      client.print(settings_low[i]);
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

//gets temperature setpoints from website
void getSettings() {
  client.flush();
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /settings2.php?checksettingsa=true HTTP/1.1"); //needs modified for differing databases
    client.println("Host: mesh.org.ohio-state.edu");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    delay(10000);
    http_response = "";
    response_start = 0;
    response_end = 0;
    char c[] = "";
  } 
  else {
    client.stop();
    if(DEBUG) Serial.println("Settings could not be gotten");
    return;
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

void getTime() {
  client.flush();
  client.stop();
  if (client.connect(server, 80)) {
    if(DEBUG) Serial.println("Connecting to receive time...");
    // send the HTTP PUT request:
    client.println("GET /time.php?check=true HTTP/1.1"); //needs modified for differing databases
    client.println("Host: mesh.org.ohio-state.edu");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    delay(10000);
    http_response1 = "";
    response_start1 = 0;
    response_end1 = 0;
    char c1[] = "";
  } 
  else {
    client.stop();
    if(DEBUG) Serial.println("Could not get time");
    return;
  }

  while (client.available()) {     // change 1000 if your query is larger than 1000 characters
    char c = client.read();
    http_response1 += c;   // We store the response in a string
  }
  response_start1 = http_response1.indexOf("<data>")+6; 
  response_end1 = http_response1.indexOf("</data>");

  char httpParse1[response_end1-response_start1];
  for(int i=0; i<(response_end1-response_start1); i++){
    httpParse1[i] = http_response1.charAt(i+response_start1);
  }
//  hourDecimal = (httpParse[0]-48)*10+(httpParse[1]-48);
  hourDecimal = ((httpParse1[0]-48)*10+(httpParse1[1]-48)) + ((httpParse1[3]-48)*10+(httpParse1[4]-48))/60.0 + ((httpParse1[6]-48)*10+(httpParse1[7]-48))/3600.0;
  if(DEBUG) {
    Serial.print("Time: ");
    Serial.println(hourDecimal);
  }
}

float schedule2() {
  /* Simplified family schedule
   Sleep (until 9) -> Master bdrm, living, kitchen, hallway
   Work (9-4) -> All open, low setpoint of 65
   Evening (4-12) -> Master bdrm, living, kitchen, office, hallway
   */
  if(DEBUG) Serial.print("James scheduling check...");

  if(hourDecimal < 9.0) {
    int activeArray[] = {1,0,0,1,1,1};
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } else if(hourDecimal < 16.0) {
    int activeArray[] = {1,1,1,1,1,1};
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
      if(DEBUG) Serial.println("Complete - Reference Point Override");
      return 65.0;
    }
  } else {
    int activeArray[] = {1,0,1,1,1,1};
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  }
  
  if(DEBUG) Serial.println("Complete");
  return 0.0;
}


//zoned control scheme using variable setpoints and hardcoded schedule
void control1(float override) { 
  if(DEBUG)  Serial.println("Deadband control checks with individual references...");
  
  int n=0;
  float refLOW = 0.0;
  float refHIGH = 0.0;
  float refTEMP = 0.0;
  for(int i=0; i<nodeCount; i++) {
    if(nodes[i].active==1) {
      ventOpen(i);
      refTEMP += nodes[i].temp;
      refLOW += settings_low[i];
      refHIGH += settings_high[i];
      n++;
    } else {
      ventClose(i);
    }
  }
  
  refTEMP /= float(n);
  refLOW /= float(n);
  refHIGH /= float(n);
  
  if(override != 0.0) {
    refLOW = override;
    refHIGH = refLOW + 3;
  }
  
  if(DEBUG) {
    Serial.print("Reference temperature: ");
    Serial.println(refTEMP);
    Serial.print("Low/high reference limits: ");
    Serial.print(refLOW);
    Serial.print("/");
    Serial.println(refHIGH);
  }
  
  if(refTEMP < refLOW) {
    if(DEBUG) Serial.println("Temperature needs raised");
    heatON();
  } else if(refTEMP > refHIGH) {
    if(DEBUG) Serial.println("Temperature needs lowered");
    heatOFF();
  } else {
    if(DEBUG) Serial.println("Temperature is good");
  }      
  
  if(DEBUG) Serial.println("Control checks complete");
}

void heatON() {
  digitalWrite(pHEAT, LOW);
  if(DEBUG) Serial.println("Heater on");
  heat = 1;
}

void heatOFF() {
  digitalWrite(pHEAT, HIGH);
  if(DEBUG) Serial.println("Heater off");
  heat = 0;
}

