#ifndef Node_h
#define Node_h

#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

class Node {
  public:
    Node(str address);
    ~Node();
    void stash();
    void flush();
    float convertTemp(int temp);
    float convertHum(int hum);
  private:
    str _address;
    float temp[];
    float hum[];
    int light1[];
    int light2[];
};

#endif
    
