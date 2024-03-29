#include <Button.h>
//#include <FastLED.h>
#include "SSD1306Spi.h"
#include "bitmaps.h"

//Display
#define SPI_MOSI 23 
#define SPI_MISO 19
#define SPI_CLK 18
#define DISP_RESET 5
#define DISP_CS 4
#define DISP_DC 2

//LEDs
#define LED_FRONT 27
#define LED_REAR 26
#define LED_LEFT 13
#define LED_RIGHT 25
#define LED_SPARE1 15
#define LED_SPARE2 14
#define LEDSTRIPCAP 100
#define MAX_BRIGHTNESS 100

//Joysticks/buttons
#define JOYSTICK_X 32
#define JOYSTICK_Y 33
#define JOYSTICK_BUTTON 22
#define DEADZONELOW 300
#define DEADZONEHIGH 3800
#define MENUDELAY 200

#define NOTIMP (void*)"Not implemented"

enum State{ //Menu state
  main, settings, gauge1, pattern1, pattern2, pattern3, bluetooth, ledsetting1, ledsetting2,
  brakeandturn, brake, turn, color, pickpattern, animation, ledorder, ledcount, ledflip,
  autoshutoff, screenbright, palette, presetcolor1, presetcolor2, colornumset
};
enum Controls{up, down, left, right, select, back, nop}; //Controls
enum SelectedPattern {
  narrow, medium, large, wide, halfandhalf, quarters, dots, pacifica, risingflames, twinklefox,
  murica, colorpop, splatter, drip, christmas, valentines, shamrock, halloween, turnsignaloff, 
  solid, chase, solidnrear, chasenrear, solidnboth, chasenboth, brakesignaloff, onnodim, onwithdim
};
/*
//LED theory
//Three patterns: Active, Test, and Current
//Current is saved into EEPROM on update. It is what is loaded on reboot
//Test is the pattern that is generated when a new pattern is being developed. When it's done,
//saved into Current.
//Active is the pattern modified in real time. Can either be saved to Test or discarded.
//A spare Palette is needed to have colors loaded into it in real time

//Execution table varieties
//Actives
//Load a new menu, resets sel and runs setupmenu(State)
//Load a new menu with a limited number of options
//Load a new menu with a custom title
//Save active pattern to test pattern
//Save test pattern to current pattern - abstract to two?
//Load a new menu and throw an eror for caution (Wiring, OBD-II, etc)
//Load a new menu and update selected LED string
//Save RGB order into hardware struct
//Save LED count into hardware struct
//Save LED flip state into hardware struct
//Save turn signal pattern into device settings
//Save brake pattern into device settings
//Save autoshutoff state into device settings
//Bluetooth

//Passives
//Update color palette on active: Active is known, state and sel describe 
//Update screen brightness: deviceSettings is known, 
*/

//Templates
typedef void (*GFunc)(void*);       //Function pointer for MenuItem
typedef void (*displayFunc)(int);   //Screen display Function
typedef int (*controlFunc)(int&, uint8_t);   //Control scheme function

//Device settings
typedef struct {
  //Internal use
  char deviceID[10];
  bool firstboot = true;
  //User settings
  uint8_t brightness; //Screen brightness
  //LED settings
  bool autoshutoff; //If device automatically shuts off when driving
  SelectedPattern turn; //Turn signal behaviour
  SelectedPattern brake; //Brake light behaviour
} Settings;

// MenuItem class
class MenuItem{
  char *title;        //Title text
  char *desc;         //Description text
  void *activeItem;   //Item to return for active
  void *passiveItem;  //Item to return for passive
public:
  //Generics
  GFunc active;   //Function that requires button press
  GFunc passive;  //Function that runs in the background
  //Setters
  void set(char *t, char *d, GFunc a, void *ai=NULL, GFunc p = NULL, void *pi = NULL){
    title = t;
    desc = d;
    active = a;
    activeItem = ai;
    passive = p;
    passiveItem = pi;
  }
  //Getters
  char *getTitle(){
    return title;
  }
  char *getDesc(){
    return desc;
  }
  void* getActiveItem(){
    return activeItem;
  }
  void* getPassiveItem(){
    return passiveItem;
  }
};

// Menu class
class Menu{
  //Menu info
  char *menuTitle;  //Title of the menu
  State currState;  //Current state of the menu
  State prev;       //Menu on level above
  //Item Array
  MenuItem itemArr[100]; //Array of menu items
  uint8_t len;           //Actual length
  int8_t inputMode;      //If scrolling through items is desirable - change to bool?
public:
  Menu(){
  }
  ~Menu(){
    delete itemArr;
  }
  //Member Functions
  displayFunc disp; //function to call to display
  controlFunc cont;  //Control scheme
  void runActive(int select){
    itemArr[select].active(itemArr[select].getActiveItem());
  }
  void runPassive(int select){
    if(itemArr[select].passive != NULL){
      void *PassItem = itemArr[select].getPassiveItem();
      itemArr[select].passive(PassItem);
    }
  }
  //Setters
  void set(char *mt, State s, State p, displayFunc d, controlFunc c){
    menuTitle = mt;
    currState = s;
    prev = p;
    disp = d;
    cont = c;
    len = 0;
    inputMode = 0;
  }
  void setItem(char *t, char *d, GFunc a,  void* ai=NULL, GFunc p=NULL, void* pi=NULL){
    itemArr[len].set(t,d,a,ai,p,pi);  //Initialize MenuItem
    len++;                            //Increment length
  }
  void setLen(uint8_t l){
    len = l;
  }
  void setInputMode(int8_t m){
    inputMode = m;
  }
  //Getters
  char* getMenuTitle(){
    return menuTitle;
  }
  char* getSubtitle(int select){
    return itemArr[select].getTitle();
  }
  char* getDesc(int select){
    return itemArr[select].getDesc();
  }
  void* getAI(int select){
    return itemArr[select].getActiveItem();
  }
  State getPrev(){
    return prev;
  }
  uint8_t getLen(){
    return len;
  }
  int8_t getInputMode(){
    return inputMode;
  }
  /*
  State getState(){
    return currState;
  }
  /*
  void setLen(int l){ //used for int as maxVal, bool as true(1) or false(0), and progBar as maxVal
    len = l;
  }
  
  void setMenuTitle(char *mt){
    menuTitle = mt;
  }
  void setPrevMenu(State s){
    prev = s;
  }
  void setCurrent(int cv){
    currVal = cv;
  }
  //Get
  State getState(){
    return s;
  }
  int getCurrent(){
    return currVal;
  }*/
};

Button joyButton(JOYSTICK_BUTTON,INPUT_PULLUP);
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);
Settings deviceSettings;

//Menu init
Menu menu;
int selector = 0;
unsigned long menuTime = 0;
//Pointers for storing data
int *selPtr = &selector;
State *sPtr;
uint8_t optionsByte = 0;
uint8_t *optionsPtr = &optionsByte;

void setup(){
  //DEBUG
  Serial.begin(115200);
  Serial.println("Start new test");
  //Buttons
  joyButton.begin();
  //Display - Make optional
  if(!display.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.flipScreenVertically();
  //DEBUG
  deviceSettings.brightness = 255;
  //END DEBUG
  display.setBrightness(deviceSettings.brightness);
  display.display();
  //DEBUG
  setMenu(main);
  menu.disp(selector);
}

void loop(){
  if(millis() >= menuTime + MENUDELAY){
    menuTime = millis(); //Resets last time updated
    int result = menu.cont(selector,menu.getLen()); //Runs currently selected input
    switch (result){
      case -1:
        setMenu(menu.getPrev());
        break;
      case -2:
        switch (menu.getInputMode()){ //Make into if statement is no other mode required
          case 1:
            menu.runActive(0);
            break;
          default:
            menu.runActive(selector);
            break;
        }
        break;
      default:
        //do nothing
        break;
    }
    menu.disp(selector);
    switch (menu.getInputMode()){ //Make into if statement is no other mode required
      case 1:
        menu.runPassive(0);
        break;
      default:
        menu.runPassive(selector);
        break;
    }
  }
}

//Set state of menu, add options
void setMenu(State s){
  selector = 0;
  switch(s){ //Add options based off of
    default: //Something went wrong, throw an error
      pError("Not defined, returning to main");
    case main: //Main menu, default landing page
      menu.set("Main Menu",s,s,optionMenuDisplay,optionMenuControls); //Setup menu
      sPtr = new State[3]{screenbright,pattern1,gauge1};
      menu.setItem("Brightness","Set screen brightness",setMenuW,sPtr);
      menu.setItem("Gauges","Choose a gauge to display",setMenuW,sPtr+2);
      menu.setItem("Patterns","Choose an LED pattern and animation",setMenuW,sPtr+1);
      break;
    case gauge1:
      menu.set("Gauges Menu",s,main,optionMenuDisplay,optionMenuControls);
      menu.setItem("gauge 1","none",pErrorW,NOTIMP);
      menu.setItem("gauge 2","none",pErrorW,NOTIMP);
      break;
    case pattern1:
      menu.set("Pattern Menu",s,main,optionMenuDisplay,optionMenuControls);
      menu.setItem("Pattern 1","none",pErrorW,NOTIMP);
      menu.setItem("Pattern 2","none",pErrorW,NOTIMP);
      break;
    case screenbright:
      menu.set("Set Brightness",s,main,brightnessMenuDisplay,optionMenuControls);
      menu.setItem("Current: ","New: ",pErrorW,NOTIMP,setScreenBrightness,selPtr); //TODO: Add save func
      menu.setLen(9);
      menu.setInputMode(1);
      break;
  }
}

//SetMenu wrapper
void setMenuW(void* ptr){
  setMenu(*(State*)ptr);
}

//Set screen brightness
void setScreenBrightness(void *bPtr){
  uint8_t temp = (uint8_t)*(uint8_t*)bPtr; //Change form 16 bit to 8 bit and dereference
  if(temp == 8){
    temp = 255; //8 * 32 = 256, this catches it so no overflow occurs
  } else {
    temp *= 32;
  }
  display.setBrightness(temp);
}

//Display options =================================================================================
void optionMenuDisplay(int sel){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(63,0,128,menu.getMenuTitle());
  display.drawLine(0,10,128,10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0,10,128,menu.getSubtitle(sel));
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0,26,128,menu.getDesc(sel));
  display.display();
}

//Displays the integer menu
void brightnessMenuDisplay(int sel){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(63,0,128,menu.getMenuTitle());
  display.drawLine(0,10,128,10);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(63,10,128,"Current: "+String(deviceSettings.brightness));
  display.drawStringMaxWidth(63,36,128,"New: "+String(sel + 1));
  if(sel != 0){
    display.drawFastImage(60,30,8,8,upArrowBitmap);
  } 
  if (sel != menu.getLen()-1) {
    display.drawFastImage(60,53,8,8,downArrowBitmap);
  }
  display.display();
}

//Sends text to screen - Used for errors and some other popups
void pError(char *eText){
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(2,2,123,"NOTICE:");
  display.drawRect(0,0,128,64);
  display.drawLine(1,19,126,19);
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0,20,123,eText);
  display.display();
  // Wait for controls to be used
  delay(200);
  while(getInput() == nop){
    continue;
  }
  //Clear screen
  display.clear();
  display.display();
}

//pError Wrapper 
//FIXME: Char arrays are const, which means const void*, which mean, this doesn't work.
void pErrorW(void *ptr){
  Serial.println(*(char*)ptr); //DEBUG
  pError((char*)ptr); //pError expects pointer
}

//Controls Options ================================================================================
//Controls for options menu
//FIXME: Factor out menu, pass by ptr
int optionMenuControls(int &sel, uint8_t len){
  switch(getInput()){
    case up:
      sel--;                              //Decriment select / move up
      if(sel < 0) sel = len-1;  //Wrap if out of bounds
      break;
    case down:
      sel++;                              //Incriment select / move down
      if(sel >= len) sel = 0;   //Wrap if out of bounds
      break;
    case left:
    case back:
      return -1; //Back up a menu
    case right:
    case select:
      return -2; //Run active item
  }
  //Serial.println(select);
  return 0;
}

//Read buttons/joystick
Controls getInput(){
  int analogVal = analogRead(JOYSTICK_Y);
  if(analogVal >= DEADZONEHIGH){
    return down;
  } else if(analogVal <= DEADZONELOW){
    return up;
  }
  analogVal = analogRead(JOYSTICK_X);
  if(analogVal <= DEADZONELOW){
    return right;
  } else if(analogVal >= DEADZONEHIGH){
    return left;
  }
  if(joyButton.pressed()){
    return select;
  }
  return nop;
}
