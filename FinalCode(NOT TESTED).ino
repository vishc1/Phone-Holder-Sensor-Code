// --- TUNE THESE VALUES ---
float threshold = 0.07;      // The 0.15V dip we are looking for
int windowSize = 5;          // Checking in 5-second blocks
// -----------------------

float baselineMinV = 0;

void setup() {
  analogReference(AR_EXTERNAL); 
  Serial.begin(9600);
  delay(1000); 
  
  // PHASE 1: 10-SECOND CALIBRATION (AVERAGING THE MINS)
  Serial.println("SYSTEM START: CALIBRATING BASELINE (10 SEC)...");
  float sumOfMins = 0;
  for (int j = 0; j < 10; j++) {
    float oneSecMin = getOneSecondMin();
    sumOfMins += oneSecMin;
    Serial.print("Sampling Baseline: "); Serial.print(j+1); Serial.println("/10");
  }
  baselineMinV = sumOfMins / 10.0;
  
  Serial.println("---------------------------------------");
  Serial.print("BASELINE MIN: "); Serial.print(baselineMinV, 4); Serial.println(" V");
  Serial.println("TELEMETRY: 3-MODE DETECTION ACTIVE");
  Serial.println("---------------------------------------");
}

void loop() {
  int hitCount = 0;
  Serial.println("Analyzing 5-second window...");
  
  for (int i = 1; i <= windowSize; i++) {
    float currentMin = getOneSecondMin();
    float dipAmount = baselineMinV - currentMin;

    if (dipAmount >= threshold) {
      hitCount++;
      Serial.print("  [Sec "); Serial.print(i); Serial.print("]: HIT");
    } else {
      Serial.print("  [Sec "); Serial.print(i); Serial.print("]: CLEAR");
    }
    Serial.print(" (Dip: "); Serial.print(dipAmount, 4); Serial.println(")");
  }

  // --- 3-MODE TELEMETRY LOGIC ---
  Serial.print("RESULT: "); Serial.print(hitCount); Serial.print("/5 hits -> ");
  
  if (hitCount >= 2) {
    // 2, 3, 4, or 5 hits all mean the phone is actively transmitting
    Serial.println("TELEMETRY: [ PHONE ON / ACTIVE ]");
  } 
  else if (hitCount == 1) {
    // Exactly 1 hit is the "periodic ping" from Find My
    Serial.println("TELEMETRY: [ PHONE OFF - FIND MY ACTIVE ]");
  } 
  else {
    // 0 hits means silence
    Serial.println("TELEMETRY: [ PHONE COMPLETELY OFF ]");
  }
  
  Serial.println("---------------------------------------");
}

// HELPER: Collects the absolute MINIMUM voltage over 10,000 samples
float getOneSecondMin() {
  int rangeCount = 10000;
  float vMin = 4.0; 
  for (int i = 0; i < rangeCount; i++) {
    int raw = analogRead(A0);
    float v = (raw * 3.3) / 1024.0;
    if (v < vMin) vMin = v;
  }
  return vMin;
}
