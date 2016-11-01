#include <DigitalIO.h>
#include <SPI.h>

#include <MySigningNone.h>
#include <MyTransportRFM69.h>
#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>
#include <MySigningAtsha204Soft.h>
#include <MySigningAtsha204.h>

#include <MySensor.h>
#include <MsTimer2.h>

//#define INCLUSION_MODE_TIME 1 // Number of minutes inclusion mode is enabled
//#define INCLUSION_MODE_PIN  3 // Digital pin used for inclusion mode button

//#define RADIO_CE_PIN        5    // radio chip enable
//#define RADIO_SPI_SS_PIN    6   // radio SPI serial select
//#define RADIO_ERROR_LED_PIN 10    // Error led pin
//#define RADIO_RX_LED_PIN    8   // Receive led pin
//#define RADIO_TX_LED_PIN    9   // the PCB, on board LED*/ 

//#define ID_RELAY 5

//KY019 5V relay module
int relay = 10; // relay turns trigger signal - active high;
int relay2 = 11;

// NRFRF24L01 radio driver (set low transmit power by default) 
//MyTransportNRF24 transport(RADIO_CE_PIN, RADIO_SPI_SS_PIN, RF24_PA_LEVEL_GW);  

//MyTransportRFM69 transport;

// Message signing driver (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
//MySigningNone signer;
//MySigningAtsha204Soft signer;
//MySigningAtsha204 signer;

// Hardware profile 
//MyHwATMega328 hw;

// Construct MySensors library (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
//MySensor gw(transport, hw /*, signer*/);

//void incomingMessage(const MyMessage &message) {
   //rxBlink(1);
   //sendMQTT(message);
   //if(message.data=="1") {
   //   digitalWrite(8,LOW);
   //} else {
      //digitalWrite(8,HIGH);   
   //}
   //moveEngine();
//   if(message.data[0]=='1') {
//       digitalWrite (relay, HIGH); // relay conduction;  
//       Serial.println("HIGH");         
//   } else {
//       digitalWrite (relay, LOW); // relay conduction;  
//       Serial.println("LOW");
//   }
//   Serial.print("Message: ");
//   Serial.print(message.data);
//   Serial.println("");
//} 

void setup ()
{
  pinMode (relay, OUTPUT); // Define port attribute is output;
  pinMode (relay2, OUTPUT);
  digitalWrite (relay, LOW); // relay conduction;
  digitalWrite (relay2, LOW);  
    
  // Initialize gateway at maximum PA level, channel 70 and callback for write operations 
  //gw.begin(incomingMessage, ID_RELAY, false, 0); 
  //gw.present(ID_RELAY, S_LIGHT);  
  
  //Serial.println("Ok!");  
    
}

void loop ()
{
  //gw.process();
    
  //delay(5000);
  //digitalWrite (relay, HIGH); // relay conduction;
  //delay (1000);
  //digitalWrite (relay, LOW); // relay switch is turned off;
  //delay (1000);
  delay(30000);
  digitalWrite(relay,HIGH);
  digitalWrite(relay2,LOW);

  delay(10000);
  digitalWrite(relay, LOW);
  digitalWrite(relay2,HIGH);

  delay(10000);
  digitalWrite(relay,LOW);
  digitalWrite(relay2,LOW);

  

}
