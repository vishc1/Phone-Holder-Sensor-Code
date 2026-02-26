#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

const char text[] = " ARDUINO UNO R4 WIFI "; // The text you want to display



void setup() {
  // 1. TELL ARDUINO TO USE THE PIN, NOT THE INTERNAL 5V
  analogReference(AR_EXTERNAL); 
  //analogReference(EXTERNAL); 
  // 2. WAIT A MOMENT FOR VOLTAGE TO SETTLE
  delay(1000); 
  
  Serial.begin(9600);
   matrix.begin(); // Initialize the LED matrix


}

void loop() {
  int rangeCount = 15000;
  int i = 0;
  int rawValue = 0;
  float vOut = 0;
  float vOutSum = 0;
  float VoutAverage = 0;
  float VoutMin = 4;
  const char ONtext[] = "  Mob ON  "; // Add spaces at start/end for cleaner loops
  const char OFFtext[] = "  Mob OFF  "; // Add spaces at start/end for cleaner loops
  const char NoMobtext[] = "  No Mob  "; // Add spaces at start/end for cleaner loops

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
  

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);      // Set "ink" to white (LEDs on)
  matrix.textScrollSpeed(80);     // Lower is faster (50-100 is standard)
  matrix.textFont(Font_5x7);      // Built-in 5x7 font
  
  // beginText(x, y, color)
  matrix.beginText(0, 1, 0xFFFFFF); 
  if (VoutMin < 0.70) {
    matrix.print(ONtext);    
  }
  else {
    matrix.print(OFFtext);    
  }

  // SCROLL_LEFT moves text to the left
  matrix.endText(SCROLL_LEFT); 
  
  matrix.endDraw();
  delay(1000);
 
}
