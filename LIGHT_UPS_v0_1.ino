//  LIGHT_UPS v0.1
//  by David Mayman
//  3/12/2013
//
//  This code drives the custom build drum LIGHT_UPS machine.
//  
//  It requires the following libraries:
//  StackArray -- Timer 3 -- SPI
//
//  It requires the following mini-libraries:
//  BAM -- MUX -- MultiButtonDebounce
//


/*----------------------------------*/
/*-----------DECLARATION------------*/
/*----------------------------------*/

#define MAX_FADE_TIME 5000 //in ms -- this value is changeable
#define RESOLUTION 10 // in ms -- how often the lights are changed
#define MAXSTACKLENGTH (MAX_FADE_TIME / RESOLUTION)

//stack definitions
byte BD_rgb_stack[MAXSTACKLENGTH][3];
byte SD_rgb_stack[MAXSTACKLENGTH][3];
byte ST_rgb_stack[MAXSTACKLENGTH][3];
byte FT_rgb_stack[MAXSTACKLENGTH][3];

int BD_index=0;
int SD_index=0;
int ST_index=0;
int FT_index=0;

int BD_total=0;
int SD_total=0;
int ST_total=0;
int FT_total=0;

//declare build function prototypes (because of referenced attribute)
void build_simple(byte (&drumArray)[MAXSTACKLENGTH][3], int &index, int &total, int reading);


//declare knobs analog input pins
int knobPin[] = {A7, A6, A5, A4};
int knobs[4];

//declare drum input pins
int drumPin[] = {A3, A2, A1, A0};

//declare drum output pins
int BD_out[] = {11,12,13}; //rgb
int SD_out[] = {8,9,10}; //rgb
int ST_out[] = {5,6,7}; //rgb
int FT_out[] = {2,3,4}; //rgb

//drum trigger threshold
int hitThreshold = 400;

//attack, release
#define ATTACK_MAX 1500
#define RELEASE_MAX 3500
int attk = 0;
int rls = 0;
int attkVal = attk/RESOLUTION;
int rlsVal = rls/RESOLUTION;

//for MultiButtonDebounce mini-library:
byte buttons[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 50}; // the analog 0-5 pins are also known as 14-19
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
// we will track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];


/*----------------------------------*/
/*--------INITIALIZATION------------*/
/*----------------------------------*/

void setup(){
 Serial.begin(9600);
 
 MUX_initialize();
 BAM_initialize();
 
 for (int i=0; i<3; i++){
   pinMode(BD_out[i], OUTPUT);
   pinMode(SD_out[i], OUTPUT);
   pinMode(ST_out[i], OUTPUT);
   pinMode(FT_out[i], OUTPUT);
 }
 
 for (int j=0; j<27; j=j+3){
   setChannel(j,255);
   setChannel(j+1,255);
   setChannel(j+2,255);
   delay(5);
  setChannel(j,0);
   setChannel(j+1,0);
   setChannel(j+2,0);
   delay(5);  
 }
 
 for (int j=22; j<45; j++){
   digitalWrite(j,HIGH);
   delay(5);
 }
 
 setAllChannelsTo(0);
}

void check_switches_test(){
  int temp = readMux(1);
 if (temp > 1){
  Serial.println(temp);
 } 
}

void loop(){
  
  
/*----------------------------------*/
/*--------------THE ASK-------------*/
/*----------------------------------*/
debug();
check_switches();
check_knobs();
//analyze();
check_drums();
execute();


  
} // end loop()



void check_knobs(){
  for (int i=0; i<4; i++){
    knobs[i] = analogRead(knobPin[i]);
  }
  
  attk = mapf(knobs[0], 0, 1024, 0, ATTACK_MAX);
  rls = mapf(knobs[1], 0, 1024, 1, RELEASE_MAX);

  
   attkVal = ceil(float(attk)/RESOLUTION);
   rlsVal = ceil(float(rls)/RESOLUTION);
   

}

void check_drums(){
  for (byte i = 0; i<4; i++){
    int reading = analogRead(drumPin[i]);
    
     if (reading > hitThreshold){
       Serial.print(reading);
       fire(i, reading);
     }
  }
  //DEBUG
  if (justpressed[1]){
    fire(1,1024);
  }
}
int checkIndex(int &index){ 
  if (index >= MAXSTACKLENGTH){
    index=MAXSTACKLENGTH;
  } else {
    index++;
  }
}
void showDrumArray(byte (&array)[MAXSTACKLENGTH][3]){ //debug
   Serial.println(SD_total);
  for (int i=0; i<SD_total; i++){
     for (int j=0; j<1; j++){
    if (j==0){ Serial.print("R");}
    else if (j==1){ Serial.print("G");}
    else if (j==2){ Serial.print("B");}
    Serial.print(i);
    Serial.print(": ");
    Serial.println(array[i][j]);
     }
  }
}

void fire(byte drum, int reading){
  Serial.print("Fire: ");
  Serial.println(drum);
    //if switches are active...etc.
     if (drum==0){
      build_simple(BD_rgb_stack, BD_index, BD_total, reading);
    } else if (drum==1){
      build_simple(SD_rgb_stack, SD_index, SD_total, reading);
    } else if (drum==2){
      build_simple(ST_rgb_stack, ST_index, ST_total, reading);
    } else if (drum==3){
      build_simple(FT_rgb_stack, FT_index, FT_total, reading);
    }
    
}


void build_simple(byte (&drumArray)[MAXSTACKLENGTH][3], int &index, int &total, int reading){
  //builds a simple stack
  Serial.println("build_simple");
  //clear old values
  int oldIndex = index;
  int oldTotal = total;
  index = 0;
  //memset(drumArray, 0, sizeof(drumArray[0][0]) * MAXSTACKLENGTH * 3);
  
  Serial.print("oldIndex: ");
  Serial.println(oldIndex);
  
  //start building
  float A=0;
  float B=255;
  float rando[] = {float(random(0,100))/100, float(random(0,100))/100, float(random(0,100))/100};
  
  /*while (rando[0]+rando[1]+rando[2] < 1.5){
    rando[0] = float(random(0,100))/100;
    rando[1] = float(random(0,100))/100;
    rando[2] = float(random(0,100))/100;
  }*/
  
  total = attkVal + rlsVal;
  
  for (int i=0; i<=(attkVal + rlsVal); i++){  
    float x;
    float z[3]; //attack add variable
    int attkAdd[3];
    //attack
    if (attkVal > 0 && i<=attkVal){
      float v = i / float(attkVal);
      v = v*v*v;
      x = (B * v) + (A * (1 - v));
      
      for (int j=0; j<3; j++){
        float v2 = (oldIndex+i) / float(oldTotal);
        v = 1 - (1 - v) * (1 - v)* (1 - v)* (1 - v);
        z[0] = (A * v) + (drumArray[oldIndex][j] * (1 - v));
         
        
        
      }
      
      attkAdd[0] = drumArray[oldIndex+i][0];
      attkAdd[1] = drumArray[oldIndex+i][1];
      attkAdd[2] = drumArray[oldIndex+i][2];
      
      
    //  Serial.print("attack: ");
    //  Serial.println(x);
    }
    
    //release
    
    if (rlsVal > 0 && i>attkVal){
      float v = (i-attkVal) / float(rlsVal);
      v = 1 - (1 - v) * (1 - v)* (1 - v)* (1 - v);
      x = (A * v) + (B * (1 - v));
      
      if (rls==1  && i>attkVal){ x = 0; } //creates mandatory end 0 value
    }
    
    drumArray[i][0] = int((float(x)*rando[0])+attkAdd[0]); //R
    drumArray[i][1] = int((float(x)*rando[1])+attkAdd[1]); //G
    drumArray[i][2] = int((float(x)*rando[2])+attkAdd[2]); //B
  }
    //end stack 
}


void execute(){
  //execute one value of each stack
  static long lastExecuted;
  
  //check timer
  if (millis() < lastExecuted) {
     // we wrapped around, lets just try again
     lastExecuted = millis();
  }
  
  if ((lastExecuted + RESOLUTION) > millis()){
    //not enough time has passed to execute again
    return;
  }
  
    lastExecuted = millis(); //reset timer
  
  //execute
 
 for (int i=0; i<3; i++){ 
  analogWrite(BD_out[i], BD_rgb_stack[BD_index][i]);
  analogWrite(SD_out[i], SD_rgb_stack[SD_index][i]);
  analogWrite(ST_out[i], ST_rgb_stack[ST_index][i]);
  analogWrite(FT_out[i], FT_rgb_stack[FT_index][i]);

  //DEBUG
  //Serial.println(BD_rgb_stack[BD_index][i]);
  setChannel(i+3, BD_rgb_stack[BD_index][i]);
}
  if (BD_index >= BD_total){
    BD_index = BD_total;
  } else {
    BD_index++;
  }
  if (SD_index >= SD_total){
    SD_index = SD_total;
  } else {
    SD_index++;
  }
  if (ST_index >= ST_total){
    ST_index = ST_total;
  } else {
    ST_index++;
  }
  if (FT_index >= FT_total){
    FT_index = FT_total;
  } else {
    FT_index++;
  }
  
 // Serial.print("BD_index: ");
 // Serial.println(BD_index);
  

}



void debug(){ //debug actions
  
    if (justreleased[2]){
    showDrumArray(SD_rgb_stack);
  }
  
  if (justreleased[0]){ //repeat stack
    BD_index=0;
    SD_index=0;
    ST_index=0;
    FT_index=0;
  }
  
  if (justpressed[3]){
  float totalTime = (((attk+rls)/2)*MAX_FADE_TIME);
  int attkTime = int(totalTime*(1/(attk+rls)*attk));
  int rlsTime = int(totalTime*(1/(attk+rls)*rls));
      Serial.print("attk: ");
     Serial.println(attk);
     Serial.print("rls: ");
     Serial.println(rls);
      Serial.print("totaltime: ");
     Serial.println(totalTime);
       Serial.print("attacktime: ");
     Serial.println(attkTime);
    Serial.print("releasetime: ");
     Serial.println(rlsTime);
}
  
}
