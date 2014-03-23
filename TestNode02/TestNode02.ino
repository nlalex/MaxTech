// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true;

const float lowTemp = 69.0;
const float highTemp = 72.0;

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

#include <Node.h>
Node hub = Node(HUB_ADDR, HUB_NUM); //hub addr: 40ABB77F
Node node2 = Node(XBeeAddress64(0x0013A200,0x40ABB9A8), 2); //black
Node node3 = Node(XBeeAddress64(0x0013A200,0x40ABB9DE), 3); //yellow
Node node4 = Node(XBeeAddress64(0x0013A200,0x40AD57DA), 4); //white
Node node5 = Node(XBeeAddress64(0x0013A200,0x40ABAE96), 5); //red
Node node6 = Node(XBeeAddress64(0x0013A200,0x40ABBB6C), 6); //blue 

Node nodes[] = {node2, node3, node4, node5, node6}; //Array containing previously defined Nodes
int nodeCount = 5; //Number of nodes excluding the hub

const int pHeaters[] = {22, 23, 24, 25, 26, 27};
const int heaterCount = 6;

unsigned long last_time; //Used for timing routines
unsigned long send_time = 10000; //amount of time program sits collecting data before moving on
unsigned long wait_time = 20000; //maximum wait time for calibration routine

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  xbee.setSerial(Serial1);
  
  if(digitalRead(pCAL) == HIGH) {
    setEqual();
  }
  
  last_time = millis();
  
  for(int i=0; i<heaterCount; i++) {
    pinMode(pHeaters[i], OUTPUT);
  }
}
 

void loop() {
  if(millis()-last_time <= send_time) { //Timed loop functions
    //attempt to read a packet    
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {
      // got something
      if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
        xbee.getResponse().getZBRxIoSampleResponse(response);
      
        for(int i=0; i < nodeCount; i++) {
          if(nodes[i].matchAddress(response)) {
            nodes[i].stashConvert(response);
//            Serial.print("Stored data ");
//            Serial.println(i);
//            nodes[i].printAll();
//            nodes[i].flush();
//            nodes[i].printAll();
//            Serial.println(nodes[i].temp);
//            Serial.println(nodes[i].tAdjust);
//            nodes[i].flush();
            
            //nodes[i].flush();
          }
        }
      }
    }
    //last_time = millis();
  } else {
  
    hub.stashConvertHub();
    hub.printAll();
    hub.flush();
    
    for(int i=0; i < nodeCount; i++) {
      nodes[i].printAll();
      nodes[i].flush();
       // Serial.print(nodes[i].trip);
    }
//    Serial.println("");
    
    control1();
    
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


void control1() { //control scheme using single reference temperature (hub temp)
  if(DEBUG)  Serial.println("Beginning deadband control checks with single reference...");
  
  hub.stashConvertHub();
  
  if(hub.trip) {
    if(hub.temp > highTemp) {
      //actuate to lower all temps
      if(DEBUG) Serial.println("Temperatures need lowered.");
      for(int i=0; i < heaterCount; i++) {
        digitalWrite(pHeaters[i], HIGH); //high turns heaters on
      }
    } else if(hub.temp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Temperatures need raised.");
      for(int i=0; i < heaterCount; i++) {
        digitalWrite(pHeaters[i], LOW); //low turns heaters on
      }
    } else {
      //do nothing
      if(DEBUG) Serial.println("Temperatures are good.");
    }
  } else {
    //data not received from hub
    if(DEBUG) Serial.println("Hub contains null data.");
  }
  
  if(DEBUG) {
    Serial.println("Control checks completed.");
    Serial.println("");
  }
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
