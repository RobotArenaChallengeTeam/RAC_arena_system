#include "FastLED.h"

#define NUM_LEDS 1  // Numero de leds
#define PIN 10      // PIN ARDUINO

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
}

void loop() {
  char cmd = Serial.read();
  Serial.print("c: ");
  Serial.println(cmd);
  switch (cmd) {
    case 'w':
      leds[0] = CRGB(255, 255, 255);
      FastLED.show();
      break;
    case 'r':
      leds[0] = CRGB(255, 0, 0);
      FastLED.show();
      break;
    case 'g':
      leds[0] = CRGB(0, 255, 0);
      FastLED.show();
      break;
    case 'b':
      leds[0] = CRGB(0, 0, 255);
      FastLED.show();
      break;
    case 'q' :
      leds[0] = CRGB(0, 0, 0);
      FastLED.show();
      break;
  }
}