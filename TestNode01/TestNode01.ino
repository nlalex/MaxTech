// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse response = ZBRxIoSampleResponse();

#include <Node.h>
Node hub = Node(XBeeAddress64(0,0), 0);
Node node1 = Node(XBeeAddress64(0x0013A200,0x40ABBB6C), 1);
Node node2 = Node(XBeeAddress64(0x,0x), 2);

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  Serial1.begin(9600); //For communication to/from XBee with Mega
  //Serial1.begin(9600); //For communication to/from XBee
  //xbeeSerial.begin(9600);
  //xbee.setSerial(xbeeSerial);
  xbee.setSerial(Serial1);
  
  unsigned long time_elapsed = millis();
}
 

void loop() {
  if(time_elapsed >= SEND_TIME) {
    node1.printAll();
    node2.printAll();
    hub.printAll();
    
    node1.flush();
    node2.flush();
    hub.flush();
    
    time_elapsed = 0;
  }

  //attempt to read a packet    
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(response);
      
      if(node1.matchAddress(response)) {
        node1.stashConvert(response);
      } else if(node2.matchAddress(response)) {
        node2.stashConvert(response);
      }
    }
  hub.stashConvertHub();
  }
}

