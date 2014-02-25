// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

#include <Node.h>
Node hub = Node(HUB_ADDR, HUB_NUM);
Node node1 = Node(XBeeAddress64(0x0013A200,0x40ABBB6C), 1);
Node node2 = Node(XBeeAddress64(0,0), 2);

unsigned long last_time;

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  //Serial1.begin(9600); //For communication to/from XBee
  //xbeeSerial.begin(9600);
  //xbee.setSerial(xbeeSerial);
  xbee.setSerial(Serial1);
  
  last_time = millis();
}
 

void loop() {
  if(millis()-last_time >= SEND_TIME) {
    //node1.printAll();
    //node2.printAll();
    //hub.printAll();
    //hub.testDatabaseSend();
    
    node1.flush();
    node2.flush();
    hub.flush();
    
    last_time = millis();
  }

  //attempt to read a packet    
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(response);
      
      if(node1.matchAddress(response)) {
        node1.stashConvert(response);
        node1.printAll();
        node1.flush();
      } else if(node2.matchAddress(response)) {
        node2.stashConvert(response);
      }
    }
  hub.stashConvertHub();
  }
}

