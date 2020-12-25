#include <Button.h>
//#include <FastLED.h>
#include "SSD1306Spi.h"
//#include "bitmaps.h"

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


enum State{ //Menu state
  main, settings, gauge1, pattern1, pattern2, pattern3, bluetooth, ledsetting1, ledsetting2,
  brakeandturn, brake, turn, color, pickpattern, animation, ledorder, ledcount, ledflip,
  autoshutoff, screenbright, palette, presetcolor1, presetcolor2, colornumset
};
//Refence for enums
State stateRef[] = {main, settings, gauge1, pattern1, pattern2, pattern3, bluetooth, ledsetting1, 
  ledsetting2, brakeandturn, brake, turn, color, pickpattern, animation, ledorder, ledcount, 
  ledflip, autoshutoff, screenbright, palette, presetcolor1, presetcolor2, colornumset};

enum Controls{up, down, left, right, select, back, nop}; //Controls

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

typedef void (*GFunc)(void*);           //Function pointer for MenuItem
typedef void (*displayFunc)(int);       //Screen display Function
typedef int (*controlFunc)(int&);       //Control scheme function

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
  
  //Maybe remove?
  //State next;            //Next menu to display (if needed)
  //int currVal;           //Value to print as current
public:
  Menu(){
  }
  ~Menu(){
    delete itemArr;
  }
  //Member Functions
  displayFunc disp; //function to call to display
  controlFunc cont;  //Control scheme
  void setItem(char *t, char *d, GFunc a, void* ai=NULL, GFunc p=NULL, void* pi=NULL){
    itemArr[len].set(t,d,a,ai,p,pi);  //Initialize MenuItem
    len++;                            //Incriment length
  }
  void runActive(int select){
    itemArr[select].active(itemArr[select].getActiveItem());
  }
  void runPassive(int select){
    itemArr[select].passive(itemArr[select].getPassiveItem());
  }
  //Setters
  void set(char *mt, State s, State p, displayFunc d, controlFunc c){
    menuTitle = mt;
    currState = s;
    prev = p;
    disp = d;
    cont = c;
    len = 0;
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
  State getPrev(){
    return prev;
  }
  int8_t getLen(){
    return len;
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
  
  
  char* getTitle(int pos){
    return itemArr[pos].title;
  }
  char* getDesc(int pos){
    return itemArr[pos].desc;
  }
  State getState(){
    return s;
  }
  int getCurrent(){
    return currVal;
  }*/
};

Button joyButton(JOYSTICK_BUTTON,PULLUP);
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);

Menu menu;
int selector = 0;

void setup(){
  //debug
  Serial.begin(115200);
  Serial.println("Start new test");
  //Buttons
  joyButton.begin();
  //Display
  if(!display.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.flipScreenVertically();
  display.setBrightness(255);
  display.display();
  // /debug
  setMenu(main);
  menu.disp(select);
}

void loop(){
  /* Menu theory of operation:
  based on state:
    - Input is interpreted differently
    - Active function runs, allowing LEDs to animate and brightness of screen to update, etc
    - Passive functions are called when button is pressed
    - Screen is updated according to state
  */
  
}

void setScreenBrightness(void* bPtr){
  int8_t *bri;
  bri = (int8_t*)bPtr;
  display.setBrightness(*bri);
}

void setMenu(State s){
  switch(s){ //Add options based off of
    default: //Something went wrong, throw an error
      pError("Not defined, returning to main");
    case main: //Main menu, default landing page
      menu.set("Main Menu",s,s,optionMenuDisplay,optionMenuControls);
      menu.setItem("Patterns","Choose an LED pattern and animation",setMenuW,&State.pattern1);
      break;
  }
}

//SetMenu wrapper
void setMenuW(void* ptr){
  setMenu(*(State*)ptr);
}

//Display options =================================================================================
void optionMenuDisplay(int select){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(63,0,128,menu.getMenuTitle());
  display.drawLine(0,10,128,10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0,10,128,menu.getSubtitle(select));
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0,26,128,menu.getDesc(select));
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
  while(getInput() == -1){
    continue;
  }
  //Clear screen
  display.clear();
  display.display();
}

//pError Wrapper
void pErrorW(void *ptr){
  pError(*(char*)ptr);
}

//Controls Options ================================================================================
//Controls for options menu
int optionMenuControls(int &sel){
  switch(getInput()){
    case up:
      sel--;                              //Decriment select / move up
      if(sel < 0) sel = menu.getLen()-1;  //Wrap if out of bounds
      break;
    case down:
      sel++;                              //Incriment select / move down
      if(sel >= menu.getLen()) sel = 0;   //Wrap if out of bounds
      break;
    case left:
    case back:
      setMenu(menu.getPrev());              //Back up a menu
    case right:
    case select:
      menu.runActive(sel);                //Run whatever is in MenuItem
      break;
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
    return select; //FIXME: Joybutton should return back
  }
  /* FIXME: Add in second button
  if(selectButton.pressed()){
    return select;
  }*/
  return nop;
}
