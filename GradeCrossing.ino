/*
    Arduino Grade Crossing Controller
    Copyright (c) 2010 railnerd

    MIT License
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
 */

#define  LED1      5
#define  LED2      6
#define  DEBUGLED  13

#define  DETECTPIN    2
#define  ROADBLOCKPIN 3

#define  FLASHCOUNTER 10


/*
 setup()
 
 Initialize the hardware to use the following I/O pins:
 
 digital pin 2:  Timed Detection Input: Use with an active low open-collector detection circuit on BOTH sides of grade crossing.
 A low on this input will trigger a 10 cycle timer whenever triggered‚Äî if you are only using a single detector,
 be sure to also connect this pin to digital pin 3, the "Road Blocked" detector.
 
 digital pin 3:  Road Blocked Detection Input: "Always Flash" Detection‚Äî whenever this input is low, the flasher will continue to operate
 
 digital pin 5:  Crossing LED
 digital pin 6:  "Other" crossing LED
 
 digital pin 13:  An optional; debug LED; this is usually visible on the Arduino board
 */

void setup()   {                
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(DEBUGLED,OUTPUT);

  analogWrite(LED1,0);
  analogWrite(LED2,0);

  pinMode(DETECTPIN,INPUT);
  pinMode(ROADBLOCKPIN,INPUT);
  digitalWrite(DETECTPIN,HIGH);  // enable pull-ups
  digitalWrite(ROADBLOCKPIN,HIGH);
}



enum {
  kOff = 0,
  kStarting,
  kFlashing,
  kFading
};

int crossingState = kOff;
boolean phase = HIGH;
boolean wasDetected = LOW;
int flashCount;


void loop()                     
{
  boolean detected = !digitalRead(DETECTPIN);
  boolean roadBlocked = !digitalRead(ROADBLOCKPIN);

  switch (crossingState) {

  case kStarting:
    analogWrite(LED1,0);
    for (int i=0;i<64;i++) {
      analogWrite(LED2,min(255,i*32));
      delay(2);
    }
    phase = LOW;
    crossingState = kFlashing;
    flashCount = FLASHCOUNTER;
    break;


  case kFlashing:
    // Make sure we don't stay flashing forever
    flashCount--;

    // Except we need to keep flashing while the road is blocked
    if (roadBlocked)
      flashCount = FLASHCOUNTER/2;

    // If we've been flashing long enough, we need to fade out
    // NOTE: the prior conditional ensures that flashCount is non-zero if the road is blocked!
    if (flashCount==0) {
      crossingState = kFading;
    }

    // Our "normal" flashing state.  We fade in one LED, while fading out the other LED
    // The global variable "phase" is used to define which LED to fade in or out
    for (int i=0;i<64;i++) {
      if (phase) {
        analogWrite(LED1,255-min(255,i*5));
        analogWrite(LED2,min(255,i*32));
      }
      else {
        analogWrite(LED1,min(255,i*32));
        analogWrite(LED2,255-min(255,i*5));
      }
      delay(2);
    }
    phase = !phase;  // swap fade in/out order for next time
    break;


  case kFading:

    // We're no longer flashing, so gradually fade the LEDs out.  Depending on what "phase"
    // we were in, we will fade out the brighter of the two LEDs.
    // NOTE: We're cheating a bit and turning the dimmer LED off completely.
  
    for (int i=0;i<65;i++) {
      if (phase) {
        analogWrite(LED1,255-min(255,i*4));
        analogWrite(LED2,0);
      } 
      else {
        analogWrite(LED1,0);
        analogWrite(LED2,255-min(255,i*4));
      }
      delay(2);
    }

    crossingState = kOff;
    break;


  default:
    // Our default case is to look at the detector inputs and decide what to do
    // If the road is blocked, or the timed detector has been tripped, go ahead and
    // start the flashers.

    if (roadBlocked || (!wasDetected && detected))
    {
      digitalWrite(DEBUGLED,HIGH);    // flash LED so we know we're starting
      crossingState = kStarting;
      delay(10);
    }    

    break;
  }

  wasDetected = detected;

  delay(200);
  digitalWrite(DEBUGLED,LOW);
}


