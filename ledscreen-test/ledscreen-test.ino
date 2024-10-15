#include <Wire.h>
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  Wire.beginTransmission(0);
  Wire.write(0xFF);
  Wire.endTransmission(0);
}

void loop() {
  Wire.beginTransmission(0);
  Wire.write(0xFF);
  Wire.endTransmission(0);
  delay(500);
}