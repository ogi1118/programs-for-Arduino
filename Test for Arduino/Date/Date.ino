/*
日時を取得するアプリ
Apple For Getting Date Information
*/

//https://codepal.ai/code-generator/query/8muhID0k/arduino-cloud-data-logging  
//https://github.com/arduino-libraries/ArduinoHttpClient/blob/master/examples/SimpleGet/SimpleGet.ino  
//https://ameblo.jp/ogura-castle/entry-12821772466.html  
//https://garretlab.web.fc2.com/arduino.cc/docs/tutorials/uno-r4-wifi/rtc/  

#include <WiFiS3.h>  
#include "Arduino_LED_Matrix.h"  
#include "RTC.h"  
#include <ArduinoHttpClient.h>  
  
const char* ssid     = "ANLI-PC" ; // WifiのSSID / SSID for Wifi
const char* password = "917d81sW" ; // Wifiのパスワード / Password for Wifi
  
WiFiUDP Udp;  
RTCTime currentTime;  

char serverAddress[] = "172.31.111.23";  // サーバのアドレス / Server Address  
  
int port = 80;  // ポート / Port
WiFiClient wifi;  
HttpClient client = HttpClient(wifi, serverAddress, port);  
int status = WL_IDLE_STATUS;  
  
  
void setup() {  

  Serial.begin(115200);  
  
  RTC.begin();  
  WiFi.begin(ssid, password);  
  delay(3000);  

  while (WiFi.status() != WL_CONNECTED)  
    delay(500);  
  
  //現在時間の取得 / Get Current Time
  get_now_date_time();  
  
}  
  
int cnt = 0;  
void loop() {  
  
  //現在時間の取得 / Get Current Time 
  String fff = get_now_date_time();  
  Serial.println(fff);  
  delay(1000);  
  
  // //PHP経由で指示を与える  
  // String response = "/pumpswphp.php?ping_txt=" + String(cnt) + "___" + fff;  
  // client.get(response);  
   
  // int statusCode = client.responseStatusCode();  
  // response = client.responseBody();  
  // Serial.println(statusCode);  
  // Serial.println(response);  
  
  delay(1000);  
  cnt++;  
}  
  
//現在時間の取得 / Get Current Time
String get_now_date_time(){  

  byte packetBuffer[48];  
  int sts;  
  
  Udp.begin(2390);  
  memset(packetBuffer, 0, 48); //初期化 / Initialize 
  packetBuffer[0] = 0xE3;   // LI, Version, Mode  
  packetBuffer[1] = 0;     // Stratum, or type of clock  
  packetBuffer[2] = 6;     // Polling Interval  
  packetBuffer[3] = 0xEC;  // Peer Clock Precision  
  packetBuffer[12]  = 49;  
  packetBuffer[13]  = 0x4E;  
  packetBuffer[14]  = 49;  
  packetBuffer[15]  = 52;  
  delay(3000);  
  
  long tim_cnt = 0;  
  do{  
    if (tim_cnt < 10){  
      //10回試行してみる / Attemp 10 times
      sts = Udp.beginPacket("pool.ntp.org", 123); //NTP requests are to port 123  
    }  
    else if (tim_cnt < 20){  
      //10回試行してみる / Attemp 10 times
      sts = Udp.beginPacket("time.nist.gov", 123); //NTP requests   
    }  
    else if (tim_cnt < 30){  
      //10回試行してみる / Attemp 10 times
      sts = Udp.beginPacket("time-a.nist.gov", 123); //NTP requests are to port 123  
    }  
    else if (tim_cnt < 40){  
      //10回試行してみる / Attemp 10 times
      sts = Udp.beginPacket("time-b.nist.gov", 123); //NTP requests are to port 123  
    }  
    else  
    { //すべての時間の取得に失敗した場合 / If All attemps for getting date information was failed
      Serial.println("Date NG++++++++++++++++++++++++++++++++++++++++++++");  
      return "";  
    }  
  
    tim_cnt++;  
    delay(1000);  
  }while(!sts);  
  
  tim_cnt = 0;  
  sts = Udp.write(packetBuffer, 48);  
  do{  
    sts = Udp.endPacket();  
    if (tim_cnt > 10000000){ return ""; } //取得失敗 / Fail to get date information
    tim_cnt++;  
  }while(!sts);  
  
  tim_cnt = 0;  
  while(!Udp.parsePacket()){  
    if (tim_cnt > 10000000){ return ""; } //取得失敗 / Fail to get date information
    tim_cnt++;  
    delay(500);  
    }  
  Udp.read(packetBuffer, 48); // read the packet into the buffer  
  
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);  
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
  unsigned long secsSince1900 = highWord << 16 | lowWord;  
  unsigned long seventyYears = 2208988800UL;  
  unsigned long secondsSince1970 = secsSince1900 - seventyYears + (9 * 3600); //日本時間は+9時間を秒で算出 / The time in Japan should be calculated +9 hours
  RTCTime timeToSet = RTCTime(secondsSince1970);  
  RTC.setTime(timeToSet);  
  RTC.getTime(currentTime);  
  
  Serial.println("Date OK++++++++++++++++++++++++++++++++++++++++++++");  
  String fff = String(currentTime.getYear());  
  fff += "/";  
  fff += String(Month2int(currentTime.getMonth()));  
  fff += "/";  
  fff += String(currentTime.getDayOfMonth());  
  fff += "_";  
  fff += String(currentTime.getHour());  
  fff += ":";  
  fff += String(currentTime.getMinutes());  
  fff += ":";  
  fff += String(currentTime.getSeconds());  
  
  return fff;  
} 