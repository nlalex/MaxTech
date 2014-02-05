// Arduino01
// 02/03/2014

// Read digital & analog data from single remote node
// Utilizes Arduino Mega's multiple serial I/O pins

// Code modified from: Adafruit.com; Peter H Anderson; xbee-arduino library examples

#include <XBee.h>

XBee xbee = XBee();
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();
XBee Address64 test = XBeeAddress64();



void setup()
{
  Serial.begin(9600); //For communication to/from computer
  
  Serial.begin(9600); //For communication to/from XBee
  xbee.setSerial(Serial1);
}
 

void loop() {
  //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);

      nss.print("Received I/O Sample from: ");
      
      nss.print(ioSample.getRemoteAddress64().getMsb(), HEX);  
      nss.print(ioSample.getRemoteAddress64().getLsb(), HEX);  
      nss.println("");
      
      if (ioSample.containsAnalog()) {
        nss.println("Sample contains analog data");
      }

      if (ioSample.containsDigital()) {
        nss.println("Sample contains digtal data");
      }      

      // read analog inputs
      for (int i = 0; i <= 4; i++) {
        if (ioSample.isAnalogEnabled(i)) {
          nss.print("Analog (AI");
          nss.print(i, DEC);
          nss.print(") is ");
          nss.println(ioSample.getAnalog(i), DEC);
        }
      }

      // check digital inputs
      for (int i = 0; i <= 12; i++) {
        if (ioSample.isDigitalEnabled(i)) {
          nss.print("Digital (DI");
          nss.print(i, DEC);
          nss.print(") is ");
          nss.println(ioSample.isDigitalOn(i), DEC);
        }
      }
      
      // method for printing the entire frame data
      //for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
      //  nss.print("byte [");
      //  nss.print(i, DEC);
      //  nss.print("] is ");
      //  nss.println(xbee.getResponse().getFrameData()[i], HEX);
      //}
    } 
    else {
      nss.print("Expected I/O Sample, but got ");
      nss.print(xbee.getResponse().getApiId(), HEX);
    }    
  } else if (xbee.getResponse().isError()) {
    nss.print("Error reading packet.  Error code: ");  
    nss.println(xbee.getResponse().getErrorCode());
  }
}
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
