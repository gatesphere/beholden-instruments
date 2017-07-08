//Dualism -- digital half

// Circuit:
// Three pots wired into A0, A1, A2
// Output on 9, through 220R resistor and  2.2uF cap (match what is on Lamewing), into dry/wet mixpot

#define ALPHA A0
#define BETA A1
#define DELTA A2
#define OUT 9

int osc0=0;
int osc1=0;
int osc2=0;
long timeOsc0=0;
long timeOsc1=0;
long timeOsc2=0;
long beforeOsc0=0;
long beforeOsc1=0;
long beforeOsc2=0;
long now=0;
int out=0;

void setup() {
  pinMode(ALPHA, INPUT);
  pinMode(BETA, INPUT);
  pinMode(DELTA, INPUT);
  pinMode(OUT, OUTPUT);
  randomSeed(analogRead(A5));
  Serial.begin(9600);
}

void loop() {
  now = micros();
  //Serial.println(now);
  timeOsc0 = map(analogRead(ALPHA), 0, 1023, 80000, 1000) + random(500);
  timeOsc1 = map(analogRead(BETA), 0, 1023, 50000, 600) + random(1000);
  timeOsc2 = map(analogRead(DELTA), 0, 1023, 40000, 300) + random(2000);
  if (now - beforeOsc0 > timeOsc0) {
    beforeOsc0 = now;
    osc0 = !osc0;
    //Serial.println(timeOsc0);
    //Serial.println("Flipping osc0");
  }
  if (now - beforeOsc1 > timeOsc1) {
    beforeOsc1 = now;
    osc1 = !osc1;
    //Serial.println("Flipping osc1");
  }
  if (now - beforeOsc2 > timeOsc2) {
    beforeOsc2 = now;
    osc2 = !osc2;
    //Serial.println("Flipping osc2");
  }
  
  //out = (osc0 ^ osc1) & osc2;
  //out = (osc0 & osc1) ^ osc2;
  //out = (osc0 & osc1) ^ (!osc0 & osc2);
  //out = (osc0 | !osc1) & (!osc0 ^ osc2);
  out = (osc0 | !osc1) & (osc1 ^ !osc2) | (osc2 ^ !osc0); // winner so far
  //out = (osc0 | !osc1) & (osc1 ^ !osc2) & (osc2 ^ !osc0);
  //out = (osc0 ^ osc2) ^ (osc1 & !osc0) | (!osc2 ^ !osc1);
  //out = osc0 ^ osc1;
  digitalWrite(OUT,out);
  //Serial.println(osc0);
  //Serial.println(osc1);
  //Serial.println(osc2);
  //Serial.println(out);
  //Serial.println("===");
}
