#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE); // I2C configuration

float TIRE_DIAMETER = 72.2; // Tire diameter in CM
float TOTAL_DISTANCE = 0; // Total distance
int ROTATIONS = 0; // Rotations

// Interruption to compute total distance on sensor trigger
void incrementTotalDistance() {
  volatile static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 200) {  // Debounce interrupts for 200ms
    TOTAL_DISTANCE += 2 * 3.14 * TIRE_DIAMETER / 100;
    ROTATIONS += 1;
  }
  
  lastInterruptTime = interruptTime;
}

void setup(){
 Serial.begin(9600); // For debugging

 // LCD setup
 lcd.begin(20,4);
 lcd.setBacklight(HIGH); 

 // Initial LCD prints
 lcd.setCursor(0,0); 
 lcd.print("DISTANCIA: ");

 lcd.setCursor(0,1); 
 lcd.print("VELOCIDADE: ");

 lcd.setCursor(0,2);
 lcd.print("RPM: ");
 
 lcd.setCursor(0,3);
 lcd.print("Tempo: ");
 
 // Magnetic sensor pin setup
 pinMode(2, INPUT_PULLUP);
 attachInterrupt(0, incrementTotalDistance, RISING);

 // Reset button setup
 pinMode(12, INPUT_PULLUP);
}
 
void loop(){
  unsigned long int startTime = millis();
  int lastComputedMinute = 0;

  while (1) {
    // Reset counters
    if (digitalRead(12) == LOW) {
      startTime = millis();
      lastComputedMinute = 0;
      
      TOTAL_DISTANCE = 0;
      ROTATIONS = 0;
    }
    
    // Print distance on LCD
    if (TOTAL_DISTANCE < 1000) {
      lcd.setCursor(11,0); 
      lcd.print((String) TOTAL_DISTANCE + " m "); 
    }
    else {
      lcd.setCursor(11,0); 
      lcd.print((String) (TOTAL_DISTANCE/1000) + " km "); 
    }

    // Print average speed on LCD
    int diffTime = (millis() - startTime) / 1000;
    lcd.setCursor(12,1); 
    lcd.print((String) (TOTAL_DISTANCE / diffTime) + " m/s"); 

    // Print RPM on LCD
    int nextMinute = (int) ((diffTime / 60) + 1); // Calculate next minute (ex: 80s -> nextMinute = 2)
    int estimatedRotations = ((ROTATIONS * nextMinute * 60) / diffTime) / nextMinute; // Application of rule of three to estimate based on proportion
    
    lcd.setCursor(5, 2);
    lcd.print((String) (estimatedRotations) + " rot/min    ");

    // Print Time on LCD
    lcd.setCursor(7, 3);
    lcd.print((String) ((float) diffTime / 60) + " min   ");

    // Delay for LCD updates
    delay(2000);
  }
}
