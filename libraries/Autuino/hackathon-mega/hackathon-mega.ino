/*
  Melody

 Plays a melody

 circuit:
 * 8-ohm speaker on digital pin 3

 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe

This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Tone

 */

int potPin = A5;
int buzzerPin = 4;
int potval=0;
int buttonleft = A6;
int buttonright = A7;
int buttonrightval;
int buttonleftval;
char cardid[50];
char cardidtmp[1];
int currentpage=0;

#include <SPI.h>
#include <MFRC522.h>
#include <Nextion.h>
#include <SoftwareSerial.h>

#define RST_PIN         5           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above

unsigned char matter1[30] = {0x1A,0x4F,0x0D,0xC5,0x1A,0x4F,0x0D,0xC5};
unsigned char matter2[30] = {0x56,0x1F,0x1D,0x7E};
//unsigned char matter3[30] = {0x04,0xA4,0x28,0xD2,0xD8,0x38,0x80};
//unsigned char matter4[30] = {0x04,0xC5,0x28,0xD2,0xD8,0x38,0x80};


long timecardread = millis();
long timerstart;
bool timerstarted=false;
int mattertimerstarted=0;
char currentclient[40]="";
char currentmatter[40]="";

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

  SoftwareSerial nextion(2, 3);// Nextion TX to pin 2 and RX to pin 3 of Arduino
  Nextion myNextion(nextion, 9600);
    
void setup() {
  Serial1.begin(9600);
  pinMode(potPin,INPUT);
  pinMode(buttonleft,INPUT);
  pinMode(buttonright,INPUT);
  Serial.begin(9600);
  myNextion.init();  
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card  

   //myNextion.sendCommand("page 0");
   currentpage=0;
   //delay(6000);
   myNextion.sendCommand("page 1");
   currentpage=1;
}

void loop() {
  myNextion.setComponentText("client", currentclient);
  myNextion.setComponentText("matter", currentmatter); 
           
  if(timerstarted) {
      int seconds = ((millis()-timerstart)/1000)%60;
      int minutes = (((millis()-timerstart)/1000)/60)%60;
      int hours = (((millis()-timerstart)/1000)/60/60)%60;

      String secondstring = "0" + String(seconds);
      secondstring = secondstring.substring(secondstring.length()-2);
      String minutestring = "0" + String(minutes);
      minutestring = minutestring.substring(minutestring.length()-2);
      String hourstring = "0" + String(hours);
      hourstring = hourstring.substring(hourstring.length()-2);
                  
      myNextion.setComponentText("second", secondstring);    
      myNextion.setComponentText("minute", String(minutestring));  
      myNextion.setComponentText("hour", String(hourstring));  
  }
  // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    
  } else {
    if((millis() - timecardread) > 2000) {
        if(currentpage!=1) {
          myNextion.sendCommand("page 1");
          currentpage=1;
        }
        timecardread = millis();
        tone(buzzerPin, 600, 500);

        //for (byte i = 0; i < mfrc522.uid.size; i++) {
        //  Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //  Serial.print(mfrc522.uid.uidByte[i], HEX);
        //} 
        //Serial.println();
        if(memcmp(mfrc522.uid.uidByte,matter1,mfrc522.uid.size)==0) {
          memcpy(currentmatter,"MATTER1",7);
          memcpy(currentclient,"CLIENT1",7);
          if(timerstarted) {
            timerstarted=false;            
          } else {
            timerstarted=true;
            timerstart=millis();
            mattertimerstarted=1;
          }          
        }
        if(memcmp(mfrc522.uid.uidByte,matter2,mfrc522.uid.size)==0) {
          memcpy(currentmatter,"MATTER2",7);
          memcpy(currentclient,"CLIENT1",7);             
          if(timerstarted) {
            timerstarted=false;            
          } else {
            timerstarted=true;
            timerstart=millis();
            mattertimerstarted=2;
          }                        
        }        
    } else {
         
    }
   }
  
    potval = analogRead(potPin);

    buttonleftval = analogRead(buttonleft);
    buttonrightval = analogRead(buttonright);
    myNextion.setComponentText("button1", String(buttonleftval));
    myNextion.setComponentText("button2", String(buttonrightval));    
    myNextion.setComponentText("potentiometer", String(potval));     

    if(currentpage==2) {
      myNextion.setComponentText("type", "Vacation Request");
      myNextion.setComponentText("user", "Fabio");
    }
    
    if(buttonleftval>1000) {
      if(currentpage==1) {
         currentpage = 2;
         myNextion.sendCommand("page 2");
      } else if(currentpage==2) {
        myNextion.setComponentText("status", "Processing aproval");
        //make the webcall to approve here
        Serial1.println("1");
        
        delay(3000);
        myNextion.setComponentText("status", "Approved");
        delay(2000);
        myNextion.sendCommand("page 1");
        currentpage = 1;
      }
    }
    if(buttonrightval>1000) {
      if(currentpage==1) {
         currentpage = 2;
         myNextion.sendCommand("page 2");
      } else if(currentpage==2) {
        myNextion.setComponentText("status", "Request Denied");
        //make the webcall to deny here
        
        delay(2000);
        myNextion.sendCommand("page 1");
        currentpage=1;
      }
    }
   
    
}
