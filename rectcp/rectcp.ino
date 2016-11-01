#include "AutuinoTransportNRF24L01.h"

AutuinoTransportNRF24L01 radio;

void setup() {
  Serial.begin(9600);  
  Serial.println("Starting radio");
  radio = AutuinoTransportNRF24L01(8, 7, RF24_PA_LOW);

  radio.start();
  Serial.println("Radio Started");
  uint32_t macaddress = radio.getMACAddress();
  Serial.print("MAC Address: ");
  Serial.println((long)macaddress);
}

void loop() {
  Serial.println("Preparing to send message");
  char payload[] = "Fabio Mendes"; 
  if(radio.send(0x6307CEA4, &payload, sizeof(payload))) {
     Serial.print("Message sent: ");
     Serial.println(payload);
  } else {
     Serial.println("Transmission failed");
  }
  delay(2000);
}
