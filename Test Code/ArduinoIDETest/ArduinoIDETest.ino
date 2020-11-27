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

// MenuItem Struc for objects
typedef struct{
  char *title; //Title text
  char *desc; //Description text
  int8_t pid; //For storing PIDs
} MenuItem;

// Menu class
class Menu{
  MenuItem *items;
  int8_t len;
  int8_t ID;
public:
  Menu(int8_t arrLen){
    len = arrLen;
    items = new MenuItem[arrLen];
  }
  ~Menu(){
    delete items;
  }
  int8_t setID(int8_t id){
    ID = id;
  }
  int8_t getLen(){
    return len;
  }
  int8_t getID(){
    return ID;
  }
  void setItem(int8_t pos, char *t, char *d, int8_t PID = 0){
    items[pos].title = t;
    items[pos].desc = d;
    items[pos].pid = PID;
  }
  char* getTitle(int pos){
    return items[pos].title;
  }
  char* getDesc(int pos){
    return items[pos].desc;
  }
  int8_t getPID(int pos){
    return items[pos].pid;
  }
};

Button joyButton(JOYSTICK_BUTTON,PULLUP);
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);

Menu main(4);
Menu *mPtr = &main;
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
  main.setItem(0,"Display","Display current selection.");
  main.setItem(1,"Gauges","Choose a Gauge.");
  main.setItem(2,"Animations","Choose an LED Animation.");
  main.setItem(3,"Settings","Display current settings.");
  pError("Press any button to continue");
}

void loop() {
  menuSelect(mPtr, sel);
  
  delay(250);
}
//Menu
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
      return executionTable(m,select);
      break;
  }
  ShowMenu(m, select);
  Serial.println(select);
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
int executionTable(Menu *m, int8_t sel){
  switch(m->getID()){ //Switch based on menu
    //Default menu
    case 0:
      switch(sel){//Switch based on selection
        case 0:
          //Generate infor about current state of the device
          Serial.println("KDA");
          break;
        case 1:
          {
            //Generate menu for gauges and run
            Menu gaugeMenu(1);
            //Items ---
            //return menuSelect(&gaugeMenu, sel);
            Serial.println("Gauges");
          }
          break;
        case 2:
          {
            //Generate menu for animations and run
            Menu animMenu(1);
            //Items ---
            //return menuSelect(&animMenu, sel);
            Serial.println("Animations");
          }
          break;
        case 3:
          {  //Generate menu for settings and run
            Menu selMenu(3);
            selMenu.setID(3);
            selMenu.setItem(0,"Bluetooth","Turn Bluetooth On/Off");
            selMenu.setItem(1,"Blackout","Auto turn off when driving");
            selMenu.setItem(2,"Signals","Use lights as signals");
            //return menuSelect(&selMenu, sel);
            Serial.println("Settings");
          }
          break;
        default:
          pError("Selection does not exist");
          return -1;
          break;
      }
      break;
    //User settings menu
    case 3:
      switch(sel){
        case 0: //Set up bluetooth
          break;
        case 1: //Set Blackout
          break;
        case 2: //Set Signals
          break;
        default: //Return back to menu
          pError("exit"); //REMOVE - for debug only
          return 0;
          break;
      }
      break;
    case 1: //PID selector
      //Just return PID value at sel
      return m->getPID(sel);
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
