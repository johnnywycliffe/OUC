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

//TODO: Move these to a global type scope for ease of use
//Declarations
enum ColorOrder { //LED color orders
  rgb, rbg, grb, gbr, brg, bgr
};

enum Pattern {
  solid, narrow, medium, large, wide, dots, 
  animation //Animation is used only for items that don't allow a preset arrangement of pixels
};

enum Animation {
  staticLEDs, rotateCW, rotateCCW, fade1, fade2, faderand, cylon,     //Uses pattern
  colorpop, splatter, drip,                                           //Uses pattern, but needs special setup
  pacifica, flames, murica, valntine, shamrock, halloween, christmas, //Uses pattern, but needs psecial setup
  gauge1, gauge2, gauge3, gauge4, gauge5, gauge6, gauge7,             //Gauge clusters
  brakes1, brakes2, brakes3, signal1, signal2, signal3, signal4, signal5, signal6 //Car exclusive
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

//LED manager need sot contain all the of the LED control code
class LEDManager {              
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
  void updateLEDs(CRGB*);                          //Updates all LEDs in system
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
void LEDManager::updateLEDs(CRGB* arr){
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
//Pattern class
class LEDPattern {
  CRGBPalette16 RGBP;
  int offsetPos;      //Offest from driver side corner 
  uint8_t brightness; //Overall brightness of LEDs
  int leng;           //Total number of LEDs in string
  Pattern patt;       //Pattern to load into manager
  TBlendType tbt;     //Blend mode
  Animation anim;     //Animation apllied
public:
  //Setters
  void setPattern(Pattern p){patt = p;}
  void setPalette(CRGBPalette16 c){RGBP = c;}
  void setOffsetPos(int op){offsetPos = op;}
  void setPatternBrightness(uint8_t b){ brightness = b;}
  void setLength(int l){leng = l;}
  void setBlendMode(bool);
  void setAnimation(Animation a){anim = a;};
  //Getters
  Pattern getPattern(){return patt;}
  CRGBPalette16 getPalette(){return RGBP;}
  int getOffsetPos(){return offsetPos;}
  int getPatternBrightness(){return brightness;}
  int getLength(){return leng;}
  TBlendType getBlendMode(){return tbt;}
  Animation getAnimation(){return anim;}
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
  int len;
public:
  void createArr(int);
  void delArr();
  void initializePattern(CRGB*,int,LEDPattern);
  void updatePattern(CRGB*,int,LEDPattern);
  CRGB savePrevRound(CRGB*,CRGB*,int);
};

//Setup a pattern by erasing the old one.
void PatternManager::initializePattern(CRGB* arr, int startPos, LEDPattern lPatt){
  Pattern p = lPatt.getPattern();
  int len = startPos+lPatt.getLength();
  if(p != animation){
    for(int i=startPos;i<len;i++){ //Start at correct pos and don't interfere with other strings of LEDs
      int off = (i + lPatt.getOffsetPos()) % len; //Rotate pattern around offset
      int paletteIndex = round(i*(256.0/len));    //get correct color index from palette
      int color = 0;  //Color
      switch(p){
        default:
        case solid:{                      //All LEDs one color
          color = paletteIndex;
          break; 
        }
        case narrow:{                     //Every other LED is on
          if(i%2 == 0){
            color = paletteIndex;
          } else {
            color = -1;
          }
          break;
        }
        case medium:{                     //Every 2 leds are on
          if(i%4 > 1){
            color = paletteIndex;
          } else {
            color = -1;
          }
          break;
        }
        case large:{                      //Every 3 leds are on
          if(i%6 > 2){
            color = paletteIndex;
          } else {
            color = -1;
          }
          break;
        }
        case wide:{                       //Every 6 leds are on
          if(i%12 > 5){                   
            color = paletteIndex;
          } else {
            color = -1;
          }
          break;
        }
        case dots:{                       //Smaller, discrete dots
          if(i%6 == 0){
            color = paletteIndex;
          } else {
            color = -1;
          }
          break;
        }
      }
      //Apply color from palette to pixel
      if(color >= 0){ 
        //Serial.println(color);
        arr[off] = ColorFromPalette(lPatt.getPalette(),color,
              lPatt.getPatternBrightness(), lPatt.getBlendMode());
      } else {
        //Serial.println("Black");
        arr[off] = CRGB::Black;
      }
    }
  } else {
    //TODO: Animation setup
  }
}

//Updates the LEDs
void PatternManager::updatePattern(CRGB *arr, int startPos, LEDPattern lPatt){
  Animation anim = lPatt.getAnimation();
  int len = startPos+lPatt.getLength();
  switch(anim){
    default:
    case staticLEDs:
      //Do nothing
      break;
    case rotateCW:
      //savePrevRound(arr,prevRound,len);
      for(int i=0;i<len;i++){
        //arr[i] = prevRound[(i+1)%len];
      }
      break;
  }
}

//Copy previous state into array for use
//FIXME: For wahtever reason, this crashes the device
CRGB PatternManager::savePrevRound(CRGB *src, CRGB *dst, int len){
  //memcpy(dst, src, sizeof(src[0])*len);
  for (int i=0;i<len;i++){
    //Serial.println(i);
    //*dst++ = *src++;
    //Serial.println(*src);
  }
}

void PatternManager::createArr(int l){
  prevRound = new CRGB[len];
  len = l;
}

void PatternManager::delArr(){
  delete prevRound;
}

//====================LED Preset===========================
class LEDPreset {
  //Common instances
  LEDManager* lMan;
  PatternManager* pMan;
  //Per-Pattern
public: //delete
  LEDPattern patt[3]; //underglow = 0, spare1 = 1, spare2 = 2
public:
  LEDPreset(LEDManager*,PatternManager*);
  void SetPattern(uint8_t,Pattern,CRGBPalette16,int,uint8_t,bool,Animation);
  //TODO: void Start() //Init pattern thru patternmanager
  //TODO: void Run()   //Update LEDs one frame thru patternmanager
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
void LEDPreset::SetPattern(uint8_t string,Pattern pattern,CRGBPalette16 palette,int offsett,
  uint8_t brightness,bool tbt,Animation anim){
  patt[string].setPattern(pattern);
  patt[string].setPalette(palette);
  patt[string].setOffsetPos(offsett);
  patt[string].setPatternBrightness(brightness);
  patt[string].setBlendMode(tbt);
  patt[string].setAnimation(anim);
}

//=======================TEST CODE=========================
LEDManager lMan;
PatternManager pMan;
CRGB* leds;
LEDPreset* active;

//Test
void setup(){
  Serial.begin(115200);
  
  //This data should be loaded from EEPROM
  uint8_t l[] = {97,1,1,1,0,0};
  ColorOrder c[] = {brg,brg,brg,brg,brg,brg};
  bool r[] = {false,false,false,false,false,false};
  
  //Close to actual init code
  lMan.initLEDs(l,c,r);
  int len = lMan.getTotalLength();
  leds = new CRGB [len];
  active = new LEDPreset(&lMan,&pMan);
  pMan.createArr(len);
  
  //Test code
  active->SetPattern(0,solid,RainbowColors_p,0,32,true,staticLEDs);
  pMan.initializePattern(leds,0,active->patt[0]);
  
  pMan.updatePattern(leds,0,active->patt[0]);
}

void loop(){
  //pMan.updatePattern(leds,0,active->patt[0]);
  lMan.updateLEDs(leds);
}
