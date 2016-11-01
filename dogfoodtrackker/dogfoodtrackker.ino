/* 
  IR Breakbeam sensor demo!
*/

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

#define RADIO_CE_PIN        5		// radio chip enable
#define RADIO_SPI_SS_PIN    6		// radio SPI serial select
//#define RADIO_ERROR_LED_PIN 10		// Error led pin
//#define RADIO_RX_LED_PIN    8		// Receive led pin
//#define RADIO_TX_LED_PIN    9		// the PCB, on board LED*/ 

#define LEDPINWATER 10
#define SENSORPINWATER 9

#define LEDPINFOOD 8
#define SENSORPINFOOD 7

#define ID_FOOD_END 3
#define ID_WATER_END 4

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

// variables will change:
int sensorStateFood = 0, lastStateFood=0;         // variable for reading the pushbutton status
int sensorStateWater = 0, lastStateWater=0;         // variable for reading the pushbutton status

int timeFoodStart = 0;  //time it takes to eat
int timeFoodEnd = 0;  //time it takes to eat

int timeWaterStart = 0; //time it takes to drink
int timeWaterEnd = 0; //time it takes to drink

void incomingMessage(const MyMessage &message) {
   //rxBlink(1);
   //sendMQTT(message);
   Serial.print("Sender: ");
   Serial.println(message.sender);
} 

void setup() {

  // Initialize gateway at maximum PA level, channel 70 and callback for write operations 
  gw.begin(incomingMessage, AUTO, true, 0); 

  // Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo("DogFoodTracker", "1.0");

  gw.present(ID_FOOD_END, S_LIGHT,"end eating",true);   
  gw.present(ID_WATER_END, S_LIGHT,"end drinking",true);  
  
  Serial.println("Ok!");  
  
  // initialize the LED pin as an output:
  pinMode(LEDPINFOOD, OUTPUT);     
  pinMode(LEDPINWATER, OUTPUT);    
  // initialize the sensor pin as an input:
  pinMode(SENSORPINFOOD, INPUT);     
  digitalWrite(SENSORPINFOOD, HIGH); // turn on the pullup
  pinMode(SENSORPINWATER, INPUT);     
  digitalWrite(SENSORPINWATER, HIGH); // turn on the pullup  
  
}
 
void loop(){
  
  //FOOD
  // read the state of the pushbutton value:
  sensorStateFood = digitalRead(SENSORPINFOOD);
  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorStateFood == LOW) {     
    // turn LED on:
    digitalWrite(LEDPINFOOD, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPINFOOD, LOW); 
  }
  
  //Detecting FOOD eating activity
  if (sensorStateFood && !lastStateFood) { //Food end eating detected
    timeFoodEnd = millis();
    Serial.println("Stopped Eating");
    MyMessage msgfoodend(ID_FOOD_END, V_LIGHT);
    if((timeFoodEnd-timeFoodStart) > 3000) { //only if more than 3 seconds
        gw.send(msgfoodend.set((timeFoodEnd-timeFoodStart))); 
        Serial.print("Drinking time: ");
        Serial.print((timeFoodEnd-timeFoodStart)/1000);
        Serial.println(" secs");        
    }
  } 
  if (!sensorStateFood && lastStateFood) { //Food start eating detected
    timeFoodStart = millis();
  }
  lastStateFood = sensorStateFood;


  //WATER
  // read the state of the pushbutton value:
  sensorStateWater = digitalRead(SENSORPINWATER);
  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorStateWater == LOW) {     
    // turn LED on:
    digitalWrite(LEDPINWATER, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPINWATER, LOW); 
  }
  
  //Detecting FOOD eating activity
  if (sensorStateWater && !lastStateWater) { //Food end eating detected
    timeWaterEnd = millis();
    Serial.println("Stopped Drinking");
    MyMessage msgwaterend(ID_WATER_END, V_LIGHT);
    if(timeWaterEnd-timeWaterStart > 3000) { //only if more 3 seconds
        gw.send(msgwaterend.set((timeWaterEnd-timeWaterStart))); 
        Serial.print("Drinking time: ");
        Serial.print((timeWaterEnd-timeWaterStart)/1000);
        Serial.println(" secs");
    }    
  } 
  if (!sensorStateWater && lastStateWater) { //Food start eating detected
    timeWaterStart = millis();
  }
  lastStateWater = sensorStateWater;    
}
