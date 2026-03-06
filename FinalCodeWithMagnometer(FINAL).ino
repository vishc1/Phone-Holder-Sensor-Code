#include <Adafruit_QMC5883P.h>
#include <Wire.h>

// --- TUNE THESE VALUES ---
float threshold = 0.08;      
float magThreshold = 0.11;//0.15   
int windowSize = 5;          

const int GREEN_PIN = 8; 
const int BLUE_PIN = 9;  
const int RED_PIN = 10;   

Adafruit_QMC5883P qmc;
float baselineMinV = 0;
float baselineMag = 0;
unsigned long startTime;

void setup() {
  analogReference(AR_EXTERNAL); 
  Serial.begin(9600); 
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  
  // 1. Start I2C
  Wire.begin();

  // 2. Initialize Magnetometer
  if (!qmc.begin()) {
    Serial.println("CRITICAL: QMC5883P not found! Check SCL/SDA wires.");
    while(1); 
  }
  qmc.setMode(QMC5883P_MODE_NORMAL);
  qmc.setRange(QMC5883P_RANGE_2G);

  // Startup sequence
  digitalWrite(RED_PIN, HIGH); delay(200); digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH); delay(200); digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH); delay(200); digitalWrite(GREEN_PIN, LOW);

  Serial.println("SYSTEM_START,Calibration_Mode");
  
  float sumOfMins = 0;
  float sumOfMag = 0;
  int magSamples = 0;

  for (int j = 0; j < 10; j++) {
    sumOfMins += getOneSecondMin();
    
    float gx, gy, gz;
    if (qmc.getGaussField(&gx, &gy, &gz)) {
      sumOfMag += sqrt(gx*gx + gy*gy + gz*gz);
      magSamples++;
    }
    Serial.print("."); 
  }
  baselineMinV = sumOfMins / 10.0;
  baselineMag = sumOfMag / (magSamples > 0 ? magSamples : 1);
  
  Serial.println("\n--- CALIBRATION COMPLETE ---");
  Serial.print("Baseline_V: "); Serial.println(baselineMinV, 4);
  Serial.println("TIME(ms),HITS/5,STATUS"); 
  
  startTime = millis();
}

void loop() {
  int hitCount = 0;
  bool magAlert = false;

  for (int i = 1; i <= windowSize; i++) {
    float currentMin = getOneSecondMin();
    float dipAmount = baselineMinV - currentMin;

    if (dipAmount >= threshold) {
      hitCount++;
    }

    // Check Magnetometer right after the RF loop
    float gx, gy, gz;
    if (qmc.getGaussField(&gx, &gy, &gz)) {
      float currentMag = sqrt(gx*gx + gy*gy + gz*gz);
      if (abs(currentMag - baselineMag) >= magThreshold) {
        magAlert = true; 
      }
    }
  }

  // --- TELEMETRY OUTPUT ---
  Serial.print(millis() - startTime); 
  Serial.print(","); 
  Serial.print(hitCount); 
  Serial.print("/5,");

  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  if (hitCount >= 3) {
    digitalWrite(RED_PIN, HIGH); 
    Serial.println("RED: ACTIVE");
  } 
  else if (hitCount == 2 || (hitCount < 2 && magAlert)) {
    digitalWrite(BLUE_PIN, HIGH); 
    Serial.println("BLUE: PING/EMF");
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
    
    // Add a tiny micro-delay every 1000 samples to let the CPU handle I2C
    if (i % 1000 == 0) delayMicroseconds(10); 
  }
  return vMin;
}
