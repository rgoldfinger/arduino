#include <Adafruit_LSM303.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoPixel internal = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_LSM303 lsm;

void setup() 
{
  Serial.begin(9600);
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  }
//
  strip.begin();
  strip.show();
  internal.begin();
  internal.show();
}

void loop() 
{
  lsm.read();
//  Serial.print("Accel X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
//  Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
//  Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
  Serial.print("Sum X + Y: "); Serial.print((int)lsm.magData.x + (int)lsm.magData.y);     Serial.print("  ");
  Serial.print("Mag X: "); Serial.print((int)lsm.magData.x);     Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.magData.y);         Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.magData.z);       Serial.print(" ");


  
  uint16_t i, j, x, y;
  uint16_t sum;
  uint16_t adjustedColor;
 
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    lsm.read();   
    x = lsm.magData.x;
    y = (int)lsm.magData.y;
    sum = x + y;
    
    Serial.print("X: "); Serial.print(x); Serial.print(" ");
    Serial.print("Y: "); Serial.print(y); Serial.print(" ");
    Serial.print("Sum: "); Serial.print(sum); Serial.print("\n");
//    Serial.print("normalized: "); Serial.print(sum & 255); Serial.print("\n");

    adjustedColor = Wheel(sum & 255);
    internal.setPixelColor(0, adjustedColor);
    internal.show();
    strip.setPixelColor(0, adjustedColor);
    strip.setPixelColor(1, adjustedColor); 
    strip.show();
    delay(100);
  }
}
 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return internal.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return internal.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return internal.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

