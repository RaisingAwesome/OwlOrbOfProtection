#include <ESP8266WiFi.h>    
#include <WiFiClientSecure.h>    
const char* ssid = "YOURSSID";    
const char* password = "YOURPASSWORD";    
const char* host = "api.pushbullet.com";    
const int httpsPort = 443;    
const char* PushBulletAPIKEY = "YOURPUSHBULLETKEY"; //get it from your pushbullet account    
// Use web browser to view and copy SHA1 fingerprint of the certificate.  Click the lock by the address in the browser and then click view certificate.    
// Alternately, go to https://www.grc.com/fingerprints.htm and enter api.pushbullet.com    
const char* fingerprint = "BB FC 9F 1B C1 3C D9 96 F2 68 A2 E3 41 29 D1 47 8F B9 33 BE";     
void setup() {    
  Serial.begin(115200);    
  Serial.println();    
  Serial.print("connecting to ");    
  Serial.println(ssid);    
  WiFi.mode(WIFI_STA);    
  WiFi.begin(ssid, password);    
  while (WiFi.status() != WL_CONNECTED) {    
    delay(500);    
    Serial.print(".");    
  }    
  Serial.println("");    
  Serial.println("WiFi connected");    
  Serial.println("IP address: ");    
  Serial.println(WiFi.localIP());    
  pushBullet("The Owl Orb of Protection Has Started!");    
  pinMode(2,INPUT_PULLUP);    
}    
void pushBullet(String the_msg){    
      // Use WiFiClientSecure class to create TLS connection    
      WiFiClientSecure client;    
      Serial.print("connecting to ");    
      Serial.println(host);    
      if (!client.connect(host, httpsPort)) {    
        Serial.println("connection failed");    
        return;    
      }    
        
      if (client.verify(fingerprint, host)) {    
        Serial.println("certificate matches");    
      } else {    
        Serial.println("certificate doesn't match");    
      }    
      String url = "/v2/pushes";    
      String messagebody = "{\"type\": \"note\", \"title\": \"ESP8266\", \"body\": \""+the_msg+"\"}\r\n";    
      Serial.print("requesting URL: ");    
      Serial.println(url);    
        
      client.print(String("POST ") + url + " HTTP/1.1\r\n" +    
                   "Host: " + host + "\r\n" +    
                   "Authorization: Bearer " + PushBulletAPIKEY + "\r\n" +    
                   "Content-Type: application/json\r\n" +    
                   "Content-Length: " +    
                   String(messagebody.length()) + "\r\n\r\n");    
      client.print(messagebody);    
        
      Serial.println("request sent");    
        
      //print the response    
        
      while (client.available() == 0);    
        
      while (client.available()) {    
        String line = client.readStringUntil('\n');    
        Serial.println(line);    
      }        
}    
void loop() {    
  delay(200);    
  if (digitalRead(2)==LOW) {//Wait for pin 2 to go to ground to trigger.    
    pushBullet("The back porch alarm has been sounded!");    
    delay(5000);    
  }    
