#include "Node.h"
#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

Node::Node(str msb_in, str lsb_in, str loc_in) {
  addr = XBeeAdress64(msb_in, lsb_in);
  loc = loc_in;j
  temp = 0;
  hum = 0;
  ldr1 = 0;
  ldr2 = 0;
  PIR = false;
}

Node::~Node() {}

void Node::stash() {
  
}

void Node::flush() {
  temp = 0;
  hum = 0;
  ldr1 = 0;
  ldr2 = 0;
  PIR = false;
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

bool Node::checkPIR(int PIR) {
  return bool(PIR);
}
