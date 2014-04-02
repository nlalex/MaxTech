const int p = 22;
const unsigned long wait1 = 120000;
const unsigned long wait2 = 300000;

boolean check = true;

void setup() {
  pinMode(p, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  while(check == true) {
    digitalWrite(p, HIGH);
    digitalWrite(13, HIGH);
    delay(wait1);
    digitalWrite(p, LOW);
    digitalWrite(13, LOW);
    delay(wait2);
    digitalWrite(p, HIGH);
    digitalWrite(13, HIGH);
    check = false;
  }
}
