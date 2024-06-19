#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <IRremote.h>

LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo;

const int sensorPin = 0;
const int receiverPin = 7;
const int servoPin = 9;
const int TRIG = 3;
const int ECHO = 2;
const int LED_GREEN = 10;
const int LED_RED = 11;

int pos = 0;
int posFlag = 0;
double duration = 0;
double distance = 0;
double speed_of_sound = 331.5 + 0.6 * 25; // 25℃の気温の想定

IRrecv receiver(receiverPin);
decode_results results;

void setup() {

  Serial.begin(9800);

  myservo.attach(servoPin);

  lcd.init(); 
  lcd.backlight();

  pinMode(ECHO, INPUT );
  pinMode(TRIG, OUTPUT );
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  receiver.enableIRIn();

}

void loop(){

 int reading = analogRead(sensorPin);  
 float voltage = reading * 5.0;
 voltage /= 1024.0; 

 float temperatureC = (voltage - 0.5) * 100 ;
 Serial.print(temperatureC); 
 Serial.println(" degrees C");

 digitalWrite(TRIG, LOW); 
 delayMicroseconds(2); 
 digitalWrite( TRIG, HIGH );
 delayMicroseconds( 10 ); 
 digitalWrite( TRIG, LOW );
 duration = pulseIn( ECHO, HIGH ); // 往復にかかった時間が返却される[マイクロ秒]

 if (duration > 0) {
  duration = duration / 2; // 往路にかかった時間
  distance = duration * speed_of_sound * 100 / 1000000;
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println(" cm");
 }

 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Temp:");
 lcd.print(temperatureC);
 lcd.print((char)223);
 lcd.print("C");
 lcd.setCursor(0, 1);
 lcd.print("Dist:");
 lcd.print(distance);
 lcd.print(" cm");

 if(receiver.decode(&results))  {

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);

  if(pos == 0) posFlag = 0;
  if(pos == 180) posFlag = 1;

  if(posFlag == 0) pos = pos + 30;
  else pos = pos - 30;
  myservo.write(pos);
  Serial.print("====="); Serial.print(pos); Serial.println(" Position=====");
  Serial.print("Message: "); Serial.println(results.value, HEX);

  receiver.resume();
 }

 else{
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
 }

 Serial.println("\n---------------\n");
 delay(1000); //waiting a second
}