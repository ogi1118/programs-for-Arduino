#include "DHT.h"
#include <WiFi.h>
#include <Servo.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define DHTTYPE DHT11
#define DHT_PIN 2
#define FAN_PIN 4
#define BLUE_PIN 11
#define GREEN_PIN 12
#define RED_PIN 13
#define FLAME_SENSOR_PIN A0
#define OPEN 90
#define CLOSE 0
#define SENSOR1_PIN 7
#define SENSOR2_PIN 3
#define trigPin1 3 // Pin Trigger cho cảm biến 1
#define echoPin1 5 // Pin Echo cho cảm biến 1
#define trigPin2 7 // Pin Trigger cho cảm biến 2
#define echoPin2 6 // Pin Echo cho cảm biến 2


DHT dht(DHT_PIN, DHTTYPE);

#define API_KEY "AIzaSyDWPoNkYHQ63UPXhjOkL1lqGcevg2by4q4"
#define DATABASE_URL "https://smarthome-7a84a-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "team3@gmail.com"
#define USER_PASS "namnam"
#define WIFI_SSID "GPBL2024"
#define WIFI_PASS "gpbl@2024"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
float temp;
float humid;
bool lightControl;
bool fanControl;
bool doorControl;
int R;
int G;
int B;
int flameValue;
bool flameDetected;
unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 10000;
Servo myservo;
int sensor1_val;
int sensor2_val;
int count = 0;

static void WiFi_setup(){
  Serial.print("connecting");
  static const int local_port = 7000;
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.print(WiFi.localIP());
  Serial.println();

}

static void Serial_setup(){
  Serial.begin(115200);
}

void Firebase_setup(){
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASS;
  if(Firebase.signUp(&config, &auth, "", "")){
    signupOK = true;
  }else{
    Serial.print("error\n");
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setup() {
  // connect to WiFi
  WiFi_setup();
  Serial_setup();
  Firebase_setup();
  dht.begin();
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  // myservo.attach(10);
  //  pinMode(trigPin1, OUTPUT);
  // pinMode(echoPin1, INPUT);
  // pinMode(trigPin2, OUTPUT);
  // pinMode(echoPin2, INPUT);


}

void getData(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Serial.println(h);
    Serial.println(t);
    Serial.println("");
    return ;
  }

  humid = h;
  temp = t;
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.println("");
  return ;
}

void getFlameData() {
  if (Firebase.RTDB.getBool(&fbdo, "/flame_sensor/status")) { // Get flame sensor status from Firebase
    flameValue = analogRead(FLAME_SENSOR_PIN); // Read data from flame sensor
    flameDetected = flameValue > 800; // Check if flame is detected
    Serial.println(flameDetected ? "Flame detected!" : "No flame detected."); // Display message based on detection result
  }
}

void sendData(){
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 ||sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  }
  if(Firebase.RTDB.setFloat(&fbdo, "sensors/temperature", temp)){
    Serial.println();
    Serial.print("success to save :" + fbdo.dataPath());
  }else{
    Serial.println("failed" + fbdo.errorReason());
  }
  if(Firebase.RTDB.setFloat(&fbdo, "sensors/humidity", humid)){
    Serial.println();
    Serial.println("success to save :" + fbdo.dataPath());
  }else{
    Serial.println("failed" + fbdo.errorReason());
  }

}

void control_door(){
  if (Firebase.RTDB.getBool(&fbdo, "/door_control/status")) {
    doorControl = fbdo.boolData();
    Serial.println(lightControl);
    if(doorControl){
      myservo.write(OPEN);
    }else{
      myservo.write(CLOSE);
    }
  }
}
void control_LED(){
  if (Firebase.RTDB.getBool(&fbdo, "/light_control/status")) {
    lightControl = fbdo.boolData();
    Serial.println(lightControl);
    if (lightControl == 1) {
      //get RGB
      Firebase.RTDB.getInt(&fbdo, "/light_control/colors/red");
      R = fbdo.intData();
      Firebase.RTDB.getInt(&fbdo, "/light_control/colors/green");
      G = fbdo.intData();
      Firebase.RTDB.getInt(&fbdo, "/light_control/colors/blue");
      B = fbdo.intData();
      Serial.println(R);
      Serial.println(G);
      Serial.println(B);
      digitalWrite(RED_PIN, R);
      digitalWrite(BLUE_PIN, B);
      digitalWrite(GREEN_PIN, G);
      Serial.println("light on");
    } else {
      digitalWrite(RED_PIN, 0); 
      digitalWrite(BLUE_PIN, 0);
      digitalWrite(GREEN_PIN, 0);
      Serial.println("light off");
    }
  }
}



void control_FAN(){
  if (Firebase.RTDB.getBool(&fbdo, "/fan_control/status")) {
    fanControl = fbdo.boolData();
    Serial.println(fanControl);
    if (fanControl == 1) {
      digitalWrite(FAN_PIN, HIGH); 
      Serial.println("fan on");
    } else {
      Serial.println("fan off");
      digitalWrite(FAN_PIN, LOW); 
    }
  }
}


void loop() {
  currentMillis = millis(); // Get the current time
  // Check if enough time has passed
  if (currentMillis - previousMillis >= interval) {
    // Save the current time for comparison in the next iteration
    previousMillis = currentMillis;
  }
  getData();
  getFlameData();
  sendData();
  control_LED();
  control_FAN();
  control_door();
  // control_people();
  delay(1000);
}
