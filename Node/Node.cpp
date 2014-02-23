#include "Node.h"
//#include <WProgram.h>
//#include <XBee.h>
//#include "Config.h"

Node::Node(str msb_in, str lsb_in, str loc_in) {
  addr = XBeeAdress64(msb_in, lsb_in);
  loc = loc_in;
  temp = 0;
  hum = 0;
  ldr1 = 0;
  ldr2 = 0;
  _pir = 0;
  motion = 0;
}

Node::~Node() {}

void Node::stash(ZBRxIoSampleResponse packet) {
  temp = packet.getAnalog(pTEMP);
  hum = packet.getAnalog(pHUM);
  ldr1 = packet.getAnalog(pLDR1);
  ldr2 = packet.getAnalog(pLDR2);
  _pir = packet.getDigital(pPIR);
}

void Node::flush() {
  temp = 0;
  hum = 0;
  ldr1 = 0;
  ldr2 = 0;
  _pir = 0;
  motion = 0;
}

float Node::convertTemp(int temp) {
  float voltage = temp * 5.0;
  voltage /= 1024.0;  
  float temperatureC = (voltage - 0.5) * 100 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  return temperatureF;
}

float Node::convertHum(int hum) {
  return float(hum);
}

byte Node::convertMotion(byte pir, byte motion) {
  if(byte pir == 0 && motion == 0) {
    return 0;
  } else {
    return 1;
  }
}

boolean Node::matchAddr(ZBRxIoSampleResponse packet) {
  if(packet.getRemoteAddress64().getLsb()==addr.getLsb() && packet.getRemoteAddress64().getMsb()==addr.getMsb()) {
    return true;
  } else {
    return false;
  }
}

void Node::printAll() {
  Serial.print("Node Address: ");
  Serial.print(addr.getMsb());
  Serial.println(addr.getLsb());
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  
  Serial.print("Humidity: ");
  Serial.println(hum);
  
  Serial.print("Light #1: ");
  Serial.println(ldr1);
  
  Serial.print("Light #2: ");
  Serial.println(ldr2);
  
  Serial.print("Motion: ");
  Serial.println(motion);
  
  Serial.println("");
}
