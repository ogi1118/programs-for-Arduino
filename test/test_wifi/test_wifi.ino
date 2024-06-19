#include <WiFi.h>
#include <WiFiUDP.h>

#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

#define WIFI_SSID "GPBL2024"
#define WIFI_PASSWORD "gpbl@2024"

static WiFiUDP wifiUdp; 
static const char *serverIP = "172.16.14.172";
static const int serverPort = 52005; //送信先のポート

int port;

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
  wifiUdp.begin(local_port);

}

static void Serial_setup(){
  Serial.begin(9600);
}


void setup() {
  // put your setup code here, to run once:
  Serial_setup();
  WiFi_setup();
}

void loop(){

  int count=0;
  char i[64];

  while(1){

    count++;
    delay(1000);

    //read temperature, humidity
    float h = 11.0;
    float t = 22.0;

    Serial.print("Humidity : ");
    Serial.print(h);
    Serial.println(" %");
  
    Serial.print("Temperature : ");
    //t = t - 25;
    Serial.print(t);
    Serial.println(" °C");
    Serial.println("----------");

    ////////////////////

    //send packet to the server
    wifiUdp.beginPacket(serverIP, serverPort);

    wifiUdp.print(h);
    wifiUdp.print("*");
    wifiUdp.print(t);
    
    wifiUdp.endPacket();
    
  } //while end

} //loop end

