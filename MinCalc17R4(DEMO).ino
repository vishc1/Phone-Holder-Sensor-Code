void setup() {
  
  analogReference(AR_EXTERNAL); 
 
  delay(1000); 
  
  Serial.begin(9600);
}

void loop() {
  int rangeCount = 15000;
  int i = 0;
  int rawValue = 0;
  float vOut = 0;
  float vOutSum = 0;
  float VoutAverage = 0;
  float VoutMin = 4;

  for (i = 0; i < rangeCount;i++) {
 
    rawValue = analogRead(A0);
   
    vOut = (rawValue * 3.3) / 1024.0;
    
    if (VoutMin > vOut) {
      VoutMin = vOut; 
    }
    vOutSum += vOut;
  }
  VoutAverage = vOutSum/rangeCount;

  
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
