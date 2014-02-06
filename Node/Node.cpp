#include "Node.h"

Node::Node(str address) {
  _address = address;
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
