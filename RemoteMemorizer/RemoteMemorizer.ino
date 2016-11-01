/*
 * IRrecord: record and play back IR signals as a minimal 
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * An IR LED must be connected to the output PWM pin 3.
 * A button must be connected to the input BUTTON_PIN; this is the
 * send button.
 * A visible LED can be connected to STATUS_PIN to provide status.
 *
 * The logic is:
 * If the button is pressed, send the IR code.
 * If an IR code is received, record it.
 *
 * Version 0.11 September, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

/*
 * EPSON PROJECTOR
 * POWER: NEC C1AA09F6
 * 
 * TV SAMSUNG
 * UP: m4350 s4550 m450 s1750 m450 s1800 m350 s1850 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m450 s1750 m400 s1800 m400 s750 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m400 s1800 m450 s1750 m450 s650 m450 s650 m450 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m400 s1800 m450 s1750 m450 s1800 m350 s1850 m400
 * DOWN: m4350 s4550 m450 s1800 m450 s1750 m400 s1800 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m450 s1750 m400 s1800 m450 s1800 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m400 s1800 m450 s650 m450 s1800 m350 s750 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m450 s650 m450 s1750 m450 s1750 m500 s1750 m450 s1750 m450
 * POWER: m4300 s4600 m400 s1800 m400 s1850 m350 s1850 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m400 s1800 m400 s1850 m400 s700 m350 s750 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m400 s700 m400 s700 m400 s700 m450 s650 m400 s750 m350 s750 m350 s1850 m400 s700 m400 s1800 m400 s1800 m400 s1800 m400 s1800 m400 s1850 m350 s1850 m400
 * MUTE: m4300 s4550 m450 s1800 m400 s1800 m400 s1800 m400 s700 m400 s700 m450 s650 m450 s650 m400 s700 m400 s1850 m400 s1800 m400 s1800 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s1800 m400 s1850 m350 s1850 m400 s1800 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s700 m400 s750 m400 s1800 m400 s1800 m400 s1800 m400 s1800 m400
 *
 * HDMI SWITCHER
 * A1 (Cable/TV): NEC FF9867
 * A2 (Roku/TV): NEC FFD827
 * B1 (Cable/Screen): NEC FF8877
 * B2 (Roku/Screen): NEC FFA857
 * 
 * ELIANA LIGHT
 * WHITE UP: NEC: FFA05F
 * WHITE DOWN: NEC FF20DF
 * OFF: NEC FF609F
 * ON: NEC FFE01F
 * R: NEC FF906F
 * G: NEC: FF10EF
 * B: NEC: FF50AF
 * W: NEC: FFD02F
 * 
 */


#include <IRremote.h>

int RECV_PIN = 11;
int BUTTON_PIN = 12;
int STATUS_PIN = 13;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state

// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) {
  codeType = results->decode_type;
  int count = results->rawlen;
  if (codeType == UNKNOWN) {
    Serial.println("Received unknown code, saving as raw");
    codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      } 
      else {
        // Space
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(rawCodes[i - 1], DEC);
    }
    Serial.println("");
  }
  else {
    if (codeType == NEC) {
      Serial.print("Received NEC: ");
      if (results->value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        Serial.println("repeat; ignoring.");
        return;
      }
    } 
    else if (codeType == SONY) {
      Serial.print("Received SONY: ");
    } 
    else if (codeType == RC5) {
      Serial.print("Received RC5: ");
    } 
    else if (codeType == RC6) {
      Serial.print("Received RC6: ");
    } 
    else {
      Serial.print("Unexpected codeType ");
      Serial.print(codeType, DEC);
      Serial.println("");
    }
    Serial.println(results->value, HEX);
    codeValue = results->value;
    codeLen = results->bits;
  }
}

void sendCode(int repeat) {
  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
      Serial.println("Sent NEC repeat");
    } 
    else {
      irsend.sendNEC(codeValue, codeLen);
      Serial.print("Sent NEC ");
      Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
    Serial.print("Sent Sony ");
    Serial.println(codeValue, HEX);
  } 
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      Serial.print("Sent RC5 ");
      Serial.println(codeValue, HEX);
      irsend.sendRC5(codeValue, codeLen);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
      Serial.print("Sent RC6 ");
      Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    irsend.sendRaw(rawCodes, codeLen, 38);
    Serial.println("Sent raw");
  }
}


void loop() {

 if (irrecv.decode(&results)) {
    storeCode(&results);
    irrecv.resume(); // resume receiver
    Serial.println("SENDING in 1 SECS");
    //delay(4000);
    //sendCode(0);
    //delay(1000);
    //sendCode(0);
    //delay(1000);    
    //Serial.println("SENT");
    irrecv.enableIRIn(); // Re-enable receiver
  }

}
