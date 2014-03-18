// TestNode01
// 02/22/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

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

Node nodes[] = {node2, node3, node4, node5, node6};
int nodeCount = 5;

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
    
    //hub.stashConvertHub();
    //hub.printAll();
    //hub.testDatabaseSend();
    //hub.flush();
    
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
          nodes[i].testDatabaseSend();
          nodes[i].flush();
        }
      }
      
//      if(node2.matchAddress(response)) {
//        node2.stashConvert(response);
//        node2.printAll();
//        //node2.testDatabaseSend();
//        node2.flush();
//      } else if(node3.matchAddress(response)) {
//        node3.stashConvert(response);
//        node3.printAll();
//        //node3.testDatabaseSend();
//        node3.flush();
//      } else if(node4.matchAddress(response)) {
//        node4.stashConvert(response);
//        node4.printAll();
//        //node4.testDatabaseSend();
//        node4.flush();
//      } else if(node5.matchAddress(response)) {
//        node5.stashConvert(response);
//        node5.printAll();
//        //node5.testDatabaseSend();
//        node5.flush();
//      } else if(node6.matchAddress(response)) {
//        node6.stashConvert(response);
//        node6.printAll();
//        //node6.testDatabaseSend();
//        node6.flush();
//      }
    }
  }
}

