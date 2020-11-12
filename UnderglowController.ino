// UnderglowController.ino
// Copyright (c) 2020 Jeremy Stintzcum, all rights reserved.
// License:
// Description: A controller for underglow, utilizing the OBD-II port of the car to sync the
// lights to the car in one of several modes. Works on all cars made after 2008 to time of writing.

//=============================================================================================
// Information needed
// LED strip type (WS2813?)
// Number of LEDs in strip (total)
// Number of LEDs in big strip and small strip
// Order of control for all four strips
// What PIDs are most wanted
// What are the states for the bluetooth controller as it is? Can I mimic?

//=============================================================================================
// TODO:
// Get OLED screen printing text
// Import some menu library
// Set up controls
// Allow for serial interface
// Add bluetooth
// App?
// Add all color modes (lol)


//Libraries
#include <EEPROM.h>
#include <CAN.h>
#include <OBD2.h>
#include "SSD1306Spi.h"
//#include <FastLED.h>

//Defines
#define EEPROM_SIZE 8 //Adjust based on actual need
#define RETRY_SPEED 1000 //In milliseconds, higher is slower
//Hardware pins
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_CLK 18
#define DISP_RESET 5
#define DISP_CS 4
#define DISP_DC 2
#define JOYSTICK_X
#define JOYSTICK_Y
#define JOYSTICK_BUTTON
#define BUTTON_0
//Input
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SELECT 4
#define BACK 5

//Globals

//Structs/Classes
// Settings struct for running program.
typedef struct{
  //Holds setting data to be written to EEPROM
  //ADMIN:
  char deviceID[10]; //Custom ID for ease of connection
  bool AllowSerial;
  //LED functions
  uint8_t trackedPID; //CANBUD PID currently being monitored.
  uint8_t currLEDAnim; //Saves previously used animation mode for future
  uint8_t LEDsettings1[3]; //Saves RGB values for primary color
  uint8_t LEDsettings2[3]; //Saves RGB values for secondary color
  uint8_t LEDsettings3[3]; //Saves RGB values
  uint8_t LEDSpeed; //LED animation speed
  uint8_t LEDBrightness; //LED brightness
  //User settings
  bool Bluetooth; //Bluttoth on or off
  bool rollingShutoff; //If device should cut power when vehicle in motion
  bool blinkersAndBrakes; //Mimics blinkers and brakes when driving
} Settings;

// MenuItem Struc for objects
typedef struct{
  char *title; //Title text
  char *desc; //Description text
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
  void setItem(int8_t pos, char *t, char *d){
    items[pos].title = t;
    items[pos].desc = d;
  }
  char* getTitle(int pos){
    return items[pos].title;
  }
  char* getDesc(int pos){
    return items[pos].desc;
  }
};

//Initialization
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);
Settings s;
Menu main(4);
Menu *m = &main;

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  display.init();
  while(!OBD2.begin()){
    //Ask user to check connection to vehicle, pause for a second
    delay(RETRY_SPEED);
  }
  main.setItem(0,"Display","Display current selection.");
  main.setItem(1,"Gauges","Choose a Gauge.");
  main.setItem(2,"Animations","Choose an LED Animation.");
  main.setItem(3,"Settings","Display current settings.");
}

void loop() {


}

/*Menu - COMPLETE?
// Structured like so:
// Mode
// - Car driven/gauge
// - - Choose PID
// - - - Gauge 1: One color, leds go from back to forward
// - - - Gauge 2: One color, LEDs go from forward to back
// - - - Gauge 3: Three colors, green bottom end, yellow middle and red for high
// - - - etc...
// - LED patterns
// - - Solid color
// - - Chasing colors
// - - Rainbow
// - - etc..
// Settings
// - Bluetooth
// - - Bluetooth password
// - - Pair new device
// - Automatic driving shutoff (cuts lights when driving for legal reasons)
*/
int menuSelect(Menu *m){
  //Initialize
  int select = 0;
  bool complete = false;
  //Get input
  while(!complete){
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
        complete = true;
        return -1;
        break;
      case RIGHT:
      case SELECT:
        //Send state to execution table for further input
        return executionTable(m,select);
        complete = true;
        break;
    }
  }
}

//Defines menu behaviour -not complete... at all
int executionTable(Menu *m, int8_t sel){
  switch(m->getID()){ //Switch based on menu
    //Default menu
    case 0:
      switch(sel){//Switch based on selection
        case 0:
          //Generate infor about current state of the device
          return 0;
          break;
        case 1:
        {
          //Generate menu for gauges and run
          Menu gaugeMenu(1);
          //Items ---
          return menuSelect(&gaugeMenu);
        }
          break;
        case 2:
        {
          //Generate menu for animations and run
          Menu animMenu(1);
          //Items ---
          return menuSelect(&animMenu);
        }
          break;
        case 3:
        {  //Generate menu for settings and run
          Menu selMenu(3);
          selMenu.setID(3);
          selMenu.setItem(0,"Bluetooth","Turn Bluetooth On/Off");
          selMenu.setItem(1,"Blackout","Auto turn off when driving");
          selMenu.setItem(2,"Signals","Use lights as signals");
          return menuSelect(&selMenu);
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
    default://Menu ID unknown
      pError("Menu does not exist");
      return -1;
      break;
  }
}

//Set PID to control - COMPLETE
void PIDMode(int PID){
  if (!OBD2.pidSupported(PID)){
    pError("Unsupported");
  } else {
    s.trackedPID = PID;
    saveSetting();
  }
}

//Returns value of sensor PID is monotoring from vehicle - COMPLETE
float fetchCarData(int pid){
  if (!OBD2.pidSupported(pid)) pError("Unsupported");
  float pidValue = OBD2.pidRead(pid);
  if(isnan(pidValue)){
    pError("Data not valid");
    return -1.0;
  }
  return pidValue;
}

//Sends text to screen - Used for errors and some other popups
void pError(char *eText){
  int8_t textLen = sizeof(eText);
  //TODO: Screen update routines
}

//EEPROM - get setting value - COMPELTE
char loadSetting(){
  EEPROM.get(0,s);
}

//EEPROM - save value - COMPLETE
void saveSetting(){
  EEPROM.put(0,s);
  EEPROM.commit();
  //Notify user settings saved
  pError("Settings Saved");
}

//Take input from joystick and button(s)
int8_t getInput(){
  //TODO: Create bit mask thing for inputs, debouncer
}
