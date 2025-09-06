#include <Arduino.h>
#include <ESP32Encoder.h>

// Create encoder objects
ESP32Encoder enc1, enc2, enc3, enc4;

// Encoder pins (CLK, DT)
#define ENC1_A 35
#define ENC1_B 34
#define ENC2_A 33
#define ENC2_B 32
#define ENC3_A 25
#define ENC3_B 26
#define ENC4_A 14
#define ENC4_B 27

#define CLAMP_LOWER 0                        // deej lower voluem bound
#define CLAMP_UPPER 1023                     // deej upper volume bound
#define DETENTS 20                           // detents on encoder

#define VOLUME_START (CLAMP_UPPER / 2)       // volume starting value
#define VOLUME_UNIT (CLAMP_UPPER / DETENTS)  // volume increment

// Direction Tracking: true = CW, false = CCW
bool direction1 = true;
bool direction2 = true; 
bool direction3 = true; 
bool direction4 = true; 

long prev_raw1 = 0;
long prev_raw2 = 0;
long prev_raw3 = 0;
long prev_raw4 = 0;

// Starting volumes: 512 = half volume
long volume1 = 512;
long volume2 = 512;   
long volume3 = 512;   
long volume4 = 512;       

long calcVolume(long volume, bool direction) {
  if (direction){
    volume = volume + VOLUME_UNIT;
    if (volume > CLAMP_UPPER){
      volume = CLAMP_UPPER;
    }
  }
  else {
    volume = volume - VOLUME_UNIT;
    if (volume < CLAMP_LOWER){
      volume = CLAMP_LOWER;
    }
  }
  return volume;
}

bool getDirection(long raw, long prev_raw) {
  bool direction = true;
  if (raw > prev_raw) {
    direction = true;
  }
  else {
    direction = false;
  }
  return direction;
}

void sendDeej(){
  Serial.print(volume1);
  Serial.print("|");
  Serial.print(volume2);
  Serial.print("|");
  Serial.print(volume3);
  Serial.print("|");
  Serial.println(volume4);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  //Serial.println("ESP32 4 Rotary Encoders Test");

  // Enable internal pull-ups
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // Attach encoders in half-quad mode (2 counts per detent internally)
  enc1.attachHalfQuad(ENC1_A, ENC1_B);
  enc2.attachHalfQuad(ENC2_A, ENC2_B);
  enc3.attachHalfQuad(ENC3_A, ENC3_B);
  enc4.attachHalfQuad(ENC4_A, ENC4_B);

  // Reset counts
  enc1.clearCount();
  enc2.clearCount();
  enc3.clearCount();
  enc4.clearCount();
}

void loop() {
  // Read raw counts
  long raw1 = enc1.getCount() / 2;
  long raw2 = enc2.getCount() / 2;
  long raw3 = enc3.getCount() / 2;
  long raw4 = enc4.getCount() / 2;

  // Get directions
  bool direction1 = getDirection(raw1, prev_raw1);
  bool direction2 = getDirection(raw2, prev_raw2);
  bool direction3 = getDirection(raw3, prev_raw3);
  bool direction4 = getDirection(raw4, prev_raw4);

  // Only print if changed
  if (raw1 != prev_raw1 || raw2 != prev_raw2 || raw3 != prev_raw3 || raw4 != prev_raw4) {
    if (raw1 != prev_raw1) {
      bool direction1 = getDirection(raw1, prev_raw1);
      volume1 = calcVolume(volume1, direction1);
      // Reset prev raw values
      prev_raw1 = raw1;
    }
    if (raw2 != prev_raw2) {
      bool direction2 = getDirection(raw2, prev_raw2);
      volume2 = calcVolume(volume2, direction2);
      // Reset prev raw values
      prev_raw2 = raw2;
    }
    if (raw3 != prev_raw3) {
      bool direction3 = getDirection(raw3, prev_raw3);
      volume3 = calcVolume(volume3, direction3);
      // Reset prev raw values
      prev_raw3 = raw3;
    }
    if (raw4 != prev_raw4) {
      bool direction4 = getDirection(raw4, prev_raw4);
      volume4 = calcVolume(volume4, direction4);
      // Reset prev raw values
      prev_raw4 = raw4;
    }
    //send Volumes in deej format over serial
    sendDeej();
  }
  delay(50); // small delay to reduce Serial spam
}

