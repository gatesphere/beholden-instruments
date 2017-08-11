// lamewing (ae modular)
// based on "Harsh Noise Generator" by Kinetik Laboratories
// see original at: 
// http://kinetiklaboratories.blogspot.com/p/harsh-noise-generator.html

class Oscillator {
  int value;  // current output of the oscillator -- 0/1 for digital oscs, 0-255 for analog oscs
  unsigned long before; // micros() time of previous tick
  
  public:
  Oscillator(int a_value = 0) {
    value = a_value;
    before = 0;
  }
  
  void update(int new_value, long new_before) {
    value = new_value;
    before = new_before;
  }
  
  void flip(long new_before) {
    update(!value, new_before);
  }
}


  
    

// MODE LAYOUT:
// 0 - dual osc mode
// 1 - square + noise osc mode
// 2 - pwm noise mode
// 3 - drone mode
// 4 - noise mode

// pins
#define INTENSITY A0
#define DENSITY A1
#define OUT_DUAL 7 // dual osc out (mode 0)
#define OUT_MERZ 8 // square + noise out (mode 1)
#define OUT_WN 9 // white noise out (PWM pin, mode 2)
#define OUT_DRONE 10 // drone out (mode 3)
#define OUT_NOISE 11 // noise out (mode 4)


// Variables
Oscillator osc0_dual = Oscillator();
Oscillator osc1_dual = Oscillator();
Oscillator osc0_merz = Oscillator();
Oscillator osc1_merz = Oscillator();
Oscillator osc0_wn = Oscillator();
Oscillator osc0_drone = Oscillator();
Oscillator osc1_drone = Oscillator();
Oscillator osc0_noise = Oscillator();

int oscOut = 0;                         // Output
int amplitude = 0;
int frequency = 0;
int out_intensity = 0;
int out_density = 0;
long delta = 0;
long intensity = 0;
long density = 0;
long now = 0;
long timeOscillator0 = 0;
long timeOscillator0_noise = 0;
long timeOscillator1 = 0;
int mode_noise = true; // mode 4 noise -- true = making noise, false = silent

void setup() {
  pinMode(INTENSITY, INPUT);
  pinMode(DENSITY, INPUT);
  pinMode(OUT_DUAL, OUTPUT);
  pinMode(OUT_MERZ, OUTPUT);
  pinMode(OUT_WN, OUTPUT);
  pinMode(OUT_DRONE, OUTPUT);
  pinMode(OUT_NOISE, OUTPUT);
  randomSeed(analogRead(A5)); // unconnected pin, hopefully true random seed
  //Serial.begin(115200);
}

void loop()
{
  // basic setup for the rest
  now = micros();

  intensity = analogRead(INTENSITY); // CV handled in hardware via op-amps
  density = analogRead(DENSITY); // CV handled in hardware via op-amps
  
  // mode 0 - dual oscillator mode
  timeOscillator0 = map(intensity, 0, 1023, 50000, 600) + random(1000);
  timeOscillator1 = map(density, 0, 1023, 50000, 600) + random(1000);
  if (now - osc0_dual.before > timeOscillator0) {
    osc0_dual.flip(now);
  }
  if (now - osc1_dual.before > timeOscillator1) {
    osc1_dual.flip(now);
  }
  oscOut = osc0_dual.value ^ osc1_dual.value;
  digitalWrite(OUT_DUAL, oscOut);

  // mode 1 - square and noise oscs
  timeOscillator0 = map(intensity, 0, 1023, 10000, 100);
  timeOscillator1 = map(density, 0, 1023, 10000, 0);
  if (now - osc0_merz.before > timeOscillator0) {
    osc0_merz.flip(now);
  }
  if (now - osc1_merz.before > timeOscillator1) {
    osc1_merz.update(random(0,2), now);
  }
  oscOut = osc0_merz.value ^ osc1_merz.value;
  digitalWrite(OUT_MERZ, oscOut);

  // mode 2 - 'white noise'
  amplitude = map(intensity, 0, 1023, 0, 255);
  frequency = map(density, 0, 1023, 10000, 0);
  if (now - osc0_wn.before > frequency) {
    osc0_wn.update(random(0,2), now);
  }
  oscOut = osc0_wn.value * amplitude;
  analogWrite(OUT_WN, oscOut);
  
  // mode 3 - drone mode
  timeOscillator0 = map(intensity, 0, 1023, 50000, 400);
  timeOscillator1 = map(density, 0, 1023, 50000, 0);
  if (now - osc0_drone.before > timeOscillator0) {
    osc0_drone.flip(now);
  }
  if (now - osc1_drone.before > timeOscillator1) {
    osc1_drone.flip(now);
  }
  oscOut = osc0_drone.value ^ osc1_drone.value;
  digitalWrite(OUT_DRONE, oscOut);
  
  // mode 4 - noise mode
  if (now - osc0_noise.before > timeOscillator0_noise) {
    osc0_noise.flip(now);
    out_density = map(density, 0, 1024, 1014, 10);
    timeOscillator0_noise = random(out_density) * 1000; // this value will need to be tweaked
    if (!osc0_noise.value) {
      timeOscillator0_noise *= 3;
    }
  }
  out_intensity = map(intensity, 0, 1023, 1, 255);
  analogWrite(OUT_NOISE, osc0_noise.value * random(out_intensity));
    
  /*
  // original code here -- the above was just a guess at being functionally equivalent
  if (mode == 22)      //Random noise
  {
    density = map (analogRead(DENSITY), 0, 1024, 1014, 10);
    for (long i = 0; i < (random (density) * 100); i++) {
      intensity = map(analogRead(INTENSITY), 0, 1023, 1, 255);
      int x = random(intensity);
      analogWrite(OUT, x);
    }
    analogWrite(OUT, 0);
    delay(random(density) * 3); // between 3042 and 30 milliseconds
  }
  */
}

