// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

//#include <SoftwareSerial.h>
//#define rxPin 10
//#define txPin 11
//SoftwareSerial xbeeSerial = SoftwareSerial(rxPin, txPin);

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

#include <Node.h>
Node node1 = Node(XBeeAddress64(0x0013A200,0x40ABBB6C), String("Right"));
Node node2 = Node(XBeeAddress64(0x435,0x4344), String("Left"));

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  //Serial1.begin(9600); //For communication to/from XBee
  //xbeeSerial.begin(9600);
  //xbee.setSerial(xbeeSerial);
  xbee.setSerial(Serial1);
}
 

void loop() {
  
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(response);
      
      if(node1.matchAddr(response)) {
        node1.stash(response);
        node1.convertTemp();
        node1.convertHum();
        node1.convertMotion();
      } else if(node2.matchAddr(response)) {
        node2.stash(response);
        node2.convertTemp();
        node2.convertHum();
        node2.convertMotion();
      }
    }
  node1.printAll();
  node2.printAll();
  
  node1.flush();
  node2.flush();
  }
  
  
}

