#include "Node.h"
#include <WProgram.h>
// #include <XBee.h>
// #include <WiFi.h>
// #include "Config_enCORE.h"

Node::Node(XBeeAddress64 addr_in, int num_in) {
  addr = addr_in;
  num = num_in;
  temp = 0;
  hum = 0;
  _ldr1 = 0;
  _ldr2 = 0;
  _pir = 0;
  tAdjust = 0;
  hAdjust = 0;
  trip = false;
 // _motion = 0;
  if(num == HUB_NUM) {
    pinMode(pPIRh, INPUT);
  }
  actuated = 0;
  ct = 0;
}

Node::~Node() {}

void Node::stash(ZBRxIoSampleResponse packet) {
  temp = packet.getAnalog(pTEMP);
  hum = packet.getAnalog(pHUM);
  _ldr1 = packet.getAnalog(pLDR1);
  _ldr2 = packet.getAnalog(pLDR2);
  switch (packet.isDigitalOn(pPIR)) {
    case 0:
      _pir = 1;
      break;
    case 1:
      _pir = 0;
      break;
  }
  trip = true;

}

void Node::stashHub() {
	Serial.println("Reading hub");
  _ldr1 = analogRead(pLDR1h);
  delay(10);
  _ldr1 = analogRead(pLDR1h);
  delay(10);
  _ldr2 = analogRead(pLDR2h);
  delay(10);
  _ldr2 = analogRead(pLDR2h);
  delay(10);
  temp = analogRead(pTEMPh);
  delay(10);
  temp = analogRead(pTEMPh);
  delay(10);
  hum = analogRead(pHUMh);
  delay(10);
  hum = analogRead(pHUMh);
  delay(10);
  switch (digitalRead(pPIRh)) {
    case 0:
      _pir = 1;
      break;
    case 1:
      _pir = 0;
      break;
  }
  trip = true;
  if(analogRead(pCT)>threshCT) {
	ct = 1;
   }else {
	ct = 0;
	}
}

void Node::flush() {
  temp = 0;
  hum = 0;
  _ldr1 = 0;
  _ldr2 = 0;
  _pir = 0;
 // _motion = 0;
 trip = false;
}

void Node::convertTemp() {
  int temp_analog = temp;
  float voltage = temp_analog * 1.2;
  voltage /= 1024.0;  
  float temperatureC = (voltage - 0.5) * 100 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  temp = temperatureF + tAdjust;
}

void Node::convertTempHub() {
  int temp_analog = temp;
  float voltage = float(temp_analog) * 5.0;
  voltage /= 1024.0;  
  float temperatureC = (voltage - 0.5) * 100.0 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  temp = temperatureF;
}

void Node::convertHum() {
  // int hum_analog = hum;
  // float hum_voltage = hum_analog * 1.2/1024.0;
  // hum_voltage *= 3.2; //constant defined by voltage divider circuit used
  // hum = (hum_voltage-0.958)/0.0370; //formula taken from datasheet
  float supply_voltage = 5.;
  float hum_voltage = 1.2/1023. * hum *4.;
  float raw_reading = (hum_voltage/supply_voltage -0.16)/0.0062;
  float hum_reading = raw_reading/(1.0546-0.00216*((temp-32.)*5./9.));
  hum = hum_reading + hAdjust;
}

void Node::convertHumHub() {
  // int hum_analog = hum;
  // float hum_voltage = hum_analog * 1.2/1024.0;
  // hum_voltage *= 3.2; //constant defined by voltage divider circuit used
  // hum = (hum_voltage-0.958)/0.0370; //formula taken from datasheet
  float supply_voltage = 5.;
  float hum_voltage = 5.0/1023. * hum *4.;
  float raw_reading = (hum_voltage/supply_voltage -0.16)/0.0062;
  float hum_reading = raw_reading/(1.0546-0.00216*((temp-32.)*5./9.));
  hum = hum_reading + hAdjust;
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
  Serial.println(temp);
  
  Serial.print("Humidity: ");
  Serial.println(hum);
  
  Serial.print("Light #1: ");
  Serial.println(_ldr1);
  
  Serial.print("Light #2: ");
  Serial.println(_ldr2);
  
  Serial.print("Motion: ");
  Serial.println(_pir);
  
  Serial.print("Actuated: ");
  Serial.println(actuated);
  
  Serial.println("");
}

void Node::printAllCompact() {
  Serial.print(num);
  Serial.print(" : ");
  Serial.print(trip);
  Serial.print(" : ");
  Serial.print(temp);
  Serial.print(" : ");
  Serial.print(hum);
  Serial.print(" : ");
  Serial.print(_ldr1);
  Serial.print(" : ");
  Serial.print(_ldr2);
  Serial.print(" : ");
  Serial.print(_pir);
  Serial.print(" : ");
  Serial.print(actuated);
  Serial.print(" : ");
  Serial.println(ct);
}

void Node::stashConvert(ZBRxIoSampleResponse packet) {
  stash(packet);
  convertTemp();
  convertHum();
}

void Node::stashConvertHub() {
  stashHub();
  convertTempHub();
  convertHumHub();
}

void Node::testDatabaseSend() {
  Serial.print(num);
  Serial.print(',');
  Serial.print(temp);
  Serial.print(',');
  Serial.print(hum);
  Serial.print(',');
  Serial.print(_ldr1);
  Serial.print(',');
  Serial.print(_ldr2);
  Serial.print(',');
  Serial.println(_pir);
}

int Node::sendToDatabase(WiFiClient client) {
	client.flush();
	client.stop();
	if(client.connect(server, 80)) {
	//client.connect(server,80);
	  //if(trip) {
		//long tStart = millis();
		// while(!client.connected()) { // && millis()-tStart<tSendTimeout
			// client.stop();
			// client.flush();
			// client.connect(server, 80);
			// delay(100);
		// }
		// client.print("GET /hook1.php?node=");
		// client.print(num);
		// client.print("&temp=");
		// client.print(temp);
		// client.print("&humidity=");
		// client.print(hum);
		// client.print("&light1=");
		// client.print(_ldr1);
		// client.print("&light2=");
		// client.print(_ldr2);
		// client.print("&motion=");
		// client.print(_pir);
		// client.print("&heat=");
		// client.print(actuated);
		// client.println(" HTTP/1.1");
		client.println("GET /hook1.php?node=1&temp=70&humidity=20.5&light1=500&light2=400&motion=1&heat=1 HTTP/1.1");
		client.println("Host: mesh.org.ohio-state.edu");
		client.println("User-Agent: ArduinoWiFi/1.1");
		client.println("Connection: close");
		client.println();
		
		delay(tWaitSend);
		// client.stop();
		return 0;
	   //} else {
			//client.stop();
		//	return 1;
		//}
	} else {
		client.stop();
		return 2;
	}
}

void Node::actuatedON() {
	actuated = 1;
}

void Node::actuatedOFF() {
	actuated = 0;
}


