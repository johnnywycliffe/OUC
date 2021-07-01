#include <FastLED.h>
/*=============================================================================================
 * LED pattern - default (Can be changed in settings)
 *      18                          32|33                           47
 *       |                            V                             |
 *       # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
 * 17 > #                                                           # < 48
 *      #                         PASSENGER                         #
 *      #                                                           #
 *      #     LEDs are represented a "#"                            #
 *      #                                                           #
 *      #                                                           #
 *      # F   0 is driver corner of car                             #
 *      # R                                                      R  #
 *  9 > # O                                                      E  # < 56
 *  8 > # N   Numbers are positioned along string                A  # < 57
 *      # T                                                      R  #
 *      #                                                           #
 *      #     Long sides are 30 LEDS long                           #
 *      #                                                           #
 *      #                                                           #
 *      #     Short sides are 18 LEDS long                          #
 *      #                                                           #
 *  0 > #                         DRIVER                            # < 65
 *       # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
 *       |                          ^                              |
 *      95                        81|80                            66
 */

//Defines
#define FRONT_LED_PIN 5
#define RIGHT_LED_PIN 4
#define LEFT_LED_PIN 17
#define REAR_LED_PIN 16
#define SPARE1_LED_PIN 2
#define SPARE2_LED_PIN 15

//TODO: Move this to a global type scope for ease of use
//Declarations
enum ColorOrder { //LED color orders
  rgb, rbg, grb, gbr, brg, bgr
};

//TODO: refactor to not be entirely public
class LEDString {         //Individual strings of LEDs
public:
  bool reversed = false;  //If LED strip is reversed.
  uint8_t ledCount;       //Number of LEDs on this strip
  int startPos;           //Position in main array
  ColorOrder cOrder;      //Order of LEDs
  //Member functions
  void setLEDStripData(uint8_t, ColorOrder, bool);
};

//Initialize indiivdual LED string
void LEDString::setLEDStripData(uint8_t lCount, ColorOrder cO=rgb, bool rev=false){
  ledCount = lCount;
  cOrder = cO;
  reversed = rev;
}

class LEDManager {              //LED manager need sot contain all the of the LED control code
  //Members
  LEDString frontLED;
  LEDString rightLED;
  LEDString rearLED;
  LEDString leftLED;
  LEDString spare1LED;
  LEDString spare2LED;
  CRGB *ledArr;                 //Main FastLED class
  int totalLEDs;                //Number of total LEDS
  //Private functions
  void setupLEDs();             //Function to run to instantiate LEDs in memory
  void colorSorter(CRGB,int);
public:
  void initLEDs(uint8_t*,ColorOrder*,bool*);  //Function settings are passed to to set up for initialization
  void updateLEDs(CRGB*);                     //Updates all LEDs in system
  LEDString* generateConfig();                //Returns a list of data to store
  //Getters
  int getTotalLength(){return totalLEDs;}
  int getUnderglowLength();
  int getSpare1Length(){return spare1LED.ledCount;}
  int getSpare2Length(){return spare2LED.ledCount;}
  CRGB* getLEDArr(){return ledArr;}
};

//LED Initialization
//Initialized LEDs to be set up.
void LEDManager::initLEDs(uint8_t *lCounts, ColorOrder *cOs, bool *rev){
  //Load data into strips
  frontLED.setLEDStripData(lCounts[0],cOs[0],rev[0]);
  rightLED.setLEDStripData(lCounts[1],cOs[1],rev[1]);
  rearLED.setLEDStripData(lCounts[2],cOs[2],rev[2]);
  leftLED.setLEDStripData(lCounts[3],cOs[3],rev[3]);
  spare1LED.setLEDStripData(lCounts[4],cOs[4],rev[4]);
  spare2LED.setLEDStripData(lCounts[5],cOs[5],rev[5]);
  
  //Initialize Underglow object
  totalLEDs = 0;
  for(int i=0;i<6;i++){
    totalLEDs+=lCounts[i];
  }
  ledArr = new CRGB[totalLEDs];
  
  //Run main LED initializer
  setupLEDs();
}

//LED Setup - Only run on init
//This function looks super nasty due to the way FastLED templates are set up, where the pin 
//is set during compile time, rather than run time, meaning 36 copy and pasted statements.
void LEDManager::setupLEDs(){
  int LEDTotal = 0;
  switch(frontLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,RGB>(ledArr,LEDTotal,frontLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,RBG>(ledArr,LEDTotal,frontLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,GRB>(ledArr,LEDTotal,frontLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,GBR>(ledArr,LEDTotal,frontLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,BRG>(ledArr,LEDTotal,frontLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,BGR>(ledArr,LEDTotal,frontLED.ledCount);
      break;
  }
  LEDTotal += frontLED.ledCount;
  rightLED.startPos = LEDTotal;
  switch(rightLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,RGB>(ledArr,LEDTotal,rightLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,RBG>(ledArr,LEDTotal,rightLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,GRB>(ledArr,LEDTotal,rightLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,GBR>(ledArr,LEDTotal,rightLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,BRG>(ledArr,LEDTotal,rightLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,BGR>(ledArr,LEDTotal,rightLED.ledCount);
      break;
  }
  LEDTotal += rightLED.ledCount;
  rearLED.startPos = LEDTotal;
  switch(rearLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,REAR_LED_PIN,RGB>(ledArr,LEDTotal,rearLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,REAR_LED_PIN,RBG>(ledArr,LEDTotal,rearLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,REAR_LED_PIN,GRB>(ledArr,LEDTotal,rearLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,REAR_LED_PIN,GBR>(ledArr,LEDTotal,rearLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,REAR_LED_PIN,BRG>(ledArr,LEDTotal,rearLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,REAR_LED_PIN,BGR>(ledArr,LEDTotal,rearLED.ledCount);
      break;
  }
  LEDTotal += rearLED.ledCount;
  leftLED.startPos = LEDTotal;
  switch(leftLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,RGB>(ledArr,LEDTotal,leftLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,RBG>(ledArr,LEDTotal,leftLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,GRB>(ledArr,LEDTotal,leftLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,GBR>(ledArr,LEDTotal,leftLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,BRG>(ledArr,LEDTotal,leftLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,BGR>(ledArr,LEDTotal,leftLED.ledCount);
      break;
  }
  LEDTotal += leftLED.ledCount;
  spare1LED.startPos = LEDTotal;
  switch(spare1LED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,RGB>(ledArr,LEDTotal,spare1LED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,RBG>(ledArr,LEDTotal,spare1LED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,GRB>(ledArr,LEDTotal,spare1LED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,GBR>(ledArr,LEDTotal,spare1LED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,BRG>(ledArr,LEDTotal,spare1LED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,BGR>(ledArr,LEDTotal,spare1LED.ledCount);
      break;
  }
  LEDTotal += spare1LED.ledCount;
  spare2LED.startPos = LEDTotal;
  switch(spare2LED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,RGB>(ledArr,LEDTotal,spare2LED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,RBG>(ledArr,LEDTotal,spare2LED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,GRB>(ledArr,LEDTotal,spare2LED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,GBR>(ledArr,LEDTotal,spare2LED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,BRG>(ledArr,LEDTotal,spare2LED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,BGR>(ledArr,LEDTotal,spare2LED.ledCount);
      break;
  }
  LEDTotal += spare2LED.ledCount;
}

//Displays all pixels from array
void LEDManager::updateLEDs(CRGB *arr){
  for(int i = 0; i < totalLEDs; i++){
    colorSorter(arr[i],i); //Inverts pixels if needed
  }
  FastLED.show();
}

//Reverses the order of LEDs on a per strip basis
void LEDManager::colorSorter(CRGB color, int position){
  //This function runs once per LED update. To keep comparisons down, Checks reversed first. 
  if(frontLED.reversed && position < frontLED.ledCount){
    ledArr[((frontLED.startPos+frontLED.ledCount-1)-(position-frontLED.startPos))] = color;
  } else if (rightLED.reversed && position < rightLED.ledCount + rightLED.startPos){
    ledArr[((rightLED.startPos+rightLED.ledCount-1)-(position-rightLED.startPos))] = color;
  } else if (rearLED.reversed && position < rearLED.ledCount + rearLED.startPos){
    ledArr[((rearLED.startPos+rearLED.ledCount-1)-(position-rearLED.startPos))] = color;
  } else if (leftLED.reversed && position < leftLED.ledCount + leftLED.startPos){
    ledArr[((leftLED.startPos+leftLED.ledCount-1)-(position-leftLED.startPos))] = color;
  } else { //Not flipped, one of the spare strips.
    ledArr[position] = color;
  } 
}

//Calculate and return Underglow length
int LEDManager::getUnderglowLength(){
  return (totalLEDs - (spare1LED.ledCount + spare2LED.ledCount));
}

//======================Pattern=============================
enum Pattern {
  narrow, medium, large, wide, halfandhalf, quarters, animation, solid
};

enum Animation {
  staticLEDs, rotateCW, rotateCCW
};
//Pattern class
class LEDPattern {
  CRGBPalette16 RGBP;
  int offsetPos; //Offest from driver side corner 
  uint8_t brightness; //Overall brightness of LEDs
  int leng; //Total number of LEDs in string
  Pattern patt; //Pattern to load into manager
  TBlendType tbt; //Blend mode
public:
  //Setters
  void setPattern(Pattern p){patt = p;}
  void setPalette(CRGBPalette16 c){RGBP = c;}
  void setOffsetPos(int op){offsetPos = op;}
  void setPatternBrightness(uint8_t b){ brightness = b;}
  void setLength(int l){leng = l;}
  void setBlendMode(bool);
  //Getters
  Pattern getPattern(){return patt;}
  CRGBPalette16 getPalette(){return RGBP;}
  int getOffsetPos(){return offsetPos;}
  int getPatternBrightness(){return brightness;}
  int getLength(){return leng;}
  TBlendType getBlendMode(){return tbt;}
};

void LEDPattern::setBlendMode(bool linear){
  if(linear){
    tbt = LINEARBLEND;
  } else {
    tbt = NOBLEND;
  }
}

//====================Pattern Manager=======================
class PatternManager {
  CRGB* prevRound; //Stores last frame for reference
public:
  void initializePattern(CRGB,int,LEDPattern);
  void updatePattern(CRGB,int,LEDPattern);
};

//Setup a pattern by erasing the old one.
void PatternManager::initializePattern(CRGB arr, int startPos, LEDPattern lPatt){
  Pattern p = lPatt.getPattern();
  int paletteIndex = 0;
  int len = startPos+lPatt.getLength();
  for(int i=startPos;i<len;i++){ //Start at correct pos and don't interfere with other strings of LEDs
    switch(p){
      default:
      case solid:{
        arr[i] = ColorFromPalette(lPatt.getPalette(),paletteIndex,
          lPatt.getPatternBrightness(), lPatt.getBlendMode()); 
        paletteIndex += round((float)256.0/len);
        break; 
      }
      case narrow:{ 
        int off = (i + lPatt.getOffsetPos()) % len;
        if(i%2 == 0){ //Every other LED
          arr[off] = ColorFromPalette(lPatt.getPalette(),paletteIndex,
            lPatt.getPatternBrightness(), lPatt.getBlendMode());
        }
        paletteIndex += round(256.0/len);
        break;
      }
      case medium:{
        //do nothing
        break;
      }
    }
  }
}
/*
void PatternManager::updatePattern(CRGB arr, int startPos ,LEDPattern ledPattern){
  for(int i=startPos;i<(startPos+ledPattern.getLength();i++)){ //Start at correct pos and don't interfere with other strings of LEDs
    
  }
}
*/
//Functions to load special pattern/animation types
/*pacifica, risingflames, twinklefox,
murica, colorpop, splatter, drip, christmas, valentines, shamrock, halloween*/
//Functions for PID patterns
//Functions for turns signals and brakes
//step function

//====================LED Preset===========================
class LEDPreset {
  //Common instances
  LEDManager* lMan;
  PatternManager* pMan;
  //Per-Pattern
  LEDPattern patt[3];
public:
  LEDPreset(LEDManager*,PatternManager*);
  void SetPattern(uint8_t,Pattern,CRGB,int,uint8_t);
  //TODO void Run(){} //Update LEDs one frame
};

//Initialization 
//TODO: Fix magic numbers
LEDPreset::LEDPreset(LEDManager* lManPtr, PatternManager* pManPtr){
  lMan = lManPtr;
  pMan = pManPtr;
  //Length determined by hardware position, cannot be updated without reset
  patt[0].setLength(lMan->getUnderglowLength());
  patt[1].setLength(lMan->getSpare1Length());
  patt[2].setLength(lMan->getSpare2Length());
}

//Update a pattern
void LEDPreset::SetPattern(uint8_t string,Pattern pattern,CRGB palette,int offsett,uint8_t brightness){
  patt[string].setPattern(pattern);
  patt[string].setPalette(palette);
  patt[string].setOffsetPos(offsett);
  patt[string].setPatternBrightness(brightness);
}

//=======================TEST CODE=========================
LEDManager lMan;
LEDManager* lManPtr = &lMan;

//Test
void setup(){
  Serial.begin(115200);
  //This data should be loaded from EEPROM
  uint8_t l[] = {100,30,18,30,0,0};
  ColorOrder c[] = {brg,brg,brg,brg,brg,brg};
  bool r[] = {true,false,false,false,false,false};
  //Close to actual init code
  lMan.initLEDs(l,c,r);
  Serial.println(lMan.getUnderglowLength());
}

void loop(){
  //lMan.updateLEDs();
}
