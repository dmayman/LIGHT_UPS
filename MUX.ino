//  MINI-LIBRARY: MUX v1.0
//  by David Mayman
//  3/9/2013
//
//  IMPLEMENTATION
//  This code drives the multiplexer and allows for easy reading of its values.
//
//  MUX_initialize() - MUST BE CALLED in setup()
//
//  Call readMux(pin) to return value on multiplexer



//Mux control pins
int s0 = 46;
int s1 = 47;
int s2 = 48;
int s3 = 49;

//Mux in "SIG" pin
int SIG_pin = 8;

int total = 13;
int loopCount = 0;
int leader = -1;
int leaderVal = 0;
int cooldown = 100;
int cooldownCount = 100;

void MUX_initialize(){
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
}

int readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(SIG_pin);

  //return the value
  return val;
}
