#include <SPI.h>
#include <WiFi.h>
#include <XBee.h>
#include <Node.h>

const int nodeCount = 6;

void setup() {
  Serial.begin(9600);
  Serial.println("Setting pinModes...");
  for(int i=0; i<nodeCount; i++) {
    pinMode(pVentPos[i], OUTPUT);
    pinMode(pVentNeg[i], OUTPUT);
    pinMode(pVentEnable[i], OUTPUT);
  }
  Serial.println("Holding vents...");
  //ventsHold();
}

void loop() {
//  int i = 4;
  ventOpen(0);
  delay(100);
  ventOpen(1);
  delay(100);
  ventOpen(2);
  delay(100);
  ventOpen(3);
  delay(100);
  ventOpen(4);
  delay(100);
  ventOpen(5);
//  delay(100);
//  digitalWrite(22, HIGH);
//  digitalWrite(23, LOW);
//  delay(100);
//  digitalWrite(38, HIGH);
//  delay(500);
//  digitalWrite(38, LOW);
//  
//  digitalWrite(22, LOW);
//  delay(500);
//  ventOpen(1);
//  delay(500);
//  ventOpen(2);
//  delay(500);
//  ventsOpen();
  Serial.println("Vents open");
  delay(2000);
  
  ventClose(0);
  delay(100);
  ventClose(1);
  delay(100);
  ventClose(2);
  delay(100);
  ventClose(3);
  delay(100);
  ventClose(4);
  delay(100);
  ventClose(5);
//  delay(100);
 
//  ventClose(1);
//  delay(500);
//  ventClose(2);
//  delay(500);
//  ventsClose();
  Serial.println("Vents closed");
  delay(2000);
}


void ventOpen(int p) {
  digitalWrite(pVentEnable[p], HIGH);
  delay(500);
  digitalWrite(pVentPos[p], HIGH);
  digitalWrite(pVentNeg[p], LOW);
  delay(tVentWait);
  digitalWrite(pVentPos[p], LOW);
  delay(500);
  digitalWrite(pVentEnable[p], LOW);
}

void ventClose(int p) {
  digitalWrite(pVentEnable[p], HIGH);
  delay(500);
  digitalWrite(pVentPos[p], LOW);
  digitalWrite(pVentNeg[p], HIGH);
  delay(tVentWait);
  digitalWrite(pVentNeg[p], LOW);
  delay(500);
  digitalWrite(pVentEnable[p], LOW);
}
  
void ventsHold() {
  for(int i=0; i<nodeCount; i++) {
    digitalWrite(pVentEnable[i], HIGH);
    digitalWrite(pVentPos[i], LOW);
    digitalWrite(pVentNeg[i], LOW);
    digitalWrite(pVentEnable[i], LOW);
  }
}

void ventsClose() {
  for(int i=0; i<2; i++) {
    digitalWrite(pVentPos[i], LOW);
    digitalWrite(pVentNeg[i], HIGH);
    delay(100);
    digitalWrite(pVentNeg[i], LOW);
    //ventClose(i);
    delay(500);
    //ventsHold();
  }
}

void ventsOpen() {
  for(int i=0; i<2; i++) {
    digitalWrite(pVentPos[i], HIGH);
    digitalWrite(pVentNeg[i], LOW);
    delay(100);
    digitalWrite(pVentPos[i], LOW);
    //ventOpen(i);
    delay(500);
    //ventsHold();
  }
}
