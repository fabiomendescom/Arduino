#include "MyTransport.h"
#include "MyTransportNRF24.h"

MyTransportNRF24 radio;
uint8_t addrfrom = 10;
uint8_t addrto = 10;

void setup() {
  Serial.begin(9600);  
  Serial.println("Starting radio");
  // put your setup code here, to run once:
  radio = MyTransportNRF24(8, 7, RF24_PA_LOW);

  if(radio.init()) {
    Serial.println("Radio started");
    radio.setAddress(addrfrom);
    Serial.print("Radio set to address ");
    Serial.println(radio.getAddress());
  } else {
    Serial.println("Failed to open radio");
    while(1);
  }
}

void loop() {
  char payload[] = "Fabioxxxxxxxxxxxxxxxxxxxxxxxxxx0000000"; 
  if(radio.send(addrto, &payload, sizeof(payload))) {
     Serial.print("Message sent: ");
     Serial.println(payload);
  } else {
     Serial.println("Transmission failed");
  }
  delay(2000);
}
