// lamewing (ae modular)
// based on "Harsh Noise Generator" by Kinetik Laboratories
// see original at: 
// http://kinetiklaboratories.blogspot.com/p/harsh-noise-generator.html

// MODE LAYOUT:
// 0 - dual osc mode
// 1 - square + noise osc mode
// 2 - pwm noise mode
// 3 - drone mode
// 4 - noise mode

// pins
#define INTENSITY A0
#define DENSITY A1
#define INTENSITY_CV A2  //pulled down to ground
#define DENSITY_CV A3    //pulled down to ground
#define OUT_DUAL 7 // dual osc out (mode 0)
#define OUT_MERZ 8 // square + noise out (mode 1)
#define OUT_WN 9 // white noise out (PWM pin, mode 2)
#define OUT_DRONE 10 // drone out (mode 3)
#define OUT_NOISE 11 // noise out (mode 4)


// Variables
int osc0_dual = 0;                      // First oscillator
int osc0_merz = 0;
int osc0_wn = 0;
int osc0_drone = 0;
int osc0_noise = 0;
int osc1_dual = 0;                      // Second oscillator
int osc1_merz = 0;
int osc1_drone = 0;
int oscOut = 0;                         // Output
int amplitude = 0;
int frequency = 0;
int out_intensity = 0;
int out_density = 0;
int delta = 0;
int intensity_pot = 0;
int intensity_cv = 0;
int density_pot = 0;
int density_cv = 0;
long intensity = 0;
long density = 0;
long now = 0;
long before0_dual = 0;
long before0_merz = 0;
long before0_wn = 0;
long before0_drone = 0;
long before0_noise = 0;
long before1_dual = 0;
long before1_merz = 0;
long before1_drone = 0;
long timeOscillator0 = 0;
long timeOscillator0_noise = 0;
long timeOscillator1 = 0;
int mode = true; // mode 4 noise -- true = making noise, false = silent


void setup() {
  pinMode(INTENSITY, INPUT);
  pinMode(INTENSITY_CV, INPUT);
  pinMode(DENSITY, INPUT);
  pinMode(DENSITY_CV, INPUT);
  pinMode(OUT_DUAL, OUTPUT);
  pinMode(OUT_MERZ, OUTPUT);
  pinMode(OUT_WN, OUTPUT);
  pinMode(OUT_DRONE, OUTPUT);
  pinMode(OUT_NOISE, OUTPUT);
  randomSeed(analogRead(A5)); // unconnected pin, hopefully true random seed 
}

void loop()
{
  // basic setup for the rest
  now = micros();
  
  intensity_pot = analogRead(INTENSITY);
  intensity_cv = analogRead(INTENSITY_CV);
  intensity_cv > 0 ? intensity = intensity_cv * intensity_pot / 1023 : intensity = intensity_pot;
  
  density_pot = analogRead(DENSITY);
  density_cv = analogRead(DENSITY);
  density_cv > 0 ? density = density_cv * density_pot / 1023 : density = density_pot;
  
  // mode 0 - dual oscillator mode
  timeOscillator0 = map(intensity, 0, 1023, 50000, 6000) + random(1000);
  timeOscillator1 = map(density, 0, 1023, 50000, 6000) + random(1000);
  if (now - before0_dual > timeOscillator0) {
    before0_dual = now;
    osc0_dual = !osc0_dual;
  }
  if (now - before1_dual > timeOscillator1) {
    before1_dual = now;
    osc1_dual = !osc1_dual;
  }
  oscOut = osc0_dual ^ osc1_dual;
  digitalWrite(OUT_DUAL, oscOut);
  
  // mode 1 - square and noise oscs
  timeOscillator0 = map(intensity, 0, 1023, 10000, 100);
  timeOscillator1 = map(density, 0, 1023, 10000, 0);
  if (now - before0_merz > timeOscillator0) {
    before0_merz = now;
    osc0_merz = !osc0_merz;
  }
  if (now - before1_merz > timeOscillator1) {
    before1_merz = now;
    osc1_merz = random(0,2); // 0 or 1
  }
  oscOut = osc0_merz ^ osc1_merz;
  digitalWrite(OUT_MERZ, oscOut);

  // mode 2 - 'white noise'
  amplitude = map(intensity, 0, 1023, 0, 255);
  frequency = map(density, 0, 1023, 10000, 0);
  if (now - before0_wn > frequency) {
    before0_wn = now;
    osc0_wn = random(0,2);
  }
  oscOut = osc0_wn * amplitude;
  analogWrite(OUT_WN, oscOut);
  
  // mode 3 - drone mode
  timeOscillator0 = map(intensity, 0, 1023, 50000, 400);
  timeOscillator1 = map(density, 0, 1023, 50000, 0);
  if (now - before0_drone > timeOscillator0) {
    before0_drone = now;
    osc0_drone = !osc0_drone;
  }
  if (now - before1_drone > timeOscillator1) {
    before1_drone = now;
    osc1_drone = !osc1_drone;
  }
  oscOut = osc0_drone ^ osc1_drone;
  digitalWrite(OUT_DRONE, oscOut);
  
  // mode 4 - noise mode
  if (now - before0_noise > timeOscillator0_noise) {
    before0_noise = now;
    out_density = map(density, 0, 1024, 1014, 10);
    delta = random(out_density) * 5000; // this 5000 will need to be tweaked
    mode_noise = !mode_noise;
    mode_noise ? timeOscillator0_noise delta : timeOscillator0_noise = delta * 3;
  }
  if (mode_noise) { // making noise
    out_intensity = map(intensity, 0, 1023, 1, 255);
    analogWrite(OUT_NOISE, random(out_intensity));
  } else { // silent
    analogWrite(OUT_NOISE, 0);
  }
    
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
    delay(random(density) * 3);
  }
  */
}

