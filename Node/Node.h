#ifndef Node_h
#define Node_h

#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

class Node {
  public:
    Node(str msb_in, str lsb_in, str loc_in); //class constructor -> initializes variables
    ~Node(); //class destructor -> does nothing
    void stash(ZBRxIoSampleResponse packet); //stores packet data to class variables
    void flush(); //resets all class variables to 0
    float convertTemp(int temp); //converts analog value to temperature in degF
    float convertHum(int hum);
    byte convertMotion(byte pir, byte motion); //allows any motion within reporting window to be recongnized
    XBeeAdress64 addr; //node address placeholder
    float temp;
    float hum;
    int ldr1;
    int ldr2;
    byte motion;
  private:
    byte _pir; //temporary motion holder for raw data
};

#endif
    
