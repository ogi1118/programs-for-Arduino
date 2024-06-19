#include "DHT.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define DHTTYPE DHT11
#define DHT_PIN 2
#define FAN_PIN 4
#define BLUE_PIN 11
#define GREEN_PIN 12
#define RED_PIN 13
#define DATA_MAX_SIZE 1000

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
JsonDocument doc;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
float temp;
float humid;
bool lightControl;
bool fanControl;
int R;
int G;
int B;
char charData[DATA_MAX_SIZE];
String tmp;
int tmp_len;
int char_len;

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

void getTempHumid(){
  humid = dht.readHumidity();
  temp = dht.readTemperature();

  if (isnan(humid) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Serial.println(humid);
    Serial.println(temp);
    Serial.println("");
    return;
  }

  doc["humidity"] = humid;
  doc["temperature"] = temp;
  Serial.print(F("Humidity: "));
  Serial.print(humid);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.println(F("°C "));
  Serial.println("");
  return;
}

void getData(){
  Firebase.RTDB.getString(&fbdo, "");
  tmp = fbdo.jsonString();
  Serial.println(tmp);
  tmp_len = tmp.length() + 1;
  tmp.toCharArray(charData, tmp_len);
  deserializeJson(doc, charData);
  lightControl = doc["light_control"]["status"];
  Serial.println(lightControl);
  fanControl = doc["fan_control"]["status"];
  R = doc["light_control"]["colors"]["red"];
  G = doc["light_control"]["colors"]["green"];
  B = doc["light_control"]["colors"]["blue"];
  Serial.println(R);
}
void sendData(){
  char_len = strlen(charData);
  serializeJson(doc, charData, char_len);
  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 ||sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
  }
  if(Firebase.RTDB.setString(&fbdo,"", charData)){
    Serial.println();
    Serial.print("success to save :" + fbdo.dataPath());
  }else{
    Serial.println("failed" + fbdo.errorReason());
  }
}

void control_LED(){
  Serial.println(lightControl);
  if (lightControl) {
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



void control_FAN(){
  Serial.println(fanControl);
  if (fanControl) {
    digitalWrite(FAN_PIN, HIGH); 
    Serial.println("fan on");
  } else {
    Serial.println("fan off");
    digitalWrite(FAN_PIN, LOW); 
  }
}

void loop() {
  getTempHumid();
  Serial.println("a");
  sendData();
  Serial.println("b");
  getData();
  Serial.println("c");
  control_LED();
  Serial.println("d");
  control_FAN();
  Serial.println("e");
  delay(500);
}
