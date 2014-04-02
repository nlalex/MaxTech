int pHeaters[] = {22, 23, 24, 25, 26, 27}; //heater control pins
int numHeaters = 6; //number of entries in above array

const unsigned long tSleep = 5000; //amount of time between switching

void setup() {
  Serial.begin(9600);
  for(int i=0; i<numHeaters; i++) { //set all pins as output
      pinMode(pHeaters[i], OUTPUT);
      Serial.print("Set ");
      Serial.println(i);
  }
}

void loop() {
  for(int i=0; i<numHeaters; i++) { //set all pins low
      digitalWrite(pHeaters[i], LOW);
      Serial.println("Off");
  }
  
  delay(tSleep);
  
  for(int i=0; i<numHeaters; i++) { //set all pins high
      digitalWrite(pHeaters[i], HIGH);
      Serial.println("On");
  }
  
  delay(tSleep);
}
