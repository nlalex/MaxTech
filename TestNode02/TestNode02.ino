// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true;

const float lowTemp = 69.0;
const float highTemp = 72.0;

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

//#include <Config_enCORE.h>
#include <Config_James.h>
#include <Node.h>
Node hub = Node(HUB_ADDR, HUB_NUM); //hub addr: 40ABB77F
Node node2 = Node(addr2, 2); //black
Node node3 = Node(addr3, 3); //yellow
Node node4 = Node(addr4, 4); //white
Node node5 = Node(addr5, 5); //red
Node node6 = Node(addr6, 6); //blue 
Node nodes[] = {node2, node3, node4, node5, node6}; //Array containing previously defined Nodes
Node allNodes[] = {hub, node2, node3, node4, node5, node6};
int nodeCount = 5; //Number of nodes excluding the hub
int allNodeCount = nodeCount + 1;

unsigned long last_time; //Used for timing routines
unsigned long send_time = 5000; //amount of time program sits collecting data before moving on
unsigned long wait_time = 20000; //maximum wait time for calibration routine

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  xbee.setSerial(Serial1);
  
  if(CONFIG == 0) {
    if(DEBUG) Serial.println("Initializing & turning all heaters off");
    for(int i=0; i<allNodeCount; i++) {
      pinMode(pHeaters[i], OUTPUT);
    }
    heatersOFF();
  } else if(CONFIG == 1) {
    if(DEBUG) Serial.println("Initializing & holding all vent positions");
    for(int i=0; i<allNodeCount; i++) {
      pinMode(pVentHigh[i], OUTPUT);
      pinMode(pVentLow[i], OUTPUT);
    }
    ventsHold();
  }
  
  if(digitalRead(pCAL) == HIGH) {
    setEqual();
  }
  
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
  
    hub.stashConvertHub();
    
//    control1();
    
    hub.printAll();
    hub.flush();
    
    for(int i=0; i < nodeCount; i++) {
      if(nodes[i].trip) nodes[i].printAll();
      nodes[i].flush();
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


void control1() { //control scheme using single reference temperature (hub temp)
  if(DEBUG)  Serial.println("Beginning deadband control checks with single reference...");
  
  hub.stashConvertHub();
  
  if(hub.trip) {
    if(hub.temp > highTemp) {
      //actuate to lower all temps
      if(DEBUG) Serial.println("Temperatures need lowered.");
      
      if(CONFIG == 0) { //enCORE setup
        heatersOFF():
      } else if (CONFIG == 1) { //James's setup
        ventsClose();
      }
      
    } else if(hub.temp < lowTemp) {
      //actuate to raise temp
      if(DEBUG) Serial.println("Temperatures need raised.");
      
      if(CONFIG == 0) { //enCORE setup
        heatersON();
      } else if (CONFIG == 1) { //James's setup
        ventsClose();
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

void heatersOFF() {
  for(int i=0; i < allNodeCount; i++) {
    digitalWrite(pHeaters[i], HIGH); //high turns heaters off
  }
}

void heatersON() {
  for(int i=0; i < allNodeCount; i++) {
    digitalWrite(pHeaters[i], LOW); //low turns heaters on
  }
}

void ventsHold() {

}

void ventsOpen() {

}

void ventsClose() {

}
