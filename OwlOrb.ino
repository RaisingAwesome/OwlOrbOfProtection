#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

int sense_pin = 0;
int alarm_pin = 1;

unsigned long pulse_length;
unsigned long first_pulse;
int sensorValue[32];
unsigned long lengths[32];
int pulse_value;
float A, B, C, D;
float averageDistance, lastAverage;
int first_few_counter=0;

void setup()
{
  Serial.begin(115200);
  delay(1000);  
  A=0;B=0;C=0;D=0;
  averageDistance=0;lastAverage;
  pinMode(sense_pin,INPUT);
  pinMode(alarm_pin,OUTPUT);
  digitalWrite(alarm_pin,HIGH); // Leave it high and pull it low to alarm.
  digitalWrite(A0,HIGH); //kills noise on the line;  
  if (!SD.begin(SDCARD_SS_PIN)) {
    Serial.println(" failed!");
    while(true);
  }  

  Serial.println("Starting...");
  delay(1000);
  PlaySound("owlHello.wav");
}

void loop() {
  Sense();
  delay(2000);
}

void Sense()
{  Serial.println("Sensing...");
  delay(1000);
  int i;
  //look for starter pulse
  pulse_length = pulseIn(sense_pin, HIGH);  
  
  // wait for the long pulse to signify the start
  while (pulse_length < 1900) {
    pulse_length = pulseIn(sense_pin, HIGH);
  }
  first_pulse=pulse_length;
  
  // consume the throw away bit
  pulse_length = pulseIn(sense_pin, HIGH);

  // Get the 32 bits to follow
  for (i = 0; i < 32; i ++) {
    pulse_length = pulseIn(sense_pin, HIGH);

    if (pulse_length < 180) {
      pulse_value = 0;
    }
    else {
      if (pulse_length > 1000) {
        Serial.println("Overran!");
        delay(1000);
        return; // just exit out of the routine if it fails to sense correctly this time through.
      }
      pulse_value = 1;
    }
    sensorValue[i] = pulse_value;
    lengths[i]=pulse_length;
  }

  // Sensor A
  if (sensorValue[0]==0) {
    //String sensorA=String(sensorValue[3])+String(sensorValue[4])+String(sensorValue[5])+String(sensorValue[6])+String(sensorValue[7]);
    //Serial.println(sensorA);
    A=16*sensorValue[3] + 8*sensorValue[4] + 4*sensorValue[5] + 2*sensorValue[6] + sensorValue[7];
    A=A/(10*.3048); // convert decimeters to feet
    Serial.println(A);
  }
 
  // Sensor D
  if (sensorValue[8]==0) {
    D=16*sensorValue[11] + 8*sensorValue[12] + 4*sensorValue[13] + 2*sensorValue[14] + sensorValue[15];
    D=D/(10*.3048); // convert decimeters to feet
    Serial.println(D);
  }
  
  // Sensor C
  if (sensorValue[16]==0) {
    C=16*sensorValue[19] + 8*sensorValue[20] + 4*sensorValue[21] + 2*sensorValue[22] + sensorValue[23];
    C=C/(10*.3048); // convert decimeters to feet
    Serial.println(C);
  }
  
  // Sensor B
  if (sensorValue[24]==0) {
    B=16*sensorValue[27] + 8*sensorValue[28] + 4*sensorValue[29] + 2*sensorValue[30] + sensorValue[31];
    B=B/(10*.3048); // convert decimeters to feet
    Serial.println(B);
  }
  averageDistance=(A+B+C+D)/4;
  if (((abs(lastAverage-averageDistance))/lastAverage)>.1) alarm();
  lastAverage=averageDistance;
}
void alarm(){
  if (first_few_counter++<3) return; // this keeps it from triggering when you first set it down and turn it on.
  digitalWrite(alarm_pin,LOW);
  PlaySound("alarm.wav");
  delay(5000);
  digitalWrite(alarm_pin,HIGH);
}

void PlaySound(String the_sound)
{
  File myFile;

  // open wave file from sdcard
  myFile = SD.open(the_sound);
  
  // until the file is not finished  
  AudioZero.begin(44100);
  AudioZero.play(myFile);
  myFile.close();
  AudioZero.end();
  digitalWrite(A0,HIGH); //kills noise on the line;
}




ESP8266 Code:
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
