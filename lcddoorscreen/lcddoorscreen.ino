// Draw Circles - Demonstrate drawCircle and fillCircle APIs
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
// Draw Texts - Demonstrate drawChar and drawString
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

//THIS IS FOR ARDUINO ONLY. NOT ESP8266
//YOU GOT THE LIBRARIES (TFT) FROM https://github.com/Seeed-Studio/TFT_Touch_Shield_V1/blob/master/examples/text/text.ino

#include <TFT.h>

long fontcolor = BLACK;

int incomingByte = 0;

void setContent(String doorlocked, String doorclosed, String dooropen, int outtemp, int intemp) {
  char outtemps[50];
  char intemps[50];
  
  if(doorlocked=="1") {
    Tft.fillRectangle(0,0,400,400,RED);
    fontcolor=WHITE;
  } else if(doorlocked=="0" && doorclosed=="1"){
    Tft.fillRectangle(0,0,400,400,YELLOW);
    fontcolor=BLACK;
  } else if (dooropen=="1") {
    Tft.fillRectangle(0,0,400,400,GREEN);
    fontcolor=BLACK;    
  }
  Tft.drawString("DOOR",7,10,7,fontcolor);
  if(doorlocked=="1") {
    Tft.drawString("LOCKED",0,85,5,fontcolor);
  } else if (doorclosed=="1") {
    Tft.drawString("CLOSED",0,85,5,fontcolor);
  } else if (dooropen=="1") {
    Tft.drawString("OPEN",40,85,5,fontcolor);    
  }
  Tft.drawString("TEMP.(F)",0,170,4,fontcolor);
  Tft.drawString("Outside  Inside",0,220,2,fontcolor);
  if(outtemp<99) {
    snprintf (outtemps, 50, "%d", outtemp);
    Tft.drawString(outtemps,0,260,6,fontcolor);
  } else {
    if(outtemp>999) {
       Tft.drawString("-",0,260,6,fontcolor);
    } else {
      snprintf (outtemps, 50, "%d", outtemp);
      Tft.drawString(outtemps,0,270,4,fontcolor);      
    }
  }
  if(intemp<99) {
    snprintf (intemps, 50, "%d", intemp);
    Tft.drawString(intemps,140,260,6,fontcolor); 
  } else {
    if(intemp>999) {
       Tft.drawString("-",140,260,6,fontcolor);
    } else {
      snprintf (intemps, 50, "%d", intemp);
      Tft.drawString(intemps,140,270,4,fontcolor);       
    }    
  }
}

void setup()
{
  Serial.begin(9600);  
  Serial.setTimeout(200);
  Tft.init();  //init TFT library
  setContent("0", "0", "1", 1000, 1000);
  Serial.println("");
  Serial.println("");
}

String inData; 
int indextempout;
int indextempin;
int indexopen;
int indexclosed;
int indexlocked;

String stringtempout;
String stringtempin;
String stringopen;
String stringclosed;
String stringlocked;

int inttempout;
int inttempin;

void loop()
{
  /*
  if(Serial.available() > 0) {
     inData = Serial.readString();
     Serial.print(inData);
     if(inData.startsWith("TX=")) {
        Serial.print(inData);
     }
  }
  */
 
    while (Serial.available() > 0)
    {
        char received = Serial.read();
        inData += received; 
        Serial.print(received);
        // Process message when new line character is received
        if (received == '\n')
        {
            inData.trim();
            Serial.println(inData);
            if(inData.startsWith("TX=")) {
               inData = inData.substring(3);
               indextempout = inData.indexOf(',');
               indextempin = inData.indexOf(',',indextempout+1);
               indexopen = inData.indexOf(',',indextempin+1);
               indexclosed = inData.indexOf(',',indexopen+1);
               indexlocked = inData.indexOf(',',indexclosed+1);

               stringtempout = inData.substring(0,indextempout);
               stringtempin = inData.substring(indextempout+1,indextempin);
               stringopen = inData.substring(indextempin+1,indexopen);
               stringclosed = inData.substring(indexopen+1,indexclosed);
               stringlocked = inData.substring(indexclosed+1,indexlocked);

               if(stringtempout=="") {
                  inttempout = 1000;
               } else {
                  inttempout = stringtempout.toInt();
               }
               if(stringtempin=="") {
                  inttempin = 1000;
               } else {
                  inttempin = stringtempin.toInt();
               }

               setContent(stringlocked,stringclosed,stringopen,inttempout,inttempin);
               
               Serial.print("Temp out: " );
               Serial.println(stringtempout);
               Serial.print("Temp in: ");
               Serial.println(stringtempin);
               Serial.print("open: ");
               Serial.println(stringopen);
               Serial.print("closed: ");
               Serial.println(stringclosed);
               Serial.print("locked: ");
               Serial.println(stringlocked);
               Serial.println("");
            }

            inData = ""; // Clear received buffer
        }
    }
  
}
