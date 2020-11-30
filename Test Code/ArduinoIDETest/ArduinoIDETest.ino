#include <Button.h>
#include "SSD1306Spi.h"

#define SPI_MOSI 23 
#define SPI_MISO 19
#define SPI_CLK 18
#define DISP_RESET 5
#define DISP_CS 4
#define DISP_DC 2
#define JOYSTICK_X 26
#define JOYSTICK_Y 27
#define JOYSTICK_BUTTON 22

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SELECT 4
#define BACK 5

#define DEADZONELOW 300
#define DEADZONEHIGH 3800

//State Enum
enum State{
  main, settings, gauges, patterns, bluetooth, ledsettings
};

// MenuItem Struc for objects
typedef struct{
  char *title; //Title text
  char *desc; //Description text
  int8_t pid; //For storing PIDs
} MenuItem;

// Menu class
class Menu{
  MenuItem itemArr[100];
  int8_t len;
  State s;
public:
  Menu(){
  }
  ~Menu(){
    delete itemArr;
  }
  void setState(State state){
    s = state;
  }
  int8_t getLen(){
    return len;
  }
  State getState(){
    return s;
  }
  void setItem(int8_t pos, char *t, char *d, int8_t PID = 0){
    itemArr[pos].title = t;
    itemArr[pos].desc = d;
    itemArr[pos].pid = PID;
    len++;
  }
  char* getTitle(int pos){
    return itemArr[pos].title;
  }
  char* getDesc(int pos){
    return itemArr[pos].desc;
  }
  int8_t getPID(int pos){
    return itemArr[pos].pid;
  }
  void resetLen(){
    len = 0;
  }
};

Button joyButton(JOYSTICK_BUTTON,PULLUP);
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);

//Menu init
Menu mMenu;
State s;

int sel = 0;

void setup() {
  Serial.begin(115200);
  joyButton.begin();
  //Display
  if(!display.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.flipScreenVertically();
  display.setContrast(255);
  s == main;
  setupMenu(s);
  pError("Press any button to continue");
}

void loop() {
  int result = menuSelect(&mMenu, sel);
  if(result==-1){
    sel = 0;
    s == main;
    setupMenu(s);
  }
  ShowMenu(&mMenu, sel);
  delay(250);
}

//Set menu options
void setupMenu(State sel){
  mMenu.resetLen();
  mMenu.setState(sel);
  switch(sel){
    default:
      pError("Not defined, returning to main");
    case main:
      mMenu.setItem(0,"Display","Display current selection.");
      mMenu.setItem(1,"Gauges","Choose a Gauge.");
      mMenu.setItem(2,"Animations","Choose an LED Animation.");
      mMenu.setItem(3,"Settings","Display current settings.");
      break;
    case settings:
      mMenu.setItem(0,"Bluetooth","Set up bluetooth");
      mMenu.setItem(1,"LED Settings","Set up and tweak LEDs");
      mMenu.setItem(2,"Device Info","Licenses, credits, stuff like that.");
      break;
  }
  ShowMenu(&mMenu, sel);
}

//Navigate menus
int menuSelect(Menu *m, int &select){
  //Get input
  int8_t input = getInput();
  switch(input){
    case UP:
      select--;
     //Wrap if out of bounds
      if(select < 0){
        select = m->getLen()-1;
      }
      break;
    case DOWN:
      select++;
      //Wrap if out of bounds
      if(select >= m->getLen()){
        select = 0; 
      }
      break;
    case LEFT:
    case BACK:
      //exit back to parent menu
      Serial.println("Back up");
      return -1;
      break;
    case RIGHT:
    case SELECT:
      //Send state to execution table for further input
      return executionTable(m->getState(),select);
      break;
  }
  Serial.println(select);
  return 0;
}

//Displays menu item
void ShowMenu(Menu *m, int select){
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0,12,128,m->getTitle(select));
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0,28,128,m->getDesc(select));
  display.display();
}

//Defines menu behaviour -not complete... at all
int executionTable(State s, int &sel){
  switch(s){ //Switch based on menu
    case main:
      switch(sel){//Switch based on selection
        case 0: //Display (Magic numbers)
          //Show present configuration
          pError("TODO");
          break;
        case 1: //Gauge menu selected (Magic numbers)
          sel = 0;
          setupMenu(s=gauges);
          break;
        case 2: //Pattern menu selected (Magic numbers)
          sel = 0;
          setupMenu(s=patterns);
          break;
        case 3: //Settings menu selected (Magic numbers)
          sel = 0;
          setupMenu(s=settings);
          break;
        default:
          sel = 0;
          pError("Selection does not exist");
          return -1;
          break;
      }
      break;
    //User settings menu
    case settings:
      switch(sel){
        case 0: //Set up bluetooth
          sel = 0;
          setupMenu(s=bluetooth);
          break;
        case 1: //Set LED behavior
          sel = 0;
          setupMenu(s=ledsettings);
          break;
        case 2: //Set Signals
          pError("TODO");
          break;
        default: 
          sel = 0;
          pError("Selection does not exist");
          return -1;
          break;
      }
      break;
    default://Menu ID unknown
      pError("Menu does not exist");
      return -1;
      break;
  }
}

//Sends text to screen - Used for errors and some other popups
void pError(char *eText){
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0,12,128,"NOTICE:");
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0,28,128,eText);
  display.display();
  // Wait for controls to be used
  while(getInput() == -1){
    continue;
  }
  //Clear screen
  display.clear();
  display.display();
}

int8_t getInput(){
  int analogVal = analogRead(JOYSTICK_Y);
  if(analogVal >= DEADZONEHIGH){
    return DOWN;
  } else if(analogVal <= DEADZONELOW){
    return UP;
  }
  analogVal = analogRead(JOYSTICK_X);
  if(analogVal <= DEADZONELOW){
    return RIGHT;
  } else if(analogVal >= DEADZONEHIGH){
    return LEFT;
  }
  if(joyButton.pressed()){
    return SELECT;
  }
  return -1;
}
