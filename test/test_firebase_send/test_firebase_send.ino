// #include <Firebase.h>
#include <Firebase_ESP_Client.h>

#define FIREBASE_HOST "https://console.firebase.google.com/u/0/project/smart-home-8ddc1/database/smart-home-8ddc1-default-rtdb/data/"
#define FIREBASE_AUTH ""
#define DATABASE_URL ""
#define API_KEY "AIzaSyBnjwp3mSDKYeCQoIN10P6sPSayCCyLOMA"
#define USER_EMAIL "nam52871@donga.edu.vn"
#define USER_PASSWORD "namnam"
#define WIFI_SSID "GPBL2024"
#define WIFI_PASSWORD "gpbl@2024"

FirebaseJson json;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


static void WiFi_setup(){
  Serial.print("connecting");
  static const int local_port = 7000;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  // wifiUdp.begin(local_port);

}


static void Serial_setup(){
  Serial.begin(9600);
}


void setup() {
  // put your setup code here, to run once:
  WiFi_setup();
  Serial_setup();

  
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
