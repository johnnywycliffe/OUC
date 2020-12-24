//Used to clear LED strips during testing

#include <FastLED.h>
#define NUM_LEDS 100
#define DATA_PIN 2
CRGB leds[NUM_LEDS];
void setup() { 
     FastLED.addLeds<WS2812, 2, BRG>(leds, NUM_LEDS);  // GRB ordering is typical
}
void loop() { 
  // Now turn the LED off, then pause
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(600);
}
