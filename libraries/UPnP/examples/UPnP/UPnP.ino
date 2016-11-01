#include <ESP8266WiFi.h>
#include <UPnP.h>
#include <UPnP/WebServer.h>
#include <UPnP/UPnPDevice.h>
#include <UPnP/SSDP.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#define TIME_CLASS
#include <UPnP/Time.h>

#include <Wire.h>

static int OTAprev;

#include "MotionSensorService.h"
#include "UPnP/LEDService.h"
#include "UPnP/DHTSensorService.h"
#ifdef ENABLE_BMP_SERVICE
#include <Wire.h>
#include "SFE_BMP180.h"
#include <UPnP/BMP180SensorService.h>
#endif

#include <SmtpClient.h>

#include <UPnP/DS3231.h>

// #include "GDBStub.h"

// Stuff to sync this source file in github
// Provide a "mywifi.h" file that defines the two macros below
//
// So copy something like this into that file and put in the right values :
// #define MY_SSID "your-ssid"
// #define MY_WIFI_PASSWORD "your-password"

#include "mywifi.h"
const char* ssid     = MY_SSID;
const char* password = MY_WIFI_PASSWORD;

// const char *serviceType = "urn:danny-backx-info:service:sensor:1";
// const char *serviceId = "urn:danny-backx-info:serviceId:sensor1";
char *deviceURN = "urn:schemas-upnp-org:device:ESP8266:1";

WebServer HTTP(80);
UPnPDevice device;
#ifdef TIME_CLASS
Time *theTime = NULL;
#endif

void setup() {
  Serial.begin(9600);
  
  delay(3000);    // Allow you to plug in the console
  
  Serial.println("Sensor system");
  Serial.printf("Boot version %d, flash chip size %d, SDK version %s\n",
                ESP.getBootVersion(), ESP.getFlashChipSize(), ESP.getSdkVersion());
  Serial.printf("Free sketch space %d\n", ESP.getFreeSketchSpace());
  
  Serial.print("Starting WiFi... ");
  WiFi.mode(WIFI_STA);

  int wifi_tries = 3;
  int wcr;
  while (wifi_tries-- >= 0) {
    WiFi.begin(ssid, password);
    wcr = WiFi.waitForConnectResult();
    if (wcr == WL_CONNECTED)
      break;
    Serial.printf(" %d ", wifi_tries + 1);
  }

  if (wcr != WL_CONNECTED) {
    Serial.printf("WiFi Failed\n");
    delay(2000);
    ESP.restart();
  }

  IPAddress ip = WiFi.localIP();
  String ips = ip.toString();
  IPAddress gw = WiFi.gatewayIP();
  String gws = gw.toString();
  Serial.print("MAC "); Serial.print(WiFi.macAddress());
  Serial.printf(", SSID {%s}, IP %s, GW %s\n", WiFi.SSID().c_str(), ips.c_str(), gws.c_str());

#ifdef TIME_CLASS
  theTime = new Time();
  theTime->begin();
#else
// Set up real time clock
  sntp_init();
  sntp_setservername(0, "ntp.scarlet.be");
  sntp_setservername(1, "ntp.belnet.be");
  (void)sntp_set_timezone(+1);
#endif

#ifdef ENABLE_SPIFFS
  SPIFFS.begin();
  Dir dir = SPIFFS.openDir("/");
  Serial.println("SPIFFS directory {/} :");
  while (dir.next()) {
    Serial.print("  "); Serial.println(dir.fileName());
  }

  FSInfo fs_info;
  SPIFFS.info(fs_info);
  Serial.printf("SPIFFS total %d used %d maxpathlength %d\n",
                fs_info.totalBytes, fs_info.usedBytes, fs_info.maxPathLength);

  //    SPIFFS.format();
  //    SPIFFS.remove("/config.xml");

  // Create a file
  File f = SPIFFS.open("/config.txt", "r");
  if (f) {
    Serial.printf("SPIFFS : /config.txt exists, content %d bytes\n", f.size());
    f.close();
  } else {
    f = SPIFFS.open("/config.txt", "w");
    f.printf("LED:active:50\n");
    f.printf("LED:passive:450\n");
    f.close();
  }
#endif

  Serial.printf("Starting HTTP...\n");
  HTTP.on("/index.html", HTTP_GET, []() {
    // Serial.println("Sending hello");
    HTTP.send(200, "text/plain", "Hello World!");
    // Serial.println("Hello ok");
  });
  HTTP.begin();

  Serial.printf("Starting SSDP...\n");
  device.setDeviceURN(deviceURN);
  device.setSchemaURL("description.xml");
  device.setHTTPPort(80);
  device.setName("UPnP Motion Sensor Kit");
  device.setSerialNumber("32302000102");
  device.setURL("index.html");
  device.setModelName("UPnP Motion Sensor Kit");
  device.setModelNumber("32302000102");
  device.setModelURL("http://danny.backx.info");
  device.setManufacturer("Danny Backx");
  device.setManufacturerURL("http://danny.backx.info");
  SSDP.begin(device);

  UPnP.begin(&HTTP, &device);

  MotionSensorService ms_srv = MotionSensorService();
  UPnP.addService(&ms_srv);

#ifdef ENABLE_LED_SERVICE
  LEDService led_srv = LEDService();
  led_srv.begin();
  UPnP.addService(&led_srv);
#endif
#ifdef ENABLE_DHT_SERVICE
  DHTSensorService dht = DHTSensorService();
  UPnP.addService(&dht);
  dht.begin();
#endif

#ifdef ENABLE_BMP_SERVICE
  BMP180SensorService bmp = BMP180SensorService();
  UPnP.addService(&bmp);
  bmp.begin();
#endif

#ifdef ENABLE_OTA
  Serial.printf("Starting OTA listener...\n");
  ArduinoOTA.onStart([]() {
    Serial.print("OTA Start : ");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int curr;
    curr = (progress / (total / 50));
    if (OTAprev < curr)
      Serial.print('#');
    OTAprev = curr;
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("OTA-Sensor");
  ArduinoOTA.begin();
#endif

#ifdef TIME_CLASS
  time_t t = theTime->getTime();
  Serial.printf("Time is %s", asctime(localtime(&t)));
#else
  // Wait for a correct time, and report it
  time_t t;
  Serial.printf("Time ");
  t = sntp_get_current_timestamp();
  while (t < 0x1000) {
    Serial.printf(".");
    delay(1000);
    t = sntp_get_current_timestamp();
  }
  Serial.printf(" is %s", asctime(localtime(&t)));
#endif

  Serial.printf("Ready!\n");


  // theTime->test();
  DS3231 *rtc = new DS3231();
  rtc->test();
#ifdef ENABLE_BMP_SERVICE
  Serial.printf("BMP180 : temperature %s pressure %s\n", bmp.GetTemperature(), bmp.GetPressure());
#endif

  while (1) {
    ms_srv.poll();
    HTTP.handleClient();
#ifdef ENABLE_LED_SERVICE
    led_srv.periodic();
#endif
#ifdef ENABLE_DHT_SERVICE
    dht.poll();
#endif
#ifdef ENABLE_BMP_SERVICE
    bmp.poll();
#endif

#ifdef ENABLE_OTA
    ArduinoOTA.handle();
#endif
    // Serial.printf("After HandleClient : Heap %X\n", ESP.getFreeHeap());
    delay(10);
  }
}

void loop() {
  HTTP.handleClient();
  Serial.printf("After HandleClient : Heap %X\n", ESP.getFreeHeap());
  // Serial.printf("Called handleClient()...\n");
  delay(10);
}
