int pHeaters[] = {22, 23, 24, 25, 26, 27};
int numHeaters = 6;

const unsigned long tSleep = 5000;

void setup() {
  Serial.begin(9600);
  for(int i=0; i<numHeaters; i++) {
      pinMode(pHeaters[i], OUTPUT);
      Serial.print("Set ");
      Serial.println(i);
  }
}

void loop() {
  for(int i=0; i<numHeaters; i++) {
      digitalWrite(pHeaters[i], LOW);
      Serial.println("Off");
  }
  delay(tSleep);
  for(int i=0; i<numHeaters; i++) {
      digitalWrite(pHeaters[i], HIGH);
      Serial.println("On");
  }
  delay(tSleep);
}
