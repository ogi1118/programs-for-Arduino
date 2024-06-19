#include "DHT.h"
#include <WiFi.h>
// #include <WiFiNINA.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define DHTTYPE DHT11
#define DHT_PIN 2
#define FAN_PIN 4
#define BLUE_PIN 11
#define GREEN_PIN 12
#define RED_PIN 13

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
float data[2];
bool lightControl;
bool fanControl;

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

}

float *getData(float *data){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Serial.println(h);
    Serial.println(t);
    Serial.println("");
    return data;
  }

  data[0] = h;
  data[1] = t;
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.println("");
  return data;
}

void sendData(float *data){
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 ||sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  }
  if(Firebase.RTDB.setFloat(&fbdo, "sensors/temperature", data[1])){
    Serial.println();
    Serial.print("success to save :" + fbdo.dataPath());
  }else{
    Serial.println("failed" + fbdo.errorReason());
  }
  if(Firebase.RTDB.setFloat(&fbdo, "sensors/humidity", data[0])){
    Serial.println();
    Serial.println("success to save :" + fbdo.dataPath());
  }else{
    Serial.println("failed" + fbdo.errorReason());
  }

}

void control_LED(){
  if (Firebase.RTDB.getBool(&fbdo, "/light_control/status")) {
    lightControl = fbdo.boolData();
    Serial.println(lightControl);
    if (lightControl == 1) {
      digitalWrite(RED_PIN, 255);
      digitalWrite(BLUE_PIN, 0);
      digitalWrite(GREEN_PIN, 0);
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
  getData(data);
  sendData(data);
  control_LED();
  control_FAN();
  delay(500);
}
