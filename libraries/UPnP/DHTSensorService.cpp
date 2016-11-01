/*
 * This is a sample of a UPnP service that runs on a IoT device.
 * 
 * UPnP commands/queries can be used from an application or a script.
 * This service represents the DHT-11 / DHT-22 temperature and humidity sensor.
 *
 * To be used with Rob Tillaart's DHTlib
 *   (https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib).
 * 
 * Copyright (c) 2015, 2016 Danny Backx
 * 
 * License (MIT license):
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 * 
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * 
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include "UPnP.h"
#include "UPnP/UPnPService.h"
#include "dht.h"
#include "UPnP/DHTSensorService.h"
#include "UPnP/WebServer.h"

extern WebServer HTTP;
static void GetVersion();

// #define DEBUG Serial

// Printf style template, parameters : serviceType, state
static const char *gsh_template = "<u:GetStateResponse xmlns=\"%s\">\r\n<State>%s</State>\r\n</u:GetStateResponse>\r\n";

static const char *getStateXML = "<action>"
  "<name>getState</name>"
  "<argumentList>"
  "<argument>"
  "<retval/>"
  "<name>State</name>"
  "<relatedStateVariable>State</relatedStateVariable>"
  "<direction>out</direction>"
  "</argument>"
  "</argumentList>"
  "</action>";

static const char *getVersionXML = "<action>"
  "<name>getVersion</name>"
  "<argumentList>"
  "<argument>"
  "<retval/>"
  "<name>Version</name>"
  "<direction>out</direction>"
  "</argument>"
  "</argumentList>"
  "</action>";

// These are static/global variables as a demo that you can query such variables.
static const char *versionTemplate = "%s %s %s\n";
static const char *versionFileInfo = __FILE__;
static const char *versionDateInfo = __DATE__;
static const char *versionTimeInfo = __TIME__;

static const char *myServiceName = "dht11Sensor";
static const char *myServiceType = "urn:danny-backx-info:service:dht11sensor:1";
static const char *myServiceId = "urn:danny-backx-info:serviceId:dht11sensor1";
static const char *stateString = "State";
static const char *getStateString = "getState";
static const char *getVersionString = "getVersion";
static const char *stringString = "string";

DHTSensorService::DHTSensorService() :
  UPnPService(myServiceName, myServiceType, myServiceId)
{
  addAction(getStateString, static_cast<MemberActionFunction>(&DHTSensorService::GetStateHandler), getStateXML);
  addAction(getVersionString, GetVersion, getVersionXML);
  addStateVariable(stateString, stringString, true);
}

DHTSensorService::DHTSensorService(const char *deviceURN) :
  UPnPService(myServiceName, myServiceType, myServiceId)
{
  addAction(getStateString, static_cast<MemberActionFunction>(&DHTSensorService::GetStateHandler), getStateXML);
  addAction(getVersionString, GetVersion, getVersionXML);
  addStateVariable(stateString, stringString, true);
}

DHTSensorService::DHTSensorService(const char *serviceType, const char *serviceId) :
  UPnPService(myServiceName, serviceType, serviceId)
{
  addAction(getStateString, static_cast<MemberActionFunction>(&DHTSensorService::GetStateHandler), getStateXML);
  addAction(getVersionString, GetVersion, getVersionXML);
  addStateVariable(stateString, stringString, true);
}

DHTSensorService::~DHTSensorService() {
#ifdef DEBUG
  DEBUG.println("DHTSensorService DTOR");
#endif  
}

void DHTSensorService::begin() {
#ifdef DEBUG
  DEBUG.println("DHTSensorService::begin");
#endif

  config = new Configuration("DHT",
    new ConfigurationItem("pin", DHT_SENSOR_PIN_DEFAULT),
    new ConfigurationItem("type", DHT_SENSOR_TYPE_DEFAULT),
    new ConfigurationItem("name", ""));
  UPnPService::begin(config);

  sensorpin = config->GetValue("pin");
  sensortype = config->GetValue("type");

  sensor = new dht();

#ifdef DEBUG
  DEBUG.printf("DHT::begin (sensor pin %d, type DHT-%d)\n", sensorpin, sensortype);
#endif
}

int inited = 0;

void DHTSensorService::poll() {
  int check = sensor->read11(sensorpin);

#ifdef DEBUG
  switch (check) {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    case DHTLIB_ERROR_CONNECT:
      Serial.print("Connect error,\t");
      break;
    case DHTLIB_ERROR_ACK_L:
      Serial.print("Ack Low error,\t");
      break;
    case DHTLIB_ERROR_ACK_H:
      Serial.print("Ack High error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }
#endif

  if (check != DHTLIB_OK)
    return;

  oldtemperature = newtemperature;
  oldhumidity = newhumidity;

  newtemperature = sensor->temperature;
  newhumidity = sensor->humidity;

  if (inited <= 3) {
    inited++;
#ifdef DEBUG
    DEBUG.print("DHT: temp ");
    DEBUG.print(newtemperature);
    DEBUG.print(" °C");
    DEBUG.print(", humidity ");
    DEBUG.println(newhumidity);
#endif
  }

  if (isnan(newtemperature) || isnan(newhumidity)) {
    newtemperature = oldtemperature;
    newhumidity = oldhumidity;
    return;
  }

  if (oldtemperature != newtemperature) {
    sprintf(state, "%d", newtemperature);

    SendNotify("State");
#ifdef DEBUG
    DEBUG.print("DHT: temp ");
    DEBUG.print(newtemperature);
    DEBUG.print(" °C");
    DEBUG.print(", humidity ");
    DEBUG.println(newhumidity);
#endif
  }
}

const char *DHTSensorService::GetState() {
  return state;
}

// Example of a static function to handle UPnP requests : only access to global variables here.
static void GetVersion() {
  char msg[128];
#ifdef DEBUG
  DEBUG.println("DHTSensorService::GetVersion");
#endif
  sprintf(msg, versionTemplate, versionFileInfo, versionDateInfo, versionTimeInfo);
  HTTP.send(200, UPnPClass::mimeTypeXML, msg);
}

// Example of a member function to handle UPnP requests : this can access stuff in the class
void DHTSensorService::GetStateHandler() {
  int l2 = strlen(gsh_template) + strlen(myServiceType) + DHT_STATE_LENGTH,
      l1 = strlen(UPnPClass::envelopeHeader) + l2 + strlen(UPnPClass::envelopeTrailer) + 5;
  char *tmp2 = (char *)malloc(l2),
       *tmp1 = (char *)malloc(l1);
#ifdef DEBUG
  DEBUG.println("DHTSensorService::GetStateHandler");
#endif
  strcpy(tmp1, UPnPClass::envelopeHeader);
  sprintf(tmp2, gsh_template, myServiceType, DHTSensorService::state);
  strcat(tmp1, tmp2);
  free(tmp2);
  strcat(tmp1, UPnPClass::envelopeTrailer);
  HTTP.send(200, UPnPClass::mimeTypeXML, tmp1);
  free(tmp1);
}
