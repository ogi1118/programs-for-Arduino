const int LED = 7;
int val = 0;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  val = analogRead(0);
  Serial.println(val);

  digitalWrite(LED, HIGH);
  delay(val);

  digitalWrite(LED, LOW);
  delay(val);
}
