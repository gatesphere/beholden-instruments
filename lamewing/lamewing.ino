// lamewing
// based on "Harsh Noise Generator" by Kinetik Laboratories
// see original at: 
// http://kinetiklaboratories.blogspot.com/p/harsh-noise-generator.html

// pins
#define OUT 9
#define INTENSITY A0
#define DENSITY A4
//#define DEBUG


// Variables
int mode = 0;                  // Working mode
int osc0;                      // First oscillator
int osc1;                      // Second oscillator
int oscOut;                    // Output
int amplitude;
int frequency;
int intensity;
long density;
long now;
long before0 = 0;
long before1 = 0;
long beforeStutter = 0;
long timeOscillator0 = 0;
long timeOscillator1 = 0;
long timeStutter = 0;
int stutter = false;


void setup() {
  pinMode(INTENSITY, INPUT);
  pinMode(DENSITY, INPUT);
  pinMode(OUT, OUTPUT);
  int readi = analogRead(INTENSITY);
  int readd = analogRead(DENSITY);
  if (readi > 342 & readi < 683) mode += 10;
  if (readi >= 683) mode += 20;
  if (readd > 342 & readd < 683) mode += 1;
  if (readd >= 683) mode += 2;
  // mode layout:
  // 00: dual random osc
  // 01: dual random osc with stutter
  // 02: square & noise osc
  // 10: square & noise osc with stutter
  // 11: white noise
  // 12: white noise with stutter
  // 20: dual stable osc (drone)
  // 21: dual stable osc (drone) with stutter
  // 22: random noise
  randomSeed(analogRead(A5));
#ifdef DEBUG
  //mode = 1;
  Serial.begin(9600);
  delay(1000);
  Serial.println(mode);
#endif //DEBUG  
}

void loop()
{
  if (mode == 0 || mode == 1)              // Double oscillator mode
  {
    now = micros();
    timeOscillator0 = (map(analogRead(INTENSITY), 0, 1023, 50000, 600) + random(1000));
    timeOscillator1 = (map(analogRead(DENSITY), 0, 1023, 50000, 600) + random(1000));
    if (now - before0 > timeOscillator0) {
      before0 = now;
      osc0 = !osc0;
    }
    if (now - before1 > timeOscillator1) {
      before1 = now;
      osc1 = !osc1;
    }
    if (mode == 1) {
      // stutter
      if (now - beforeStutter > timeStutter) {
        beforeStutter = now;
        stutter = !stutter;
        timeStutter = stutter ? random(12500, 550000) : random(25000, 2700000);
      }
      oscOut = stutter ? 0 : (osc0 ^ osc1);
    }
    else {
      oscOut = osc0 ^ osc1;
    }
    digitalWrite (OUT, oscOut);
  }

  if (mode == 2 || mode == 10) // square and noise oscs
  {
    now = micros();
    timeOscillator0 = (map(analogRead(INTENSITY), 0, 1023, 10000, 100));
    timeOscillator1 = (map(analogRead(DENSITY), 0, 1023, 10000, 0));
    if (now - before0 > timeOscillator0) {
      before0 = now;
      osc0 = !osc0;
    }
    if (now - before1 > timeOscillator1) {
      before1 = now;
      osc1 = random(0,2); // 0 or 1
    }
    if (mode == 10) {
      // stutter
      if (now - beforeStutter > timeStutter) {
        beforeStutter = now;
        stutter = !stutter;
        timeStutter = stutter ? random(12500, 550000) : random(25000, 2700000);
      }
      oscOut = stutter ? 0 : (osc0 ^ osc1);
    }
    else {
      oscOut = osc0 ^ osc1;
    }
    digitalWrite (OUT, oscOut);
  }

  if (mode == 11 || mode == 12) // white noise mode
  {
    amplitude = map(analogRead(INTENSITY), 0, 1023, 0, 255);
    frequency = map(analogRead(DENSITY), 0, 1023, 10000, 0);
    now = micros();
    if (now - before0 > frequency) {
      before0 = now;
      osc0 = random(0,2);
      oscOut = osc0 * amplitude;
    }
    if (mode == 12) {
      // stutter
      if (now - beforeStutter > timeStutter) {
        beforeStutter = now;
        stutter = !stutter;
        timeStutter = stutter ? random(12500, 550000) : random(25000, 2700000);
      }
      oscOut = stutter ? 0 : oscOut;
    }
    analogWrite (OUT, oscOut);
  }

  if (mode == 20 || mode == 21)              // Double stable oscillator (drone) mode
  {
    now = micros();
    timeOscillator0 = (map(analogRead(INTENSITY), 0, 1023, 50000, 400));
    timeOscillator1 = (map(analogRead(DENSITY), 0, 1023, 50000, 0));
    if (now - before0 > timeOscillator0) {
      before0 = now;
      osc0 = !osc0;
    }
    if (now - before1 > timeOscillator1) {
      before1 = now;
      osc1 = !osc1;
    }
    if (mode == 21) {
      // stutter
      if (now - beforeStutter > timeStutter) {
        beforeStutter = now;
        stutter = !stutter;
        timeStutter = stutter ? random(12500, 550000) : random(25000, 2700000);
      }
      oscOut = stutter ? 0 : (osc0 ^ osc1);
    }
    else {
      oscOut = osc0 ^ osc1;
    }
    digitalWrite (OUT, oscOut);
  }

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
}

