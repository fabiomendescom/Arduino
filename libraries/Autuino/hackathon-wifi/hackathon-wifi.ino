/**
 * Pin config
 * ESP8266    DHT11
 * 3.3        +
 * GND        -
 * D4(GPI02)  Data
 * 
 * You must use this DHT library to work
 * https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver/code
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "argo";
#define wifi_ssid "IntHack2016"
//#define wifi_ssid "mendes"
#define wifi_password "IntappHacks!"
//#define wifi_password "MENDESFAMILY"

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 4; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

WiFiClientSecure client;
WiFiClient clientnossl;

const char* host = "demo2.terms.intapp.com";
const int httpsPort = 8085;

const int httpTime = 8084;
const char* hostTime = "192.168.0.71";

String httpGetTime() {
  if (clientnossl.connect(hostTime, httpTime)) {
    clientnossl.print(String("GET ") + "/" + " HTTP/1.1\r\n" +
               "Host: " + hostTime + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
    
    delay(500);
    String line = clientnossl.readString();
    //Serial.println(line);                 
    return line;
  }
  else {
    return "ERROR";
  }  
}

String httpsGet() {
  if (client.connect(host, httpsPort)) {
    client.print(String("GET ") + "/api/common/v1/matters/2011?clientId=30000" + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Accept: application/json\r\n" +
               "Authorization: Bearer TGLJMYpQWEsfZ_g_bi1RdADY-8o9KE1SZQ0lXtdTyE3EjJlcrmGpu5LNJ7P1MKzNqQwcN9y4_uJy-iDKlDAMExiprCex5lEXACHvWRZ_laFNTe7Ezo82fCvHd7ZOteReFUWmWV7arYudx6DeKwp9tPFjxAu3zmIcf2EMTaQlG84sNyYtOdEn9CZ39eLkI_NGdnXL7yp3JsFgraHxLmrAxOlrCF-bop9udFKRUPVpHHChegPOat6Go3Gnk5YsnIgtFVzyW9hU9z0P35TiPV_1Eyasf5ltU6A24kHUS4I4DVw8Ojfc\r\n" +
               "Connection: close\r\n\r\n");
    
    delay(1000);
    String line = client.readString();
    line = line.substring(line.indexOf("{"));
    //Serial.println(line);                 
    return line;
  }
  else {
    return "ERROR";
  }
}


String httpsPatch(String description) {
  if (client.connect(host, httpsPort)) {
    String data = String("{\"description\":\"" + description + "\"}");
    client.print(String("PATCH ") + "/api/common/v1/matters/2011?clientId=30000" + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Accept: application/json\r\n" +
               "Authorization: Bearer TGLJMYpQWEsfZ_g_bi1RdADY-8o9KE1SZQ0lXtdTyE3EjJlcrmGpu5LNJ7P1MKzNqQwcN9y4_uJy-iDKlDAMExiprCex5lEXACHvWRZ_laFNTe7Ezo82fCvHd7ZOteReFUWmWV7arYudx6DeKwp9tPFjxAu3zmIcf2EMTaQlG84sNyYtOdEn9CZ39eLkI_NGdnXL7yp3JsFgraHxLmrAxOlrCF-bop9udFKRUPVpHHChegPOat6Go3Gnk5YsnIgtFVzyW9hU9z0P35TiPV_1Eyasf5ltU6A24kHUS4I4DVw8Ojfc\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + (String)data.length() + "\r\n\r\n" + 
               data);

               String a = String("PATCH ") + "/api/common/v1/matters/2011?clientId=30000" + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Accept: application/json\r\n" +
               "Authorization: Bearer TGLJMYpQWEsfZ_g_bi1RdADY-8o9KE1SZQ0lXtdTyE3EjJlcrmGpu5LNJ7P1MKzNqQwcN9y4_uJy-iDKlDAMExiprCex5lEXACHvWRZ_laFNTe7Ezo82fCvHd7ZOteReFUWmWV7arYudx6DeKwp9tPFjxAu3zmIcf2EMTaQlG84sNyYtOdEn9CZ39eLkI_NGdnXL7yp3JsFgraHxLmrAxOlrCF-bop9udFKRUPVpHHChegPOat6Go3Gnk5YsnIgtFVzyW9hU9z0P35TiPV_1Eyasf5ltU6A24kHUS4I4DVw8Ojfc\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + (String)data.length() + "\r\n\r\n" + 
               data;

               //Serial.println(">>>>>>>>>>>>>>");
               //Serial.println(a);
               //Serial.println(">>>>>>>>>>>>>>");
  
    delay(1000);
    String line = client.readString();
    line = line.substring(line.indexOf("{"));
    //Serial.println(line);                 
    return line;
  }
  else {
    return "ERROR";
  }
}

void setupWiFiStation() {
  WiFi.mode(WIFI_STA);   
  WiFi.begin(wifi_ssid, wifi_password);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {  
    delay(100);
  }
  Serial.println("WIFI connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

long seconds(int secs) {
    return secs * 1000 * 1000;
}

long minutes(int mins) {
  return seconds(mins*60);
}

void setup()  
{ 
  Serial.begin(9600);
  Serial.println("Starting");
  setupWiFiStation();  
}

void loop()      
{  
  if(Serial.available() > 0) {
      int incomingByte = Serial.read();
      if(incomingByte==49) {          
          String res = httpsGet();
          if(res.indexOf("\"description\":\"vacation request pending dina\"") > 0) {
            String d = res.substring(res.indexOf("vacation request pending"),res.indexOf("\",",res.indexOf("vacation request pending")));    
            d = d + " Fabio";   
            String res = httpsPatch(d);
            Serial.println(d);
          }   
      }
      if(incomingByte==50) {          
        String res = httpGetTime();
        Serial.println(res);
      }                   
  }        
}




