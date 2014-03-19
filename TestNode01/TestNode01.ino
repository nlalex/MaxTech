// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

const boolean DEBUG = true;

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

#include <Node.h>
Node hub = Node(HUB_ADDR, HUB_NUM);
Node node2 = Node(XBeeAddress64(0x0013A200,0x40ABB7F7), 2);
Node node3 = Node(XBeeAddress64(0x0013A200,0x40AEB88F), 3);
Node node4 = Node(XBeeAddress64(0x0013A200,0x40AEBA2C), 4);
Node node5 = Node(XBeeAddress64(0x0013A200,0x40AEB9AA), 5);
Node node6 = Node(XBeeAddress64(0x0013A200,0x40AEB9C3), 6);

Node nodes[] = {node2, node3, node4, node5, node6}; //Array containing previously defined Nodes
int nodeCount = 5; //Number of nodes excluding the hub

unsigned long last_time; //Used for timing routines

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  xbee.setSerial(Serial1);
  
  if(digitalRead(pCAL) == HIGH) {
    setEqual();
  }
  
  last_time = millis();
}
 

void loop() {
  if(millis()-last_time >= SEND_TIME) { //Timed loop functions
    
    last_time = millis();
  }

  //attempt to read a packet    
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(response);
      
      for(int i=0; i < nodeCount; i++) {
        if(nodes[i].matchAddress(response)) {
          nodes[i].stashConvert(response);
          nodes[i].printAll();
//          nodes[i].testDatabaseSend();
          nodes[i].flush();
        }
      }
    }
  }
}




void setEqual() {  
  if(DEBUG){
    Serial.println("Beginning calibaration process...");
  }
  
  int tripCount = 0;
  
  while(tripCount != nodeCount) {
    tripCount = 0;
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
    
    for(int i=0; i < nodeCount; i++) {
      if(nodes[i].trip == true) {
        tripCount++;
        if(DEBUG) {
          Serial.print(tripCount);
          Serial.print(" of ");
          Serial.print(nodeCount);
          Serial.println(" nodes accounted for.");
        }
      }
    }
  }
  
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
      Serial.print(i);
      Serial.print(": ");
      Serial.print(nodes[i].tAdjust);
      Serial.print(", ");
      Serial.println(nodes[i].hAdjust);
      Serial.println("");
    }
  }
}
