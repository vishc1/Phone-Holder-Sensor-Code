// --- TUNE THESE VALUES ---
float threshold = 0.08;      // The 0.15V dip we are looking for
int windowSize = 5;          // Checking in 5-second blocks

// --- RGB LED PINS (Digital 8, 9, 10) ---
const int GREEN_PIN = 8; 
const int BLUE_PIN = 9;  
const int RED_PIN = 10;   

float baselineMinV = 0;
unsigned long startTime;

void setup() {
  analogReference(AR_EXTERNAL); 
  Serial.begin(9600);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  
  // Startup sequence
  digitalWrite(RED_PIN, HIGH); delay(200); digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH); delay(200); digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH); delay(200); digitalWrite(GREEN_PIN, LOW);

  Serial.println("SYSTEM_START,Calibration_Mode");
  
  float sumOfMins = 0;
  for (int j = 0; j < 10; j++) {
    float oneSecMin = getOneSecondMin();
    sumOfMins += oneSecMin;
    Serial.print("."); 
  }
  baselineMinV = sumOfMins / 10.0;
  
  Serial.println("\n--- CALIBRATION COMPLETE ---");
  Serial.print("Baseline_V: "); Serial.println(baselineMinV, 4);
  Serial.println("TIME(ms),HITS/5,STATUS"); // Updated Header
  startTime = millis();
}

void loop() {
  int hitCount = 0;

  for (int i = 1; i <= windowSize; i++) {
    float currentMin = getOneSecondMin();
    float dipAmount = baselineMinV - currentMin;

    if (dipAmount >= threshold) {
      hitCount++;
    }
  }

  // --- TELEMETRY OUTPUT (The part you wanted added) ---
  Serial.print(millis() - startTime); 
  Serial.print(","); 
  Serial.print(hitCount); 
  Serial.print("/5"); // Shows the x/5 value
  Serial.print(",");

  // --- RGB COLOR LOGIC (Kept exactly the same) ---
  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  if (hitCount >= 3) {
    digitalWrite(RED_PIN, HIGH); 
    Serial.println("RED: ACTIVE");
  } 
  else if (hitCount == 2) {
    digitalWrite(BLUE_PIN, HIGH); 
    Serial.println("BLUE: PING");
  } 
  else {
    digitalWrite(GREEN_PIN, HIGH); 
    Serial.println("GREEN: IDLE");
  }
}

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
