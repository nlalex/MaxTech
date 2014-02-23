#ifndef Node_h
#define Node_h

#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

class Node {
  public:
    Node(str msb_in, str lsb_in, str loc_in);
    ~Node();
    void stash(ZBRxIoSampleResponse packet);
    void flush();
    float convertTemp(int temp);
    float convertHum(int hum);
    boolean checkPIR(int PIR);
    XBeeAdress64 addr;
    float temp;
    float hum;
    int ldr1;
    int ldr2;
    byte motion;
  private:
    int _pir; //temporary motion holder for raw data
};

#endif
    
