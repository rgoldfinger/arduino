#include <Wire.h>
#include <LSM303.h>
#include <Adafruit_NeoPixel.h>

LSM303 compass;

#define PIN 6

Adafruit_NeoPixel internal = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);

byte priorColor;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>){  -573,   -879,   -638};
  compass.m_max = (LSM303::vector<int16_t>){  +542,   +321,   +290};

  
  strip.begin();
  strip.show();
  internal.begin();
  internal.show();
}

void loop() {
  compass.read();
  
  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
    compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  byte move, distanceToZero, distanceTo255, resultColor;
  float heading = compass.heading();
  byte newColor = toByte(heading);
  byte MOVE_LIMIT = 2;
  
  if (newColor != priorColor) {
    if (priorColor > newColor) {
      distanceToZero = priorColor - newColor;
      distanceTo255 = newColor + (255 - priorColor);
    } else {
      distanceToZero = newColor - priorColor;
      distanceTo255 = priorColor + (255 - newColor);
    }

    Serial.print("distanceToZero: "); Serial.print(distanceToZero); Serial.print("\n");
    Serial.print("distanceTo255: "); Serial.print(distanceTo255); Serial.print("\n");
    Serial.print("newColor: "); Serial.print(newColor); Serial.print("\n");
    Serial.print("priorColor: "); Serial.print(priorColor); Serial.print("\n");
   
    if (distanceToZero <= distanceTo255) {
      Serial.print("regular move------ "); Serial.print("\n");
      move = distanceToZero;
      if (move > MOVE_LIMIT) {
        move = MOVE_LIMIT;
      }
      if (priorColor > newColor) {
        resultColor = priorColor - move;
      } else {
        resultColor = priorColor + move;
      }
    } else {
      move = distanceTo255;
      Serial.print("go around the top move------ "); Serial.print("\n");
      if (move > MOVE_LIMIT) {
        move = MOVE_LIMIT;
      }
      if (priorColor < newColor) {
        resultColor = priorColor - move;
      } else {
        resultColor = priorColor + move;
      }
    }
    Serial.print("adjusted move: "); Serial.print(move); Serial.print("\n");
    Serial.print("result: "); Serial.print(resultColor); Serial.print("\n\n\n");
    uint32_t adjustedColor = Wheel(resultColor);
    internal.setPixelColor(0, adjustedColor);
    internal.show();
    strip.setPixelColor(0, adjustedColor);
    strip.setPixelColor(1, adjustedColor);
    strip.show();
    priorColor = resultColor;
  }

  delay(50);
}

byte toByte(float heading) {
  float rat = (heading / 360) * 255;
  return uint16_t(rat) & 255;
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
