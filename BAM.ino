//  MINI-LIBRARY: BAM v1.0
//  by David Mayman
//  3/9/2013
//
//  Lot of code from:
//  Created 22 May 2009
//  Created 23 Mar 2010
//  by Tom Igoe
//
//  This code drives leds using daisy chained 595 shiftregisters.
//
//  IMPLEMENTATION
//
//  BAM_Initialize() - MUST BE CALLED in setup()
//
//  setAllChannelsTo(val) does just that
//
//  setChannel(channel, val) does just that
//
//  setChannels(channels[], length) - this accepts an array of all the channels
//  (or of the first however many) and the array length and sets those channels


#include <SPI.h>
#include <TimerThree.h>


//
// Pins on arduino mega
//
//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 53;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 52;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 51;

//
// constants
//
#define PWM_FREQ 100 // hz
#define PWM_STEPS 256 // PWM resolution - set to 256 when using bam
#define BAM_STEPS 8 // how many times BAM updates are done each cycle
#define TICKER_STEP 256/PWM_STEPS
#define NUM_SHIFT_REGS 4  
#define NUM_CHANNELS NUM_SHIFT_REGS*8


//
// Global variables
//
// holds the current pwm values of pins to be sent by interrupt
volatile unsigned char pwmValues[NUM_CHANNELS];
volatile byte bamLookup[BAM_STEPS*NUM_SHIFT_REGS]; // precalculate the bytes to send every time the PWM values change
volatile byte pwmLookup[PWM_STEPS*NUM_SHIFT_REGS];// precalculate the bytes to send to 595 for pwm


/*
  Precalculate the bytes to send for each time slice. Call everytime you update or change
  pwmvalues.

*/
void precalcBamBytes() {
  byte bytesToSend[NUM_SHIFT_REGS];
  
  for(int slice=0;slice<BAM_STEPS;slice++) {
    unsigned char sliceMask = (1 << slice);   
    // generate one byte per register
    for(unsigned char regNo = 0 ;regNo<NUM_SHIFT_REGS;regNo++) {
      bytesToSend[regNo]  = 0; 
      // loop bits of each register
      for(unsigned char ch = 0;ch<8;ch++){
        // test if the pwm value has the current slicebit 1
        if( (pwmValues[regNo*8+ch] & sliceMask) == sliceMask) {
          // turn on channel 0-7
          bytesToSend[regNo] |= (1 << ch);
        }
        
      }
      bamLookup[slice*NUM_SHIFT_REGS + regNo] = bytesToSend[regNo];
    }
    
  }

}


// the current data that has last been sent to shift registers
//byte *shiftRegValues;

// interrupt counter
volatile byte ticker = 0;


void BAM_initialize() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
    
  
  // serial communication to 595s
  SPI.begin(); 
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  
  // Activate the interrupt
  // calculate how many times per sec the ISR ISR is called
  // to get the requested pwm frequency
  Timer3.initialize(1000000/PWM_FREQ/PWM_STEPS); // Timer for updating pwm pins
  Timer3.attachInterrupt(iUpdateBAM3);
  
}


/*
  Sends a byte array using SPI to 595 shift registers
*/
void sendSPI(volatile byte *valuesToSend, int from, int to) {
  // latch off
  digitalWrite(latchPin, LOW);   
  // send bytes the last register first
  for(int i = to-1; i>=from;i--) {
    SPI.transfer(valuesToSend[i]);
  }
   // latch on
  digitalWrite(latchPin, HIGH);
}


void setAllChannelsTo(byte duty) {
  for(int i = 0;i<NUM_CHANNELS;i++) {
    pwmValues[i] = duty;
  }
  precalcBamBytes();
}

void setChannel(int ch,byte duty) {
  pwmValues[ch] = duty;
  precalcBamBytes();
}
void setChannels(byte channelsArray[], int n) {
  for (int i=0; i<n; i++){
    pwmValues[i] = channelsArray[i];
  }
  precalcBamBytes();
}



/*
  
  Bit Angle modulation. Faster version that uses precalculated data for each slice.
  Mighty fast compared to the other version. 
  
  This version also divides
  the bits 6 and 7 to 32 tick slices to smoothen the transitions over 126->127
  

*/
void iUpdateBAM3() {
  // update ticker and reset if rolls over
  ticker++;
  if(ticker > PWM_STEPS) {
    ticker = 0;
  }
  
  // find out timeslice startpoint we are at
  unsigned char slice;
  if(ticker == 0) {
    slice = 0;
  } 
  else if(ticker == 1) {
    slice = 1;
  } 
  else if(ticker == 3) {
    slice = 2;
  } 
  else if(ticker == 7) {
    // B3 - 8 long
    slice = 3;
  }
  else if(ticker == 15) {
    // B5 - 32 long
    slice = 5;
  }
  else if(ticker == 47) {
    // B4 - 16
    slice = 4;
  }
  else if(ticker == 63) {
    // B7A - 32 long
    slice = 7;
  }
  else if(ticker == 95) {
    // B6A - 32 long
    slice = 6;
  }
  else if(ticker == 127) {
    // B7B - 32 long
    slice = 7;
  } else if(ticker == 159) {
    // B6B - 32 long
    slice = 6;
  }else if(ticker == 191) {
    // B7C - 32 long
    slice = 7;
  } else if(ticker == 223) {
    // B7D - 32 long
    slice = 7;
  }
  
  else {
    // no actions required
    return;
  }
  
  // update registers. The lookup table is recalculated always when setting pwm values.
  sendSPI(bamLookup,slice*NUM_SHIFT_REGS,slice*NUM_SHIFT_REGS + NUM_SHIFT_REGS);
  

}

