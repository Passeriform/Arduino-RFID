#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>

#define RX_pin D9
#define TX_pin D10

SoftwareSerial mySerial(RX_pin, TX_pin); // RX, TX on nodemcu
int timeout = 30;
char* ssid = "id";  //Wifi Settings
char* pass = "password";
String host = "http://rnc-test.000webhostapp.com"; //Change this to local storage
String poster = "/extract.php";
String postData;
String member_name;
String club_name;
String content = "";
char character;
HTTPClient http;
void setup() {
  Serial.begin(115200);   // Initiate a serial communication
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  mySerial.begin(115200);
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
  {
    if (i < timeout) {
      delay(500);
      Serial.print(".");
    }
    else {
      Serial.println("Couldn't connect to selected network. Connection timed out.");
      Serial.println("Retrying...");
      i = 0;
    }
  }
  mySerial.write(10);
  Serial.print("\nConnected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  while(mySerial.read()!=69)
  {
    return;
    Serial.print(".");
  }
  while(mySerial.read()!=420) {Serial.print("."); continue;}
  character = mySerial.read();
  content.concat(character);
  Serial.print(content);
  while(0){
  postData = "uid=" + content;
  http.begin(host + poster);
  http.addHeader("Accept-Language", "en-US,en;q=0.5");
  http.addHeader("Accept-Encoding", "gzip, deflate, br");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Connection", "keep-alive");
  int httpCode = http.POST(postData);
  String payload = http.getString();
  Serial.println(payload);
  mySerial.print(payload[0]);
  for(int i=1;i<13;i++)  payload[i];
  http.end();}
}
