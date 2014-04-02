#ifndef Node_h
#define Node_h

#include <XBee.h>
#include <WProgram.h>
#include <WiFi.h>
#include "Config_James.h"

class Node {
  public:
    Node(XBeeAddress64 addr_in, int num_in); //class constructor -> initializes variables
    ~Node(); //class destructor -> does nothing
    void stash(ZBRxIoSampleResponse packet); //stores packet data to class variables
    void stashHub(); //gets all data for hub 'node'
    void flush(); //resets all class variables to 0
    void convertTemp(); //converts analog value to temperature in degF
    void convertTempHub();
    void convertHum();
    void convertHumHub();
    boolean matchAddress(ZBRxIoSampleResponse packet); //checks for address match
    void printAll(); //prints all variables to console via Serial for testing purposes
    void printAllCompact();
    void stashConvert(ZBRxIoSampleResponse packet); //saves & converts data
    void stashConvertHub(); //same as above but for hub
    void testDatabaseSend(); //formats data as .csv and sends to python script via Serial
    int sendToDatabase(WiFiClient client);
	void actuatedON();
	void actuatedOFF();
    XBeeAddress64 addr; //node address placeholder
    int num; //node number for referencing purposes
    float tAdjust; //temperature adjustment factor
    float hAdjust; //humidity adjustment factor
    float temp;
    float hum;
    int actuated; //0 or 1 placeholder for energy tracking
    boolean trip; //whether or not node has been activated
    int _ldr1;
    int _ldr2;
    int _pir; //temporary motion holder for raw data
	int ct;
};

#endif
    
