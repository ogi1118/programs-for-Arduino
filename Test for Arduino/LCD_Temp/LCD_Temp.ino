#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorPin = 0;

void setup() {
  lcd.init(); 
  lcd.backlight();
  Serial.begin(9800);
}

void loop(){

 int reading = analogRead(sensorPin);  

 float voltage = reading * 5.0;
 voltage /= 1024.0; 

 Serial.print(voltage); Serial.println(" volts");

 float temperatureC = (voltage - 0.5) * 100 ;

 Serial.print(temperatureC); Serial.println(" degrees C");

 float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
 Serial.print(temperatureF); Serial.println(" degrees F");

 Serial.println("\n---------------\n");

 lcd.clear();
 lcd.setCursor(0, 0);       // start to print at the first row
 lcd.print(temperatureC);    // print the temperature in Celsius
 lcd.print((char)223);      // print ° character
 lcd.print("C");
 lcd.setCursor(0, 1);       // start to print at the second row
 lcd.print(temperatureF); // print the temperature in Fahrenheit
 lcd.print((char)223);      // print ° character
 lcd.print("F");

 delay(1000); //waiting a second
}
