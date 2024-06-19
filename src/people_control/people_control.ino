#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"


#define API_KEY "AIzaSyDWPoNkYHQ63UPXhjOkL1lqGcevg2by4q4"
#define DATABASE_URL "https://smarthome-7a84a-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "team3@gmail.com"
#define USER_PASS "namnam"
#define WIFI_SSID "GPBL2024"
#define WIFI_PASS "gpbl@2024"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define trigPin1 3 // Pin Trigger cho cảm biến 1
#define echoPin1 5 // Pin Echo cho cảm biến 1
#define trigPin2 6 // Pin Trigger cho cảm biến 2
#define echoPin2 7 // Pin Echo cho cảm biến 2
int count = 0; // Biến đếm số người

bool signupOK = false;
unsigned long sendDataPrevMillis = 0;


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
  WiFi_setup();
  Serial_setup();
  Firebase_setup();
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
}

void sendData(){
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 ||sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  }
  if(Firebase.RTDB.setInt(&fbdo, "/control_people", count)){
    Serial.println();
    Serial.print("success to save :" + fbdo.dataPath());
  }else{
    Serial.println("failed" + fbdo.errorReason());
  }
}
void control_people() {
  int distance1 = getDistance(trigPin1, echoPin1);
    Serial.println(distance1);

  int distance2 = getDistance(trigPin2, echoPin2);
    Serial.println(distance2);

  if (distance1 < 100) {
    count++;

    delay(1000); // Đợi 1 giây để tránh đếm nhiều lần cho một lần phát hiện
  }
  if (distance2 < 100) {
    count--;
    Serial.println("Someone exited");
    delay(1000); // Đợi 1 giây để tránh đếm nhiều lần cho một lần phát hiện
  }
  Serial.print("Current count: ");
  Serial.println(count);
  delay(100); // Đợi một chút trước khi đọc lại cảm biến
}

int getDistance(int trigPin, int echoPin) {
  long duration;
  int distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  control_people();
  sendData();
  delay(500);
}