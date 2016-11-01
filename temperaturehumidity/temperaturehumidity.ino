/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik EKblad
 * 
 * DESCRIPTION
 * This sketch provides an example how to implement a humidity/temperature
 * sensor using DHT11/DHT-22 
 * http://www.mysensors.org/build/humidity
 */
 
#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>  

#define CHILD_ID_HUM 1
#define CHILD_ID_TEMP 2
#define HUMIDITY_SENSOR_DIGITAL_PIN 7
unsigned long SLEEP_TIME = 600000; // Sleep time between reads (in milliseconds)

#define RADIO_CE_PIN        5    // radio chip enable
#define RADIO_SPI_SS_PIN    6   // radio SPI serial select

DHT dht;
float lastTemp;
float lastHum;
boolean metric = false; 
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

// NRFRF24L01 radio driver (set low transmit power by default) 
MyTransportNRF24 transport(RADIO_CE_PIN, RADIO_SPI_SS_PIN, RF24_PA_LEVEL_GW);  

//MyTransportRFM69 transport;

// Message signing driver (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
//MySigningNone signer;
//MySigningAtsha204Soft signer;
//MySigningAtsha204 signer;

// Hardware profile 
MyHwATMega328 hw;

// Construct MySensors library (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
MySensor gw(transport, hw /*, signer*/);

void setup()  
{ 
  gw.begin(NULL, AUTO, false, 0);   
  //Serial.print("Node ID: ");
  //Serial.println(gw.getNodeId());

  // Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo("HumidityTemperature", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_HUM, S_HUM, "Humidity Sensor", true);
  gw.present(CHILD_ID_TEMP, S_TEMP, "Temperature Sensor", true);

  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN); 
  metric = false; 
}

void loop()      
{  
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT");
  } else if (temperature != lastTemp) {
    //lastTemp = temperature;
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    gw.send(msgTemp.set(temperature, 1));
    //Serial.print("T: ");
    //Serial.println(temperature);
  }
  
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      //lastHum = humidity;
      gw.send(msgHum.set(humidity, 1));
      //Serial.print("H: ");
      //Serial.println(humidity);
  }
  delay(10000);

  gw.sleep(SLEEP_TIME); //sleep a bit
}




