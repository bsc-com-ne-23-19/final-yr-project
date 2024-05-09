
#include <LiquidCrystal.h>

const int lightSensorPin = A0;    // Analog pin connected to the light sensor
const int ledPin = 6;             // Pin connected to the LED
const int targetBrightness = 50;  // Target brightness level (%)
const int brightnessThreshold = 5; // Threshold for brightness adjustment

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  
  pinMode(ledPin, OUTPUT);
  
  // Initialize LCD
  lcd.begin(16, 2);
  
  // Display initial message
  lcd.print("Light Intensity:");
}

void loop() {
  // Read light intensity from sensor
  int lightIntensity = analogRead(lightSensorPin);
  
  // Map the value to a range suitable for display
  int brightnessLevel = map(lightIntensity, 0, 1023, 0, 100);
  
  // Display the light intensity on LCD
  lcd.setCursor(0, 1);
  lcd.print("      "); // Clear previous value
  lcd.setCursor(0, 1);
  lcd.print(brightnessLevel);
  lcd.print("% Brightness");
  
  // Control LED based on light intensity
  if (brightnessLevel < targetBrightness - brightnessThreshold) {
    analogWrite(ledPin, 255); // Set LED brightness to maximum
  } else if (brightnessLevel > targetBrightness + brightnessThreshold) {
    analogWrite(ledPin, 0);   // Set LED brightness to minimum
  } else {
    // LED brightness is within acceptable range
    // Turn off LED
    analogWrite(ledPin, 0);
  }
  
  delay(500); // Adjust delay according to your requirement
}
