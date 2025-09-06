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

long pos1 = 0;

bool direction1 = true; //true = CW, false = CCW
long prev_raw1 = 0;

long volume1 = 512;     // starting volume

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

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 4 Rotary Encoders Test");

  // Enable internal pull-ups
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // Attach encoders in half-quad mode (2 counts per detent internally)
  enc1.attachHalfQuad(ENC1_A, ENC1_B);

  // Reset counts
  enc1.clearCount();
}

void loop() {
  // Read raw counts
  long raw1 = enc1.getCount();
  // Get directions
  bool direction1 = getDirection(raw1, prev_raw1);

  // Only print if changed
  if (raw1 != prev_raw1) {

    volume1 = calcVolume(volume1, direction1);

    if (direction1) {
      Serial.print(" -> ");
    }
    else {
      Serial.print(" <- ");
    }
    Serial.print("|");
    Serial.println(volume1);
      
    // Reset prev raw values
    prev_raw1 = raw1;
  }

  delay(50); // small delay to reduce Serial spam
}

