const int LED = 9;
int i = 0;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);  
}

void loop() {

  for(i = 0;i < 255;i++){
    analogWrite(LED, i);
    Serial.println(i);
    delay(10);
  }

  for(i = 255;i > 0;i--){
    analogWrite(LED, i);
    Serial.println(i);
    delay(10);
  }

}
