const int pTemp = 12; //define analog pin here

void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL); //only enable if using reference value other than 5V
}

void loop() {
  int temp_analog = analogRead(pTemp);
  float voltage = temp_analog * 5000.;
  voltage /= 1024.0;
  float temperatureC = (voltage - 500.0)/ 10.0 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  float temp = temperatureF;
  Serial.println(temp);
  delay(500);
}
