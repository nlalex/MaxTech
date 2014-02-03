// Arduino01
// 02/03/2014

// Code modified from: Adafruit.com; Peter H Anderson

#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial xbeeSerial = SoftwareSerial(rxPin, txPin);

// Needed for humidity sensor
#include <Wire.h> //I2C library
float hum();
float temp();
#define TRUE 1
#define FALSE 0

int pTMP = A0; 
int pLDR = A1;
int pPIR = A5;
int pCOT = 12; // CO trigger
int pCOS = A2; // CO sensor

long tCO = 0;
long tCO_wait = 60000;

int iCO_last = 0;

float vPIR = 2000;
float vTMP = 0;
float vLDR = 0;
float vCO = 0;
float vHUM = 0;
float vTH = 0;
int n = 0;

void setup()
{
  Serial.begin(9600); 
  xbeeSerial.begin(9600);
  pinMode(pCOT, OUTPUT); // For CO sensor
  
  // Following needed for humidity sensor
  Wire.begin();
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH); // this turns on the HIH3610
  delay(5000);
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

//int fPIR() {
//  // Returns PIR value from 0-1023
//  int reading = analogRead(pPIR);
//  return reading;
//}

//int fCO(int rLast) {
//  // Turns CO sensor heating element on or off; takes reading if time allottment has passed
//  // Returns stagnant value if not new
//  int reading;
//  if(millis() - tCO < tCO_wait) {
//    analogWrite(pCOT, 255); // Needs to be at 5V for 60s
//    reading = rLast;
//  } else {
//    analogWrite(pCOT, 67); // Low voltage 1.4V
//    delay(1000);
//    reading = analogRead(pCOS); 
//    analogWrite(pCOT,255);
//    tCO = millis();
//  }
//  return reading;
//}

int fCO() {
  return analogRead(pCOS);
}

float hum() { 
  byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
  unsigned int H_dat, T_dat;
  float RH;
  
  address = 0x27;
  Wire.beginTransmission(address); 
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom((int)address, (int) 4);
  Hum_H = Wire.read();
  Hum_L = Wire.read();
  Temp_H = Wire.read();
  Temp_L = Wire.read();
  
  Wire.endTransmission();
  
  _status = (Hum_H >> 6) & 0x03;
  
  Hum_H = Hum_H & 0x3f;
  H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
  T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
  T_dat = T_dat / 4;
  
  RH = H_dat * 6.10e-3;   
  
  return(RH);
}

float temp() { 
  byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
  unsigned int H_dat, T_dat;
  float T_C;
  
  address = 0x27;
  Wire.beginTransmission(address); 
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom((int)address, (int) 4);
  Hum_H = Wire.read();
  Hum_L = Wire.read();
  Temp_H = Wire.read();
  Temp_L = Wire.read();
  
  Wire.endTransmission();
  
  _status = (Hum_H >> 6) & 0x03;
  
  Hum_H = Hum_H & 0x3f;
  H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
  T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
  T_dat = T_dat / 4;
  
  T_C = T_dat * 1.007e-2 - 40.0;   
  
  return(T_C);
}  

float getMotion(int MSB1, int LSB1){
  int motion_analog = MSB1 * 256 + LSB1;
  return motion_analog;
}
