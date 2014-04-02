void setup() {
  Serial.begin(9600);
  //analogReference(EXTERNAL);
}

void loop() {
  int temp_analog = analogRead(15);
  float voltage = temp_analog * 5000.;
  voltage /= 1024.0;  
  float temperatureC = (voltage - 500.0)/ 10.0 ;
  float temperatureF = ((temperatureC * 9.0) / 5.0) + 32.0;
  float temp = temperatureF;
  Serial.println(temp);
  delay(500);
}
