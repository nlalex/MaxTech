#include "Node.h"
#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

Node::Node(str msb_in, str lsb_in, str loc_in) {
  msb = msb_in;
  lsb = lsb_in;
  loc = loc_in;
}

Node::~Node() {}

void Node::stash() {
  
}

void Node::flush() {

}

float Node::convertTemp(int temp) {
  return float(temp);
}

float Node::convertHum(int hum) {
  return float(hum);
}

bool Node::checkPIR(int PIR) {
  return bool(PIR);
}
