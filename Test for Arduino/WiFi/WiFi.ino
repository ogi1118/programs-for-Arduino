#include <WiFi.h>
#include <WiFiUDP.h>

#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "ANLI-PC";        // your network SSID (name)
char pass[] = "917d81sW";    // your network password (use for WPA, or use as key for WEP)

static WiFiUDP wifiUdp; 
static const char *serverIP = "172.31.111.23";
static const int serverPort = 52005; //送信先のポート

IPAddress remoteIP; // 相手のIPアドレス
int port;

static void WiFi_setup(){

  static const int localPort = 7000;  //自身のポート
  WiFi.begin(ssid, pass);
  int cou=0;

  while( WiFi.status() != WL_CONNECTED) {
    char line[100];
    sprintf(line,"connecting... %d", cou);
    Serial.println(line);
    cou++;
    delay(1000);  
  }  
  wifiUdp.begin(localPort);

}

static void Serial_setup(){
  Serial.begin(115200);
}

void setup(){
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
