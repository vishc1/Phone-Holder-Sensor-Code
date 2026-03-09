

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS1115!");
    while (1);
  }

  
  ads.setGain(GAIN_ONE);

  Serial.println("ADS1115 Basic Voltage Test");
  Serial.println("Time_ms,Raw,Voltage_V");
}

void loop() {
  int16_t raw = ads.readADC_SingleEnded(0); // A0
  float voltage = raw * 0.000125;            // 0.125 mV per bit

  Serial.print(millis());
  Serial.print(",");
  Serial.print(raw);
  Serial.print(",");
  Serial.println(voltage, 4);

  delay(5000); // output every 5 seconds
}
