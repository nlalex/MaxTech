#ifndef Node_h
#define Node_h

#include <WProgram.h>
#include <XBee.h>
#include "Config.h"

class Node {
  public:
    Node(XBeeAddress64 addr_in, int num_in); //class constructor -> initializes variables
    ~Node(); //class destructor -> does nothing
    void stash(ZBRxIoSampleResponse packet); //stores packet data to class variables
    void stashHub(); //gets all data for hub 'node'
    void flush(); //resets all class variables to 0
    void convertTemp(); //converts analog value to temperature in degF
    void convertHum();
//    void convertMotion(); //allows any motion within reporting window to be recongnized
    boolean matchAddress(ZBRxIoSampleResponse packet); //checks for address match
    void printAll(); //prints all variables to console via Serial for testing purposes
    void stashConvert(); //saves & converts data
    void stashConvertHub(); //same as above but for hub
    void testDatabaseSend(); //formats data as .csv and sends to python script via Serial
    XBeeAddress64 addr; //node address placeholder
    int num; //node number for referencing purposes
  private:
    float _temp;
    float _hum;
    int _ldr1;
    int _ldr2;
    int _pir; //temporary motion holder for raw data
    //int _motion;
};

#endif
    
