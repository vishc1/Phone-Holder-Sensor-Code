// --- TUNE THESE VALUES ---
float threshold = 0.015;    // V drop required to count as a "hit"
int requiredHits = 5;       // How many seconds in a row to confirm
// -----------------------

float baselineV = 0;
int triggerCount = 0;       // Keeps track of consecutive hits

void setup() {
  analogReference(AR_EXTERNAL); 
  Serial.begin(9600);
  delay(1000); 
  
  Serial.println("STEP 1: CALIBRATING BASELINE (10 SECONDS)...");
  
  float tenSecondSum = 0;
  for (int j = 0; j < 10; j++) {
    float oneSecondAvg = getOneSecondAverage();
    tenSecondSum += oneSecondAvg;
    Serial.print("Baseline Reading "); Serial.print(j+1); 
    Serial.print("/10: "); Serial.println(oneSecondAvg, 4);
  }
  
  baselineV = tenSecondSum / 10.0;
  Serial.println("---------------------------------------");
  Serial.print("BASELINE SET AT: "); Serial.print(baselineV, 4); Serial.println(" V");
  Serial.println("MONITORING (NEEDS 5 CONSECUTIVE DIPS)...");
  Serial.println("---------------------------------------");
}

void loop() {
  float currentMin = getOneSecondMin();
  float dipAmount = baselineV - currentMin;

  // Check if this specific second was a "hit"
  if (dipAmount > threshold) {
    triggerCount++; // Add to the streak
    Serial.print(" [!] DIP DETECTED ("); 
    Serial.print(triggerCount); 
    Serial.println("/5)");
  } else {
    triggerCount = 0; // RESET if we have a clean second
    Serial.println(" [.] Signal Clear (Streak Reset)");
  }

  // Final Decision Logic
  if (triggerCount >= requiredHits) {
    Serial.println(" >>>>>>> ALERT: PHONE CONFIRMED ACTIVE <<<<<<< ");
    // triggerCount = 0; // Optional: Reset after alert, or keep alerting
  }

  // Small delay is handled by the 10,000 samples in the helper functions
}

// HELPER: Samples for ~1 second to find the MINIMUM spike
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

// HELPER: Samples for ~1 second to find the AVERAGE for baseline
float getOneSecondAverage() {
  int rangeCount = 10000;
  float sum = 0;
  for (int i = 0; i < rangeCount; i++) {
    sum += (analogRead(A0) * 3.3) / 1024.0;
  }
  return sum / rangeCount;
}
