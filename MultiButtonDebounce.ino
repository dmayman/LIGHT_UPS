//  MINI-LIBRARY: MultiButtonDebounce v1.0
//  by David Mayman
//  3/11/2013
//
//  IMPLEMENTATION
//  Call check_switches() to assign appropriate boolean values to the arrays:
//  pressed[], justpressed[], justreleased[]

#define DEBOUNCE 5  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

int thresh = 800;
int lowThresh = 300;

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lastReleased;
  int tempThresh;
  byte index;
  pinMode(50, INPUT);
  
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       // when we start, we clear out the "just" indicators
    justreleased[index] = 0;
    if (buttons[index] > 15){ // not within MUX analog ins
        if (digitalRead(buttons[index]) == HIGH){   // read the button
         currentstate[index] = HIGH;
      } else {
        currentstate[index] = LOW;
      }
    } else { // is within MUX analog ins
      if (index==0){ // adjust threshold for channel 0
        tempThresh = lowThresh;
      } else {
        tempThresh = thresh;
      }
      if (readMux(buttons[index]) > tempThresh){   // read the button
        //Serial.print("raw hit ");
         currentstate[index] = HIGH;
      } else {
        currentstate[index] = LOW;
      }
    }
    
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == HIGH)) {
          // just pressed
          justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == LOW)) {
          // just released
          if (lastReleased + DEBOUNCE < millis()){
            justreleased[index] = 1;
          }
          lastReleased = millis();
      }
      pressed[index] = currentstate[index];
    }
    //Serial.println(pressed[i], DEC);
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}


/*void loop() {
  check_switches();      // when we check the switches we'll get the current state
  
  for (byte i = 0; i < NUMBUTTONS; i++)  {
     if (justpressed[i]) {
      Serial.print(buttons[i], DEC);
      Serial.println(" Just pressed");       
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (justreleased[i]) {
      Serial.print(buttons[i], DEC);
      Serial.println(" Just released");
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    if (pressed[i]) {
      Serial.print(buttons[i], DEC);
      Serial.println(" pressed");   
      // is the button pressed down at this moment
    }
  }
 */ 
  
  


