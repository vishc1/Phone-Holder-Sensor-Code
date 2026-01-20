#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup() {
  Serial.begin(9600);

  if (!ads.begin()) {
    Serial.println("ERROR: ADS1115 NOT FOUND");
    while (1);
  }

  ads.setGain(GAIN_TWOTHIRDS); // ±6.144 V range

  Serial.println("Baseline RF Measurement");
  Serial.println("Ensure NO phone is near the antenna");
  Serial.println("Collecting data...");
  Serial.println("Time(ms), Voltage(V)");
}

void loop() {
  static unsigned long startTime = millis();

  int16_t raw = ads.readADC_SingleEnded(0);
  float voltage = raw * 0.1875 / 1000.0;

  Serial.print(millis() - startTime);
  Serial.print(", ");
  Serial.println(voltage, 3);

  delay(100); // 10 samples per second
}
