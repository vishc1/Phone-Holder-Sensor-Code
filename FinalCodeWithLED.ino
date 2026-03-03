// --- TUNE THESE VALUES ---
float threshold = 0.15;      // The 0.15V dip we are looking for
int windowSize = 5;          // Checking in 5-second blocks

// --- RGB LED PINS (Digital 8, 9, 10) ---
const int GREEN_PIN = 8; 
const int BLUE_PIN = 9;  
const int RED_PIN = 10;   

float baselineMinV = 0;

void setup() {
  analogReference(AR_EXTERNAL); 
  Serial.begin(9600);
  
  // Set RGB pins as outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  
  // Startup Test: Quick color cycle to confirm wiring
  digitalWrite(RED_PIN, HIGH); delay(200); digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH); delay(200); digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH); delay(200); digitalWrite(GREEN_PIN, LOW);

  Serial.println("SYSTEM START: CALIBRATING BASELINE (10 SEC)...");
  
  float sumOfMins = 0;
  for (int j = 0; j < 10; j++) {
    float oneSecMin = getOneSecondMin();
    sumOfMins += oneSecMin;
    Serial.print("."); 
  }
  baselineMinV = sumOfMins / 10.0;
  
  Serial.println("\n--- CALIBRATION COMPLETE ---");
  Serial.print("BASELINE MIN: "); Serial.println(baselineMinV, 4);
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

  // --- RGB COLOR LOGIC ---
  // Turn everything off first
  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  if (hitCount >= 2) {
    digitalWrite(RED_PIN, HIGH); // Red for Active
    Serial.println("TELEMETRY: [ RED ] PHONE ACTIVE");
  } 
  else if (hitCount == 1) {
    digitalWrite(BLUE_PIN, HIGH); // Blue for Find My
    Serial.println("TELEMETRY: [ BLUE ] FIND MY PING");
  } 
  else {
    digitalWrite(GREEN_PIN, HIGH); // Green for Off
    Serial.println("TELEMETRY: [ GREEN ] NO SIGNAL");
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
