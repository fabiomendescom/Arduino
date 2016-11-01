/* 
  I have not tested this in ESP8266 but it should work probably. Need to test it.
*/

#define INCLUSION_MODE_TIME 1 // Number of minutes inclusion mode is enabled
#define INCLUSION_MODE_PIN  3 // Digital pin used for inclusion mode button

#define LEDPINWATER 10
#define SENSORPINWATER 9

#define LEDPINFOOD 8
#define SENSORPINFOOD 7

#define ID_FOOD_END 3
#define ID_WATER_END 4

// variables will change:
int sensorStateFood = 0, lastStateFood=0;         // variable for reading the pushbutton status
int sensorStateWater = 0, lastStateWater=0;         // variable for reading the pushbutton status

int timeFoodStart = 0;  //time it takes to eat
int timeFoodEnd = 0;  //time it takes to eat

int timeWaterStart = 0; //time it takes to drink
int timeWaterEnd = 0; //time it takes to drink


void setup() {
  
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
    if((timeFoodEnd-timeFoodStart) > 3000) { //only if more than 3 seconds
        Serial.print("Eating time: ");
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
  
  //Detecting Drinking activity
  if (sensorStateWater && !lastStateWater) { //Food end eating detected
    timeWaterEnd = millis();
    Serial.println("Stopped Drinking");
    if(timeWaterEnd-timeWaterStart > 3000) { //only if more 3 seconds
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
