// CurrentArduino -> enCORE setup
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
Node nodes[] = {hub, node2, node3, node4, node5, node6}; //array containing previously defined Nodes
int nodeCount = 6; //number of objects in above array

//define timing routine constants
unsigned long last_time; //placeholder for timing routines
unsigned long send_time = 90000; //amount of time program sits collecting data before moving on

//WiFi placeholder variables
int status = WL_IDLE_STATUS;
WiFiClient client;

//WiFi network settings
char ssid[] = "enCORE_OSU"; //your network SSID (name)
char pass[] = "20solardec11"; //your network password

float hourDecimal; //variable for current time in decimal hours

float settings_high[6] = {73.0,73.0,73.0,73.0,73.0,73.0}; //default high settings at 73
float settings_low[6] = {71.0,71.0,71.0,71.0,71.0,71.0}; //default low settings at 71
float highTemp = 73.0;
float lowTemp = 71.0;

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


void setup() {
  analogReference(EXTERNAL); //external reference of 3.3V is used
  if(DEBUG) Serial.begin(9600); //for communication to/from computer

  Serial1.begin(9600); //for communication to/from XBee with Mega
  xbee.setSerial(Serial1);

  //set all heater pins as outputs & turn all off initially
  if(DEBUG) Serial.println("Initializing & turning all heaters off");
  for(int i=0; i<nodeCount+2; i++) {
    pinMode(pHeaters[i], OUTPUT);
  }
  
  if(DEBUG) {
    heatersON();
    for(int i=0; i<nodeCount; i++) {
      delay(1000);
      heaterOFF(i);
    }
  } else {
    heatersOFF();
  }

  //attempt to connect to Wifi network:
  connectWifi();
  if(DEBUG) printWifiStatus();

  if(DEBUG) Serial.println("All setup complete");

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
            if(DEBUG) {
              nodes[i].printAllCompact();
//              nodes[0].stashConvertHub();
//              nodes[0].printAllCompact();
            }
          }
        }
      }
    }
  } 
  else {  //if timeout occurs

    nodes[0].stashConvertHub(); //save hub data
    
    getSettings(); //get temperature setpoints
    control1(); //run control actuation

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

    last_time = millis(); //reset timing for next iteration
  }
}


//non-zoned control using hub as reference
void control1() {
  float referenceTemp = nodes[0].temp;
  highTemp = settings_high[0];
  lowTemp = settings_low[0];

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

  float referenceCoeffs[] = {.10, .15, .25, .15, .25, .10  };
  float referenceTemp = 0;
  for(int i=0; i<nodeCount; i++) {
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


void schedule1() {
  /* Simplified family schedule
   Sleep (until 7) -> Master bdrm, child bdrm
   Wake-up/breakfast (7-8) -> bathroom, kitchen, hw
   Morning (8-12) -> living rm, hw
   Lunch (12-1) -> living rm, kitchen
   Afternoon (1-5) -> living rm, hw
   Dinner (5-6) -> living rm, kitchen
   Evening (6-9) -> living rm, hw
   Bedtime (9-10) -> bathroom, hw, master bdrm, child bdrm
   */
  if(DEBUG) Serial.print("Family scheduling check...");

  if(hourDecimal < 7.0) {
    int activeArray[] = {
      0,0,0,1,1,0    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 8.0) {
    int activeArray[] = {
      1,0,1,0,0,1    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 12.0) {
    int activeArray[] = {
      1,1,0,0,0,0    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 13.0) {
    int activeArray[] = {
      0,1,1,0,0,1    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 17.0) {
    int activeArray[] = {
      1,1,0,0,0,0    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 18.0) {
    int activeArray[] = {
      0,1,1,0,0,0    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 21.0) {
    int activeArray[] = {
      1,1,0,0,0,0    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else if (hourDecimal < 22.0) {
    int activeArray[] = {
      1,0,0,1,1,1    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  } 
  else {
    int activeArray[] = {
      0,0,0,1,1,0    };
    for(int i=0; i<nodeCount; i++) {
      nodes[i].isActive(activeArray[i]);
    }
  }
  if(DEBUG) Serial.println("Complete");
}

//zoned control scheme using variable setpoints and hardcoded schedule
void control3() { 
  if(DEBUG)  Serial.println("Deadband control checks with individual references...");

  for(int i=0; i<nodeCount; i++) {
    if(nodes[i].active == 1) {
      if(nodes[i].temp < settings_low[i]) {
        heaterON(i);
        if(DEBUG) {
          Serial.print("Node ");
          Serial.print(i+1);
          Serial.println(" temperature needs raised");
        }
      } 
      else if (nodes[i].temp > settings_high[i]) {
        heaterOFF(i);
        if(DEBUG) {
          Serial.print("Node ");
          Serial.print(i+1);
          Serial.println(" temperature needs lowered");
        }
      } 
      else {
        if(DEBUG) {
          Serial.print("Node ");
          Serial.print(i+1);
          Serial.println(" temperature is good");
        }
      }
    } 
    else {
      heaterOFF(i);
      if(DEBUG) {
        Serial.print("Node ");
        Serial.print(i+1);
        Serial.println(" is inactive");
      }
    }
  }
  if(DEBUG) Serial.println("Control checks complete");
}

void heaterOFF(int p) {
  nodes[p].isActuated(0);
  digitalWrite(pHeaters[p], HIGH); //high turns heaters off
  if(DEBUG) {
    Serial.print("Heater ");
    Serial.print(p+1);
    Serial.println(" off");
  }
}

void heaterON(int p) {
  nodes[p].isActuated(1);
  digitalWrite(pHeaters[p], LOW); //low turns heaters on
  if(DEBUG) {
    Serial.print("Heater ");
    Serial.print(p+1);
    Serial.println(" on");
  }
}

void heatersOFF() {
  for(int i=0; i < nodeCount; i++) {
    heaterOFF(i);
    delay(100);
  }
}

void heatersON() {
  for(int i=0; i < nodeCount; i++) {
    heaterON(i);
    delay(100);
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
//      while(!client.connected()) { // && millis()-tStart<tSendTimeout
//        client.stop();
//        client.flush();
//        client.connect(server, 80);
//        delay(100);
//      }

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
      client.print("&active=");
      client.print(nodes[i].active);
      client.print("&settinghigh=");
      client.print(settings_high[i]);
      client.print("&settinglow=");
      client.print(settings_low[i]);
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

//gets temperature setpoints from website
void getSettings() {
  client.flush();
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /settings1.php?checksettingsa=true HTTP/1.1"); //needs modified for differing databases
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
    if(DEBUG) Serial.println("Could not get settings");
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
    if(DEBUG) Serial.println("Time could not be gotten");
    return;
  }

  while (client.available()) {     // change 1000 if your query is larger than 1000 characters
    char c1 = client.read();
    http_response1 += c1;   // We store the response in a string
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
