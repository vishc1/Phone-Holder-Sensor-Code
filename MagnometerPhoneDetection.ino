#include <Adafruit_QMC5883P.h>

// --- TUNE THESE VALUES ---
float magThreshold = 0.15;   // Threshold in Gauss (Start with 0.05 or 0.10)
int windowSize = 5;          // Checking in 5-second blocks

// --- RGB LED PINS ---
const int GREEN_PIN = 8; 
const int BLUE_PIN = 9;  
const int RED_PIN = 10;   

Adafruit_QMC5883P qmc;
float baselineMag = 0;
unsigned long startTime;

void setup() {
  Serial.begin(115200);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  if (!qmc.begin()) {
    Serial.println("CRITICAL: QMC5883P not found!");
    while(1); 
  }

  // Setup Sensor for High Sensitivity
  qmc.setMode(QMC5883P_MODE_NORMAL);
  qmc.setODR(QMC5883P_ODR_50HZ);
  qmc.setRange(QMC5883P_RANGE_2G); // 2G is most sensitive for small EMF

  Serial.println("CALIBRATING MAGNETIC BASELINE (10 SEC)...");
  
  float magSum = 0;
  int samples = 0;
  unsigned long calStart = millis();
  
  // Calibrate for 10 seconds
  while (millis() - calStart < 10000) {
    float gx, gy, gz;
    if (qmc.getGaussField(&gx, &gy, &gz)) {
      float currentMag = sqrt(gx*gx + gy*gy + gz*gz);
      magSum += currentMag;
      samples++;
      if(samples % 10 == 0) Serial.print(".");
    }
    delay(100);
  }
  
  baselineMag = magSum / samples;
  
  Serial.println("\n--- CALIBRATION COMPLETE ---");
  Serial.print("Baseline Magnitude: "); Serial.println(baselineMag, 4);
  Serial.println("TIME(ms), MAG_DIFF, HITS/5, STATUS");
  
  startTime = millis();
}

void loop() {
  int hitCount = 0;
  float latestDiff = 0;

  // 5-second window
  for (int i = 0; i < windowSize; i++) {
    float gx, gy, gz;
    if (qmc.getGaussField(&gx, &gy, &gz)) {
      float currentMag = sqrt(gx*gx + gy*gy + gz*gz);
      latestDiff = abs(currentMag - baselineMag);

      if (latestDiff >= magThreshold) {
        hitCount++;
      }
    }
    delay(1000); // 1 second intervals
  }

  // --- TELEMETRY ---
  Serial.print(millis() - startTime); Serial.print(",");
  Serial.print(latestDiff, 4);        Serial.print(",");
  Serial.print(hitCount);             Serial.print("/5,");

  // --- LED LOGIC ---
  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  if (hitCount >= 2) {
    digitalWrite(BLUE_PIN, HIGH); // Blue if it senses the phone (2+ hits)
    Serial.println("BLUE: SENSING_PHONE");
  } 
  else {
    digitalWrite(GREEN_PIN, HIGH); // Green for Idle
    Serial.println("GREEN: IDLE");
  }
}
