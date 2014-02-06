// TestXbee02
// 02/05/2014

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

#include <SoftwareSerial.h>
#define rxPin 10
#define txPin 11
SoftwareSerial xbeeSerial = SoftwareSerial(rxPin, txPin);

#include <XBee.h>
XBee xbee = XBee();
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();
XBeeAddress64 test = XBeeAddress64();

//Setup node validation
#define node1_addr 100
#define node2_addr 100

void setup()
{
  Serial.begin(9600); //For communication to/from computer
 
  //Serial1.begin(9600); //For communication to/from XBee
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
}
 

void loop() {
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      Serial.print("Received I/O Sample from: ");
      
      Serial.print(ioSample.getRemoteAddress64().getMsb(), HEX);  
      Serial.print(ioSample.getRemoteAddress64().getLsb(), HEX);  
      Serial.println("");
      
      if (ioSample.containsAnalog()) {
        Serial.println("Sample contains analog data");
      }

      if (ioSample.containsDigital()) {
        Serial.println("Sample contains digtal data");
      }      

      // read analog inputs
      for (int i = 0; i <= 4; i++) {
        if (ioSample.isAnalogEnabled(i)) {
          Serial.print("Analog (AI");
          Serial.print(i, DEC);
          Serial.print(") is ");
          Serial.println(ioSample.getAnalog(i), DEC);
        }
      }

      // check digital inputs
      for (int i = 0; i <= 12; i++) {
        if (ioSample.isDigitalEnabled(i)) {
          Serial.print("Digital (DI");
          Serial.print(i, DEC);
          Serial.print(") is ");
          Serial.println(ioSample.isDigitalOn(i), DEC);
        }
      }
      
      // method for printing the entire frame data
      //for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
      //  Serial.print("byte [");
      //  Serial.print(i, DEC);
      //  Serial.print("] is ");
      //  Serial.println(xbee.getResponse().getFrameData()[i], HEX);
      //}
    } 
    else {
      Serial.print("Expected I/O Sample, but got ");
      Serial.print(xbee.getResponse().getApiId(), HEX);
    }    
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  }
}


/*
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
}*/
