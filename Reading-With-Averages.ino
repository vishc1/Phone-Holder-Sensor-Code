

#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  // use default I2C address 0x48

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS1115!");
    while (1);
  }

 
  ads.setGain(GAIN_ONE);

  Serial.println("ADS1115 Basic Voltage Test");
  Serial.println("Time_ms,Raw,Voltage_V, min, max");
}

void loop() {
  int i = 0;
  int readingCount = 100;
  float average_voltage = 0;
  int raw = 0;
  float max_peak = 0;
  float min_peak = 2;
  float voltage;
  float voltageSum = 0;
  for (i = 0; i < readingCount;i++) {
    raw = ads.readADC_SingleEnded(0); // A0
    voltage = raw * 0.000125;            // 0.125 mV per bit
    voltageSum += voltage;
    if (min_peak > voltage) {
      min_peak = voltage;
    }
    if (max_peak < voltage) {
      max_peak = voltage;
    }
    delay(3);
  }

  average_voltage = voltageSum/readingCount;
  
  Serial.print(millis());
  Serial.print(",");
  Serial.print(raw);
  Serial.print(",");
  Serial.print(average_voltage, 4);
  Serial.print(",");
  Serial.print(min_peak, 4);
  Serial.print(",");
  Serial.println(max_peak, 4);
  // delay(2000); // output every 5 seconds
}
