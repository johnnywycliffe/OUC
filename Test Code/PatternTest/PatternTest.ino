//For testing new patterns and pattern integration

#include <FastLED.h>

//Pins - LED outputs
#define LED_FRONT 27
#define LED_REAR 26
#define LED_LEFT 13
#define LED_RIGHT 25
#define LED_SPARE1 15
#define LED_SPARE2 14

#define MAX_BRIGHTNESS 100

enum ColorOrder {
  rgb, rbg, grb, gbr, brg, bgr
};
enum SelectedPattern {
  narrow, medium, large, wide, halfandhalf, quarters, dots, pacifica, risingflames, twinklefox,
  murica, colorpop, splatter, drip, christmas, valentines, shamrock, halloween
};
enum SelectedString {
  front, rear, passenger, driver, spare1, spare2
};

// Lighting system struct
typedef struct {
  uint8_t offsetPos; //How offset from default position pattern is.
  CRGBPalette16 RGBP; //RGB color pallette
  uint8_t brightness; //Brightness level of LEDs
  uint8_t animSpd; //Animation speed
  uint8_t trackedPID; // OBD-II PID Datsa to be read (if relevant)
  SelectedPattern sp; //Current pattern used
} LEDPatterns;

// Hardware definition struct
typedef struct {
  bool reversed = true; //True for backwards, false for forwards
  uint8_t ledCount; //num of LEDs on strip.
  int startPos; //First led array val
  ColorOrder order; //BRG for test code
} LEDHardware;

LEDPatterns active;
SelectedString sLEDString;

LEDHardware frontLH;
LEDHardware rightLH;
LEDHardware rearLH;
LEDHardware leftLH;
LEDHardware spare1LH;
LEDHardware spare2LH;

CRGB *underglow;

int LEDTotal = 0;

bool preset = false;

//Test code
int cntr = 0;

void setup() {
  Serial.begin(115200);
  setupLEDs();
}

void loop() { 
  //Moving test pattern
  for(int i = 0; i < 18; i++) {
    colorSorter(CRGB::Red,i);
    FastLED.show();
    colorSorter(CRGB::Black,i);
    delay(100);
  }
  /*
  //Simple rainbow
  for(int i =0; i < LEDTotal; i++){
    colorSorter(CHSV(cntr,225,128), i);
  }
  FastLED.show();
  cntr++;*/
}

//Example:
// Right led strip is reversed. What should be LED 20 is actually 45.
// First, determine which LED strip we're talking about
// Determine if strip is flipped. If not, pass through.
// Else, use: (startPos+ledCount-1)-(led-startPos)
// in example, this is (18+30-1)-(20-18) = 47-2 = 45
// Second example as proof. thirs strip is reversed, what does LED 60 become?
// (48+18-1)-(60-48) = 65 - 12 = 53 which checks out against default pattern

// Flips array for LED strips installed "backwards"
void colorSorter(CRGB color, int led){
  if(led < frontLH.ledCount && frontLH.reversed){
    underglow[((frontLH.startPos+frontLH.ledCount-1)-(led-frontLH.startPos))] = color;
  } else if (led < rightLH.ledCount + rightLH.startPos && rightLH.reversed){
    underglow[((rightLH.startPos+rightLH.ledCount-1)-(led-rightLH.startPos))] = color;
  } else if (led < rearLH.ledCount + rearLH.startPos && rearLH.reversed){
    underglow[((rearLH.startPos+rearLH.ledCount-1)-(led-rearLH.startPos))] = color;
  } else if (led < leftLH.ledCount + leftLH.startPos && leftLH.reversed){
    underglow[((leftLH.startPos+leftLH.ledCount-1)-(led-leftLH.startPos))] = color;
  } else {
    underglow[led] = color;
  } 
}


//On reboot, initialize LEDS
void setupLEDs(){
  
  if(preset){
    //Load from EEPROM
    
  } else {
    //Load test strip setup
    LEDS.setBrightness(MAX_BRIGHTNESS);
    frontLH.order = brg;
    frontLH.ledCount = 18;
    rightLH.order = brg;
    rightLH.ledCount = 30;
    rearLH.order = brg;
    rearLH.ledCount = 18;
    leftLH.order = brg;
    leftLH.ledCount = 30;
    spare1LH.order = brg;
    spare1LH.ledCount = 2;
    spare2LH.order = brg;
    spare2LH.ledCount = 2;
  }
  LEDTotal = frontLH.ledCount + rightLH.ledCount + rearLH.ledCount + 
  leftLH.ledCount + spare1LH.ledCount + spare2LH.ledCount;
  underglow = new CRGB[LEDTotal];
  //based on rgb order, initialize front LEDs
  LEDTotal = 0;
  switch(frontLH.order){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LED_FRONT,RGB>(underglow,LEDTotal,frontLH.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LED_FRONT,RBG>(underglow,LEDTotal,frontLH.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LED_FRONT,GRB>(underglow,LEDTotal,frontLH.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LED_FRONT,GBR>(underglow,LEDTotal,frontLH.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LED_FRONT,BRG>(underglow,LEDTotal,frontLH.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LED_FRONT,BGR>(underglow,LEDTotal,frontLH.ledCount);
      break;
  }
  LEDTotal += frontLH.ledCount;
  rightLH.startPos = LEDTotal;
  switch(rightLH.order){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LED_RIGHT,RGB>(underglow,LEDTotal,rightLH.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LED_RIGHT,RBG>(underglow,LEDTotal,rightLH.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LED_RIGHT,GRB>(underglow,LEDTotal,rightLH.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LED_RIGHT,GBR>(underglow,LEDTotal,rightLH.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LED_RIGHT,BRG>(underglow,LEDTotal,rightLH.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LED_RIGHT,BGR>(underglow,LEDTotal,rightLH.ledCount);
      break;
  }
  LEDTotal += rightLH.ledCount;
  rearLH.startPos = LEDTotal;
  switch(rearLH.order){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LED_REAR,RGB>(underglow,LEDTotal,rearLH.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LED_REAR,RBG>(underglow,LEDTotal,rearLH.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LED_REAR,GRB>(underglow,LEDTotal,rearLH.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LED_REAR,GBR>(underglow,LEDTotal,rearLH.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LED_REAR,BRG>(underglow,LEDTotal,rearLH.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LED_REAR,BGR>(underglow,LEDTotal,rearLH.ledCount);
      break;
  }
  LEDTotal += rearLH.ledCount;
  leftLH.startPos = LEDTotal;
  switch(leftLH.order){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LED_LEFT,RGB>(underglow,LEDTotal,leftLH.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LED_LEFT,RBG>(underglow,LEDTotal,leftLH.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LED_LEFT,GRB>(underglow,LEDTotal,leftLH.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LED_LEFT,GBR>(underglow,LEDTotal,leftLH.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LED_LEFT,BRG>(underglow,LEDTotal,leftLH.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LED_LEFT,BGR>(underglow,LEDTotal,leftLH.ledCount);
      break;
  }
  LEDTotal += leftLH.ledCount;
  spare1LH.startPos = LEDTotal;
  switch(spare1LH.order){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LED_SPARE1,RGB>(underglow,LEDTotal,spare1LH.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LED_SPARE1,RBG>(underglow,LEDTotal,spare1LH.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LED_SPARE1,GRB>(underglow,LEDTotal,spare1LH.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LED_SPARE1,GBR>(underglow,LEDTotal,spare1LH.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LED_SPARE1,BRG>(underglow,LEDTotal,spare1LH.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LED_SPARE1,BGR>(underglow,LEDTotal,spare1LH.ledCount);
      break;
  }
  LEDTotal += spare1LH.ledCount;
  spare2LH.startPos = LEDTotal;
  switch(spare2LH.order){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LED_SPARE2,RGB>(underglow,LEDTotal,spare2LH.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LED_SPARE2,RBG>(underglow,LEDTotal,spare2LH.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LED_SPARE2,GRB>(underglow,LEDTotal,spare2LH.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LED_SPARE2,GBR>(underglow,LEDTotal,spare2LH.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LED_SPARE2,BRG>(underglow,LEDTotal,spare2LH.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LED_SPARE2,BGR>(underglow,LEDTotal,spare2LH.ledCount);
      break;
  }
  LEDTotal += spare2LH.ledCount;
}
