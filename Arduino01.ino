// Arduino01
// 02/03/2014

// Code modified from: Adafruit.com; Peter H Anderson

#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial xbeeSerial = SoftwareSerial(rxPin, txPin);

// Pinout
int pTMP = A0; 
int pLDR1 = A1;
int pLDR2 = A2;
int pPIR = A5;
int pHUM = A6;


float vPIR = 2000;
float vTMP = 0;
float vLDR = 0;
float vHUM = 0;
int n = 0;

void setup()
{
  Serial.begin(9600); 
  xbeeSerial.begin(9600);
}
 
 
void loop() {
  if (xbeeSerial.available() >= 21){
    byte xbee[23];
    if (xbeeSerial.read() == 0x7E){  // starting byte 
      for (int i = 1; i < 25; i++){
        xbee[i-1] = xbeeSerial.read();
        delay(5); 
      }

      String slave_one = "171185222";
      String address;  // Each router's unique address
      for (int q = 8; q < 11; q = q + 1) {
           address = String(address + String(xbee[q]));
       }
      
      float motion = getMotion(xbee[18], xbee[19]);
      
      if(motion < vPIR) {
        vPIR = motion;
      }
    }
  }
  
  n += 1;
  float t = fTMP();
 // Serial.println(t);
  vTMP += t;
  vLDR += fLDR();
//  int iCO = fCO(iCO_last);
  int iCO = fCO();
  iCO_last = iCO;
  vHUM += hum();
  vTH += temp()*9.0/5.0 + 32.0;
  
  
  if(Serial.available() > 0) {
    Serial.read();
    
  //  Serial.print(n);
   // Serial.print(',');
    Serial.print(vTMP/n);
    Serial.print(',');
    Serial.print(vLDR/n);
    Serial.print(',');
    Serial.print(iCO);
    Serial.print(',');
    Serial.print(vPIR);
    Serial.print(',');
    Serial.print(vHUM/n);
    Serial.print(',');
    Serial.println(vTH/n);
    
    vPIR = 2000;
    vTMP = 0;
    vLDR = 0;
    vCO = 0;
    vHUM = 0;
    vTH = 0;
    n = 0;
  } 
  
  delay(500);
  
}



float fTMP() {
  // Returns temperature from TMP36 in degF
  // From left to right looking at flat side of sensor -> +5V, Analog Out, Gnd
  int reading = analogRead(pTMP); 
  float voltage = reading * 5.0;
  voltage /= 1024.0;  
  float temperatureC = (voltage - 0.5) * 100 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  return temperatureF;
}

int fLDR() {
  // Returns light sensor value from 0-1023
  int reading = analogRead(pLDR);
  return reading;
}

float getMotion(int MSB1, int LSB1){
  int motion_analog = MSB1 * 256 + LSB1;
  return motion_analog;
}
