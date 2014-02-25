#include "Node.h"
//#include <WProgram.h>
//#include <XBee.h>
//#include "Config.h"

Node::Node(XBeeAddress64 addr_in, int num_in) {
  addr = addr_in;
  num = num_in;
  _temp = 0;
  _hum = 0;
  _ldr1 = 0;
  _ldr2 = 0;
  _pir = 0;
 // _motion = 0;
  if(num == HUB_NUM) {
    pinMode(pPIRh, INPUT);
  }
}

Node::~Node() {}

void Node::stash(ZBRxIoSampleResponse packet) {
  _temp = packet.getAnalog(pTEMP);
  _hum = packet.getAnalog(pHUM);
  _ldr1 = packet.getAnalog(pLDR1);
  _ldr2 = packet.getAnalog(pLDR2);
  _pir = packet.isDigitalOn(pPIR);
}

void Node::stashHub() {
  _temp = analogRead(pTEMPh);
  _hum = analogRead(pHUMh);
  _ldr1 = analogRead(pLDR1h);
  _ldr2 = analogRead(pLDR2h);
  _pir = digitalRead(pPIRh);
}

void Node::flush() {
  _temp = 0;
  _hum = 0;
  _ldr1 = 0;
  _ldr2 = 0;
  _pir = 0;
 // _motion = 0;
}

void Node::convertTemp() {
  int temp_analog = _temp;
  float voltage = temp_analog * 1.2;
  voltage /= 1024.0;  
  float temperatureC = (voltage - 0.5) * 100 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  _temp = temperatureF;
}

void Node::convertHum() {
  int hum_analog = _hum;
  float hum_voltage = hum_analog * 1.2/1024.0;
  hum_voltage *= 3.2; //constant defined by voltage divider circuit used
  _hum = (hum_voltage-0.958)/0.0370; //formula taken from datasheet
  // float supply_voltage = 5;
  // int hum_voltage = 1.2/1023 * _hum *4;
  // float raw_reading = (hum_voltage/supply_voltage -0.16)/0.0062;
  // float hum_reading = raw_reading/(1.0546-0.00216*_temp);
  // _hum = hum_reading;
}

//void Node::convertMotion() {
//  if(_pir == 0 && _motion == 0) {
//    return;
//  } else {
//    _motion = 1;
//  }
//}

boolean Node::matchAddress(ZBRxIoSampleResponse packet) {
  if(packet.getRemoteAddress64().getLsb()==addr.getLsb() && packet.getRemoteAddress64().getMsb()==addr.getMsb()) {
    return true;
  } else {
    return false;
  }
}

void Node::printAll() {
  Serial.print("Node #: ");
  Serial.println(num);

  Serial.print("Node Address: ");
  Serial.print(addr.getMsb(), HEX);
  Serial.println(addr.getLsb(), HEX);
  
  Serial.print("Temperature: ");
  Serial.println(_temp);
  
  Serial.print("Humidity: ");
  Serial.println(_hum);
  
  Serial.print("Light #1: ");
  Serial.println(_ldr1);
  
  Serial.print("Light #2: ");
  Serial.println(_ldr2);
  
  Serial.print("Motion: ");
  Serial.println(_pir);
  
  Serial.println("");
}

void Node::stashConvert(ZBRxIoSampleResponse packet) {
  stash(packet);
  convertTemp();
  convertHum();
}

void Node::stashConvertHub() {
  stashHub();
  convertTemp();
  convertHum();
}

void Node::testDatabaseSend() {
  Serial.print(num);
  Serial.print(',');
  Serial.print(_temp);
  Serial.print(',');
  Serial.print(_hum);
  Serial.print(',');
  Serial.print(_ldr1);
  Serial.print(',');
  Serial.print(_ldr2);
  Serial.print(',');
  Serial.println(_pir);
}


