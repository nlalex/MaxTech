// TestNode01 -> enCORE setup
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true;

const float lowTemp = 71.0;
const float highTemp = 73.0;

const float hubTempAdjust = 0;
const float hubHumAdjust = 0;

#include <SPI.h>
#include <WiFi.h>
#include <XBee.h>
//#include <Config_enCORE.h>
#include <Node.h>

XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

Node hub = Node(HUB_ADDR, HUB_NUM);
Node node2 = Node(addr2, 2);
Node node3 = Node(addr3, 3);
Node node4 = Node(addr4, 4);
Node node5 = Node(addr5, 5);
Node node6 = Node(addr6, 6);
Node nodes[] = {hub, node2, node3, node4, node5, node6}; //Array containing previously defined Nodes
//Node nodes[] = {hub, node2, node3, node4, node5, node6};
int nodeCount = 6;

unsigned long last_time; //Used for timing routines
unsigned long send_time = 10000; //amount of time program sits collecting data before moving on
unsigned long wait_time = 20000; //maximum wait time for calibration routine

int status = WL_IDLE_STATUS;
WiFiClient client;

char ssid[] = "enCORE_OSU";      //  your network SSID (name)
char pass[] = "20solardec11";   // your network password

void setup()
{
  if(DEBUG) Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  xbee.setSerial(Serial1);
  

  if(DEBUG) Serial.println("Initializing & turning all heaters off");
  for(int i=0; i<nodeCount+2; i++) {
    pinMode(pHeaters[i], OUTPUT);
  }
  heatersOFF();

  
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
            }
        }
      }
    }
  } else {  
    nodes[0].stashConvertHub();
    
    float sumTemp = 0.0;
    for(int i=1; i<nodeCount; i++) {
      sumTemp += nodes[i].temp;
    }
    float referenceTemp = sumTemp/5;
    control1(referenceTemp);
    if(DEBUG) {
      Serial.print("Reference temperature: ");
      Serial.println(referenceTemp);
    }
    
    
    unsigned long start_send = millis();
    if(DEBUG) Serial.println("Sending data...");
    for(int i=0; i < nodeCount; i++) {
      if(DEBUG) nodes[i].printAllCompact();
//      connectWifi();
//      while (client.available()) {
//        char c = client.read();
//        Serial.write(c);
//      }
      
      int sendCheck = nodes[i].sendToDatabase(client);
      if(DEBUG) {
        if(sendCheck == 0) {
          Serial.print(nodes[i].num);
          Serial.println(" sent successfully");
        } else if(sendCheck == 1) {
          Serial.print(nodes[i].num);
          Serial.println(" did not send -> contains null data");
        } else if(sendCheck == 2) {
          Serial.print(nodes[i].num);
          Serial.println(" did not send -> could not connect");
        } else {
          Serial.print(nodes[i].num);
          Serial.println(" had unknown error");
        }
      }
      nodes[i].flush();
      sendCheck = 5;
      delay(tWaitSend);
    }

    if(DEBUG) {
      Serial.print("All data sent in ");
      Serial.print((millis()-start_send)/1000.);
      Serial.println(" seconds");
    }
    
    last_time = millis();
  }
}




void setEqual() {  
  if(DEBUG){
    Serial.println("Beginning calibaration process...");
  }
  
  int tripCount = 0;
  
  last_time = millis();
  
  while(tripCount != nodeCount && millis() - last_time <= wait_time) { //wait until all nodes are accounted for or times out
    tripCount = 0;
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {
      if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
        xbee.getResponse().getZBRxIoSampleResponse(response);
        for(int i=0; i < nodeCount; i++) {
          if(nodes[i].matchAddress(response)) {
            nodes[i].stashConvert(response);
            if(DEBUG) {
              Serial.print("Node ");
              Serial.print(i+2);
              Serial.println(" responded.");
            }
          }
          tripCount += nodes[i].trip;
        }
      }
    }
  }
  
  if(DEBUG) {
    Serial.print(tripCount);
    Serial.print(" of ");
    Serial.print(nodeCount);
    Serial.println(" nodes accounted for.");
  }
  
  if(tripCount == nodeCount) {
    hub.stashConvertHub();
  
    if(DEBUG) {
      Serial.println("Calibration completed.");
      Serial.println("Reference values (node, temperature adjustment, humidity adjustment)");
    }
  
    for(int i=0; i < nodeCount; i++) {
      nodes[i].tAdjust = hub.temp - nodes[i].temp;
      nodes[i].hAdjust = hub.hum - nodes[i].hum;
    
      if(DEBUG) {
        Serial.print("Node ");
        Serial.print(i+2);
        Serial.print(": ");
        Serial.print(nodes[i].tAdjust);
        Serial.print(", ");
        Serial.println(nodes[i].hAdjust);
      }
    }
  } else { //if not all nodes accounted for
    if(DEBUG) Serial.println("Calibration unsuccessful");
  }
}


void control1(float referenceTemp) { //control scheme using single reference temperature (hub temp)
  if(DEBUG)  Serial.println("Beginning deadband control checks with single reference...");
  
 // hub.stashConvertHub();
  
  if(nodes[0].trip) {
    if(referenceTemp > highTemp) {
      //actuate to lower all temps
      if(DEBUG) Serial.println("Temperatures need lowered.");
      
      heatersOFF();
      
    } else if(referenceTemp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Temperatures need raised.");
      
      heatersON();
      
    } else {
      //do nothing
      if(DEBUG) Serial.println("Temperatures are good.");
    }
  } else {
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
    } else if(hub.temp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Hub temperature needs raised.");
    } else {
      //do nothing
      if(DEBUG) Serial.println("Hub temperature is good.");
    }
  } else {
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
      } else if(nodes[i].temp < lowTemp) {
        //actuate to raise temp
        if(DEBUG) Serial.println(" temperature needs raised.");
      } else {
        //do nothing
        if(DEBUG) Serial.println(" temperature is good.");
      }
    } else {
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
