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

#define INCLUSION_MODE_TIME 1 // Number of minutes inclusion mode is enabled
#define INCLUSION_MODE_PIN  3 // Digital pin used for inclusion mode button

#define RADIO_CE_PIN        5    // radio chip enable
#define RADIO_SPI_SS_PIN    6   // radio SPI serial select
//#define RADIO_ERROR_LED_PIN 10    // Error led pin
//#define RADIO_RX_LED_PIN    8   // Receive led pin
//#define RADIO_TX_LED_PIN    9   // the PCB, on board LED*/ 

#define PINSERVO 8
#define ID_TREATER 80

#include <Servo.h> 

 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
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
   //rxBlink(1);
   //sendMQTT(message);
   //if(message.data=="1") {
   //   digitalWrite(8,LOW);
   //} else {
      //digitalWrite(8,HIGH);   
   //}
   moveEngine();
   Serial.print("Message: ");
   Serial.print(message.data);
   Serial.println("");
} 


void setup() 
{ 
  // Initialize gateway at maximum PA level, channel 70 and callback for write operations 
  gw.begin(incomingMessage, ID_TREATER, false, 0); 
  gw.present(ID_TREATER, S_LIGHT);  
  
  Serial.println("Ok!");  

  pinMode(9,OUTPUT);
  pinMode(3,INPUT);

  myservo.attach(PINSERVO);  // attaches the servo on GIO2 to the servo object 
  myservo.write(0);   
} 
 
void loop() 
{ 
  gw.process();
  int pos;

  //Serial.println(digitalRead(3));

  if(digitalRead(3)==1) {
  //  for(int i=0;i<21;i++) {
        moveEngine();
  //  }    
  }
  
//digitalWrite(8,HIGH);  
/*
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(5000);
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
*/
} 

void playBuzzer() {
  tone(9,3000,1000);
  delay(100);
  tone(9,3500,1000);
  delay(100);
  tone(9,2000,1000);  
  delay(100);
  tone(9,3000,1000);
  delay(100);
  tone(9,3500,1000);
  delay(100);
  tone(9,2000,1000);  
  delay(100);
  tone(9,3000,1000);
  delay(100);
  tone(9,3500,1000);
  delay(100);
  tone(9,2000,3000);  
  delay(100);
  tone(9,2000,1000);  
  delay(100);
  tone(9,3000,1000);
  delay(100);
  tone(9,3500,1000);
  delay(100);
  tone(9,2000,1000);  
  delay(100);
  tone(9,3000,1000);
  delay(100);
  tone(9,3500,1000);    
  noTone(9);
}

void moveEngine() {
 // playBuzzer();

 // delay(3000);  
  
  int pos;
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(10);                       // waits 15ms for the servo to reach the position 
  } 
  delay(2000);
}


