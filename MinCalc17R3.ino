void setup() {
  // 1. TELL ARDUINO TO USE THE PIN, NOT THE INTERNAL 5V
  //analogReference(AR_EXTERNAL); 
  analogReference(EXTERNAL); 
  // 2. WAIT A MOMENT FOR VOLTAGE TO SETTLE
  delay(1000); 
  
  Serial.begin(9600);
}

void loop() {
  int rangeCount = 5000;
  int i = 0;
  int rawValue = 0;
  float vOut = 0;
  float vOutSum = 0;
  float VoutAverage = 0;
  float VoutMin = 4;

  for (i = 0; i < rangeCount;i++) {
  // Now, 1023 represents 3.3V instead of 5V
    rawValue = analogRead(A0);
    // Updated math for 3.3V reference
    vOut = (rawValue * 3.3) / 1024.0;
    // find the lowest vout value
    if (VoutMin > vOut) {
      VoutMin = vOut; 
    }
    vOutSum += vOut;
  }
  VoutAverage = vOutSum/rangeCount;

  // AD8317 Logic (approximate values)
  float intercept = 2.0; 
  float slope = -0.022; 
  float dBm = (vOut - intercept) / slope;

  Serial.print("VoutAverage: ");
  Serial.print(VoutAverage);
  Serial.print(" VoutMin: ");
  Serial.print(VoutMin);
  Serial.print(" V | Power: ");
  Serial.print(dBm);
  Serial.println(" dBm");
  
  // delay(500);
}
