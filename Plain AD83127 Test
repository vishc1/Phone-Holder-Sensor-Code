// --- Configuration for AD8317 ---
const int ad8317Pin = A0;      
const int numReadings = 20;    

// AD8317 Parameters
const float slope = -0.022;    // -22mV/dB (Negative slope)
const float intercept = 2.5;   // Voltage at theoretical 0dBm

void setup() {
  Serial.begin(9600);
  Serial.println("AD8317 | Final Corrected Math");
}

void loop() {
  long sumADC = 0;

  // 1. Average 20 readings
  for (int i = 0; i < numReadings; i++) {
    sumADC += analogRead(ad8317Pin);
    delay(2); 
  }

  float avgADC = (float)sumADC / numReadings;
  
  // 2. Convert to Voltage (Assuming 5V Arduino rail)
  float voltage = avgADC * (5.0 / 1023.0);
  
  // 3. FIXED dBm FORMULA
  // Result = (MeasuredVoltage - Intercept) / Slope
  // Example: (1.6 - 2.5) / -0.022 = -0.9 / -0.022 = -40.9 dBm
  float dbm = (voltage - intercept) / slope;

  Serial.print("Vout: ");
  Serial.print(voltage, 3);
  Serial.print("V | Power: ");
  Serial.print(dbm, 1);
  Serial.println(" dBm");

  delay(1000); 
}
