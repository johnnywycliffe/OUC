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
  CRGB *underglow;              //Main FastLED class
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
  underglow = new CRGB[totalLEDs];
  
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
      LEDS.addLeds<WS2811,FRONT_LED_PIN,RGB>(underglow,LEDTotal,frontLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,RBG>(underglow,LEDTotal,frontLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,GRB>(underglow,LEDTotal,frontLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,GBR>(underglow,LEDTotal,frontLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,BRG>(underglow,LEDTotal,frontLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,FRONT_LED_PIN,BGR>(underglow,LEDTotal,frontLED.ledCount);
      break;
  }
  LEDTotal += frontLED.ledCount;
  rightLED.startPos = LEDTotal;
  switch(rightLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,RGB>(underglow,LEDTotal,rightLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,RBG>(underglow,LEDTotal,rightLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,GRB>(underglow,LEDTotal,rightLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,GBR>(underglow,LEDTotal,rightLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,BRG>(underglow,LEDTotal,rightLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,RIGHT_LED_PIN,BGR>(underglow,LEDTotal,rightLED.ledCount);
      break;
  }
  LEDTotal += rightLED.ledCount;
  rearLED.startPos = LEDTotal;
  switch(rearLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,REAR_LED_PIN,RGB>(underglow,LEDTotal,rearLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,REAR_LED_PIN,RBG>(underglow,LEDTotal,rearLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,REAR_LED_PIN,GRB>(underglow,LEDTotal,rearLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,REAR_LED_PIN,GBR>(underglow,LEDTotal,rearLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,REAR_LED_PIN,BRG>(underglow,LEDTotal,rearLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,REAR_LED_PIN,BGR>(underglow,LEDTotal,rearLED.ledCount);
      break;
  }
  LEDTotal += rearLED.ledCount;
  leftLED.startPos = LEDTotal;
  switch(leftLED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,RGB>(underglow,LEDTotal,leftLED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,RBG>(underglow,LEDTotal,leftLED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,GRB>(underglow,LEDTotal,leftLED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,GBR>(underglow,LEDTotal,leftLED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,BRG>(underglow,LEDTotal,leftLED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,LEFT_LED_PIN,BGR>(underglow,LEDTotal,leftLED.ledCount);
      break;
  }
  LEDTotal += leftLED.ledCount;
  spare1LED.startPos = LEDTotal;
  switch(spare1LED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,RGB>(underglow,LEDTotal,spare1LED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,RBG>(underglow,LEDTotal,spare1LED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,GRB>(underglow,LEDTotal,spare1LED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,GBR>(underglow,LEDTotal,spare1LED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,BRG>(underglow,LEDTotal,spare1LED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,SPARE1_LED_PIN,BGR>(underglow,LEDTotal,spare1LED.ledCount);
      break;
  }
  LEDTotal += spare1LED.ledCount;
  spare2LED.startPos = LEDTotal;
  switch(spare2LED.cOrder){
    default:
    case rgb:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,RGB>(underglow,LEDTotal,spare2LED.ledCount);
      break;
    case rbg:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,RBG>(underglow,LEDTotal,spare2LED.ledCount);
      break;
    case grb:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,GRB>(underglow,LEDTotal,spare2LED.ledCount);
      break;
    case gbr:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,GBR>(underglow,LEDTotal,spare2LED.ledCount);
      break;
    case brg:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,BRG>(underglow,LEDTotal,spare2LED.ledCount);
      break;
    case bgr:
      LEDS.addLeds<WS2811,SPARE2_LED_PIN,BGR>(underglow,LEDTotal,spare2LED.ledCount);
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
    underglow[((frontLED.startPos+frontLED.ledCount-1)-(position-frontLED.startPos))] = color;
  } else if (rightLED.reversed && position < rightLED.ledCount + rightLED.startPos){
    underglow[((rightLED.startPos+rightLED.ledCount-1)-(position-rightLED.startPos))] = color;
  } else if (rearLED.reversed && position < rearLED.ledCount + rearLED.startPos){
    underglow[((rearLED.startPos+rearLED.ledCount-1)-(position-rearLED.startPos))] = color;
  } else if (leftLED.reversed && position < leftLED.ledCount + leftLED.startPos){
    underglow[((leftLED.startPos+leftLED.ledCount-1)-(position-leftLED.startPos))] = color;
  } else { //Not flipped, one of the spare strips.
    underglow[position] = color;
  } 
}

//Calculate and return Underglow length
int LEDManager::getUnderglowLength(){
  return (totalLEDs - (spare1LED.ledCount + spare2LED.ledCount));
}

//======================PATTERN TEST======================
//uint8_t offsetPos; //How offset from default position pattern is.
//CRGBPalette16 RGBP; //RGB color pallette
//uint8_t brightness; //Brightness level of LEDs
//uint8_t animSpd; //Animation speed
//uint8_t trackedPID; // OBD-II PID Datsa to be read (if relevant)
//SelectedPattern sp; //Current pattern used

enum Pattern {
  narrow, medium, large, wide, halfandhalf, quarters, dots 
};

//Pattern class
class LEDPattern {
  CRGB *leds; //Pattern to be manipulated
  CRGBPalette16 RGBP;
  int offsetPos; //Offest from driver side corner 
  uint8_t brightness; //Overall brightness of LEDs
  int leng; //Total numb er of LEDs in string
public:
  LEDPattern(int len){
    leds = new CRGB[len];
    leng = len;
  }
  ~LEDPattern(){
    delete leds;
  }
  //Setters
  void setPattern(Pattern);
  void setPalette(CRGBPalette16 c){RGBP = c;}
  void setPatternBrightness(uint8_t b){ brightness = b;}
  void setOffestPos(int op){offsetPos = op;}
  //Getters
  CRGB getPattern(){return *leds;}
  CRGB getPixel(int p){return leds[p];}
  CRGBPalette16 getPalette(){return RGBP;}
  int getOffsetPos(){return offsetPos;}
  int getPatternBrightness(){return brightness;}
  int getLength(){return leng;}
};

void LEDPattern::setPattern(Pattern p){
  switch(p){
    default:
    case narrow:
      //Using color pallet, adjusted by offset, to set up a pattern of LEDs
      //Use the number of LEDs to stretch palette to color spectrum
      //Only color the correct LEDs based on 
      break;
  }
}

class LEDAnimation {
  //Functions to load special pattern/animation types
  /*pacifica, risingflames, twinklefox,
  murica, colorpop, splatter, drip, christmas, valentines, shamrock, halloween*/
  //Functions for PID patterns
  //Functions for turns signals and brakes
  //step function
};

class LEDPreset { //Contains all pattern funcitions. One per preset.
  LEDPattern *underglow; //Ptr so len can be set later
  LEDAnimation *uAnimation;
  LEDPattern *spare1;
  LEDPattern *spare2;
  //Function to update and retrieve each of these.
 
public:
  void init(int,int,int); //Sets up LEDs
  CRGB* getPattern(CRGB*,int);
  void step(); //Move all animations forwards one step
};

void LEDPreset::init(int underglowLen, int spare1Len, int spare2Len){
  
}

//Returns all of the data
CRGB* LEDPreset::getPattern(CRGB* out, int len){
  int uL = underglow->getLength();
  int s1L = spare1->getLength();
  int s2L = spare2->getLength();
  for(int i=0; i < len; i++){
    if(i < uL){ out[i]=underglow->getPixel(i);}
    else if(i < s1L){ out[i]=spare1->getPixel(i-uL);}
    else if(i < s2L){ out[i]=spare2->getPixel(i-(uL+s1L));}
    else {break;} //End the loop even if buffer not full
  }
  return out;
}

//=======================TEST CODE=========================

LEDManager lMan;
LEDPreset *active;
LEDPreset preset1;
LEDPreset preset2;

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
