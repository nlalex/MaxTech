#ifndef Node_h
#define Node_h

#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

class Node {
  public:
    Node(str msb_in, str lsb_in, str loc_in);
    ~Node();
    void stash();
    void flush();
    float convertTemp(int temp);
    float convertHum(int hum);
    bool checkPIR(int PIR);
    str msb;
    str lsb;
    float temp;
    float hum;
    int ldr1;
    int ldr2;
    bool PIR;
  private:
};

#endif
    
