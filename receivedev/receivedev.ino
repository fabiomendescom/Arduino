//#include "MyTransport.h"
//#include "MyTransportNRF24.h"
#include "AutuinoTransportNRF24L01.h"

//MyTransportNRF24 radio;

AutuinoTransportNRF24L01 radio;

uint8_t addr = 10;

void setup() {
  Serial.begin(9600);  
  Serial.println("Starting radio");
  radio = AutuinoTransportNRF24L01(8, 7, RF24_PA_LOW);

  if(radio.init()) {
    Serial.println("Radio open");
  } else {
    Serial.println("Failed to open radio");
    while(1);
  }

  //if(radio.init()) {
   // Serial.println("Radio started");
   // radio.setAddress(addr);
   // Serial.print("Radio set to address ");
  //  Serial.println(radio.getAddress());
 // } else {
  //  Serial.println("Failed to open radio");
  //  while(1);
 // }
}

void loop() {
  //char payload[] = "                                            "; 
  //uint8_t ad = 10;
  //if(radio.available(&ad)) {
  //     int le = radio.receive((uint8_t *)&payload);
     //  Serial.print("Received: ");
      // Serial.println(payload);
 // }
 // delay(3000);
  
}
