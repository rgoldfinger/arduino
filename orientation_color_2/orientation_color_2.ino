#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoPixel internal = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_LSM303 lsm;

//Accel Minimums: -15.46  -10.87  -14.36
//Accel Maximums: 12.32  14.36  16.08
//Mag Minimums: -51.09  -79.18  -75.92
//Mag Maximums: 48.73  30.00  29.29
//

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
//  strip.begin();
//  strip.show();
  internal.begin();
  internal.show();
}

void loop() 
{
  lsm.read();


  
  float i, j, x, y, z, sum, deg;
  float heading;
  uint16_t adjustedColor;
 
  for(j=0; j<256; j++) {
    lsm.read();   
    x = lsm.magData.x;
    y = lsm.magData.y;
    z = lsm.magData.z;
    sum = x + y + z;

    heading = getHeading(lsm);
    deg = getDeg(lsm);

    
    Serial.print("X: "); Serial.print(x); Serial.print(" ");
    Serial.print("Y: "); Serial.print(y); Serial.print(" ");
    Serial.print("Z: "); Serial.print(z); Serial.print(" ");
    Serial.print("Sum: "); Serial.print(sum); Serial.print("\n");
    Serial.print("deg: "); Serial.print(deg); Serial.print("\n");
    Serial.print("heading: "); Serial.print(heading); Serial.print("\n");

    adjustedColor = Wheel(deg);
    internal.setPixelColor(0, adjustedColor);
//    internal.setPixelColor(0, internal.Color(lsm.magData.x, lsm.magData.y, lsm.magData.z));
    internal.show();
    
    delay(500);
  }
}

uint16_t getDeg(Adafruit_LSM303 lsm) {
  float x = lsm.magData.x;
  float y = lsm.magData.y;
  float z = lsm.magData.z;
  
  float Pi = 3.14159;
  
  // Calculate the angle of the vector y,x
  float heading = (atan2(y,x) * 180) / Pi;
  
  // Normalize to 0-360
  if (heading < 0)
  {
    heading = 360 + heading;
  }

  return heading;
}

  /* pitch: Rotation around the lateral axis (the wing span, 'Y axis'). -180<=pitch<=180)     */
  /* pitch is positive and increasing when moving upwards                                     */
  /*                                                                                          */
  /*                                 x                                                        */
  /*            pitch = atan(-----------------)                                               */
  /*                          sqrt(y^2 + z^2)                                                 */
  /* where:  x, y, z are returned value from accelerometer sensor                             */

//  t_pitch = event->acceleration.y * event->acceleration.y + event->acceleration.z * event->acceleration.z;
//  orientation->pitch = (float)atan2(event->acceleration.x, signOfZ * sqrt(t_pitch)) * 180 / PI;

uint16_t getHeading(Adafruit_LSM303 lsm) {
    float x, y, z;
    x = lsm.magData.x;
    y = lsm.magData.y;
    z = lsm.magData.z;

    float signOfZ = z >= 0 ? 1.0F : -1.0F;
    float pitch = y * y + z * z;
    float orientation = (float)atan2(x, signOfZ * sqrt(pitch)) * 180 / PI;
    Serial.print("X: "); Serial.print(x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(z); Serial.print("  ");
    Serial.print("pitch: "); Serial.print(pitch); Serial.print("  ");
    Serial.print("orientation: "); Serial.print(orientation); Serial.print("\n");

    return int(orientation);
  
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

