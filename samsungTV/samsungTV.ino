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

#include <IRremote.h>

#define INCLUSION_MODE_TIME 1 // Number of minutes inclusion mode is enabled
#define INCLUSION_MODE_PIN  3 // Digital pin used for inclusion mode button

#define RADIO_CE_PIN        5    // radio chip enable
#define RADIO_SPI_SS_PIN    6   // radio SPI serial select
//#define RADIO_ERROR_LED_PIN 10    // Error led pin
//#define RADIO_RX_LED_PIN    8   // Receive led pin
//#define RADIO_TX_LED_PIN    9   // the PCB, on board LED*/ 

#define POWER         0xE0E040BF            //from IRrecvDemo, Ken Shirriff
#define VOL_UP        0xE0E0E01F
#define VOL_DOWN      0xE0E0D02F
#define SOURCE        0xE0E0807F
#define MUTE          0xE0E0F00F
#define CH_UP         0xE0E048B7
#define CH_DOWN       0xE0E008F7
#define KEY_0         0xE0E08877
#define KEY_1         0xE0E020DF
#define KEY_2         0xE0E0A05F
#define KEY_3         0xE0E0609F
#define KEY_4         0xE0E010EF
#define KEY_5         0xE0E0906F
#define KEY_6         0xE0E050AF
#define KEY_7         0xE0E030CF
#define KEY_8         0xE0E0B04F
#define KEY_9         0xE0E0708F

#define SAMSUNG_BITS  32 

#define ID_IR 8

IRsend irsend;

//sets volume to 0 to have a starting point
void resetVolume() {
  for(int i=0;i<40;i++) {
      irsend.sendSAMSUNG(VOL_DOWN, SAMSUNG_BITS); 
      delay(200);
  }    
}

//the volume level you want
void setVolume(int vol) {
   resetVolume();
   for(int i=0;i<=vol;i++) {
      irsend.sendSAMSUNG(VOL_UP, SAMSUNG_BITS); 
      delay(800);    
   }
}

void turnONorOFF() {
  irsend.sendSAMSUNG(POWER, SAMSUNG_BITS);
}

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

void incomingMessage(const MyMessage &message) {
   if(message.data[0]=='0') { //turn TV off
      turnONorOFF();      
   } else if(message.data[0]=='1') {  //turn TV on
      turnONorOFF();
   } else if(message.data[0]=='2') { //volume up
      irsend.sendSAMSUNG(VOL_UP, SAMSUNG_BITS);
   } else if(message.data[0]=='3') { //volume down      
      irsend.sendSAMSUNG(VOL_DOWN, SAMSUNG_BITS);
   } else if(message.data[0]=='4') { //mute      
      irsend.sendSAMSUNG(MUTE, SAMSUNG_BITS);
   } else if(message.data[0]=='5') { //channel up      
      irsend.sendSAMSUNG(CH_UP, SAMSUNG_BITS);
   } else if(message.data[0]=='6') { //channel down      
     irsend.sendSAMSUNG(CH_DOWN, SAMSUNG_BITS);
   } else if(message.data[0]=='7') { //source      
      irsend.sendSAMSUNG(SOURCE, SAMSUNG_BITS);
   } else if(message.data[0]=='8') { //key press      
      //Finish this
   } else if(message.data[0]=='9') { //macro. Put volume all the way down      
      resetVolume();
   }  else if(message.data[0]=='A') { //macro. Set volume to a pre-defined level           
      setVolume(18);
   }     
   
} 

void setup()
{
  pinMode (3, OUTPUT);  //output as used in library
  gw.begin(incomingMessage, ID_IR, true, 0); 
  gw.sendSketchInfo("IR", "1.0");     
  gw.present(ID_IR, S_LIGHT,"IR",true);  
  
  Serial.println("Ok!");    
}

void loop() {
  gw.process(); 
}



