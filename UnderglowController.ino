// UnderglowController.ino
// Copyright (c) 2020 Jeremy Stintzcum, all rights reserved.
// License: MIT
// Description: A controller for underglow/LED strips, utilizing the OBD-II port of the car to 
// sync the lights to the car in one of several modes. Works on all cars made after 2008 to 
// time of writing.

//=============================================================================================
// TODO (rough order of priority):
// preset class
// LED mode presets
// Fix menu to make non-blocking
// Get OLED screen printing text
// Check orientation of Joystick X and Y
// Add all color modes
// Relay control
// More options for brakes/turn signals
// 12v logic shifter for brakes and turn signals
// Add bluetooth
// App?
// Extra LED output lines
// Chain (LOL)

//Libraries
#include <EEPROM.h>
#include <CAN.h>
#include <OBD2.h>
#include "SSD1306Spi.h"
#include <FastLED.h>
#include <Button.h>

//Defines
#define EEPROM_SIZE 8 //Adjust based on actual need
#define RETRY_SPEED 1000 //In milliseconds, higher is slower
#define DEADZONELOW 1024
#define DEADZONEHIGH 3072
//Hardware pins
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_CLK 18
#define DISP_RESET 5
#define DISP_CS 4
#define DISP_DC 2
#define JOYSTICK_X 36
#define JOYSTICK_Y 39
#define JOYSTICK_BUTTON 34
#define BUTTON_0 35
#define LED_FRONT 33
#define LED_REAR 32
#define LED_LEFT 27
#define LED_RIGHT 26
#define LED_SPARE1 15
#define LED_SPARE2 14
#define LTURNSIGNAL 22
#define RTURNSIGNAL 21
#define BRAKE 16
#define RELAY 13
#define CANH 25
#define CANL 17

//Input
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SELECT 4
#define BACK 5
//LEDS
//It's assumed SIDE LEDs start near front of vehicle, and 
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define SIDELEDCOUNT 30
#define FRONTLEDCOUNT 18 //Also for rear

//Globals

//Structs/Classes
// Settings struct for running program.
typedef struct{
  //Holds setting data to be written to EEPROM
  //ADMIN:
  char deviceID[10]; //Custom ID for ease of connection
  bool AllowSerial;
  //LED functions - move to preset class
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

//Initialization
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);
Settings s;
Button joyButton(JOYSTICK_BUTTON);
Button button1(BUTTON_0);
CRGB frontLEDs[FRONTLEDCOUNT];
CRGB rearLEDs[FRONTLEDCOUNT];
CRGB leftLEDs[SIDELEDCOUNT];
CRGB rightLEDs[SIDELEDCOUNT];
Menu main(4);
Menu *m = &main;

void setup() {
  // Set up each system
  EEPROM.begin(EEPROM_SIZE);
  // Display
  display.init();
  // Controls
  joyButton.begin();
  button1.begin();
  // LEDs
  FastLED.addLeds<LED_TYPE, LED_FRONT, COLOR_ORDER>(frontLEDs, FRONTLEDCOUNT);
  FastLED.addLeds<LED_TYPE, LED_REAR, COLOR_ORDER>(rearLEDs, FRONTLEDCOUNT);
  FastLED.addLeds<LED_TYPE, LED_LEFT, COLOR_ORDER>(leftLEDs, SIDELEDCOUNT);
  FastLED.addLeds<LED_TYPE, LED_RIGHT, COLOR_ORDER>(rightLEDs, SIDELEDCOUNT);
  // OBD2
  while(!OBD2.begin()){
    //Ask user to check connection to vehicle, pause for a second
    delay(RETRY_SPEED);
  }
  // Main menu
  main.setItem(0,"Display","Display current selection.");
  main.setItem(1,"Gauges","Choose a Gauge.");
  main.setItem(2,"Animations","Choose an LED Animation.");
  main.setItem(3,"Settings","Display current settings.");

  // load settings
  loadSetting();
}

void loop() {


}

//Menu
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
    case 1: //PID selector
      //Just return PID value at sel
      return m->getPID(sel);
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

// Find valid PIDs for connected vehicle - COMPLETE
int8_t getValidPIDs(int *PIDArr){
  //Use count as len for menu
  int8_t count = 0;
  for(int i = 0; i < 96; i++){
    if (OBD2.pidSupported(i)) {
      PIDArr[i] = i;
      count++;
    }
  }
  return count;
}


// Select a PID via menu - COMPLETE
int selectPID(){
  int tempArray[96] = {0};
  int8_t len = getValidPIDs(tempArray);
  Menu pidMenu(len);
  pidMenu.setID(1);
  int8_t count = 0;
  for(int8_t i = 0; i < 96; i++){
    if(tempArray[i] == 0){
      continue;
    } else {
      char cBuffer1[32];
      char cBuffer2[32];
      OBD2.pidName(i).toCharArray(cBuffer1,32);
      OBD2.pidUnits(i).toCharArray(cBuffer2,32);
      pidMenu.setItem(count, cBuffer1, cBuffer2); //Hacky work-around, remove
      count++;
    }
    int result = menuSelect(&pidMenu);
    PIDMode(result);
    return result;
  }
}

// LED Color selection

// LED Speed selection

// LED Brightness selection

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

//EEPROM - Reset - COMPLETE
void EEPROMReset(){
  //Create new, zeroed struct
  Settings temp = {0};
  //Copy important information
  for(int8_t i = 0; i < 10; i++){
    temp.deviceID[i] = s.deviceID[i];
  }
  //Save settings
  s = temp;
  saveSetting;
}

//Take input from joystick and button(s). Only accepts one input per cycle. Not building an NES here.
int8_t getInput(){
  int analogVal = analogRead(JOYSTICK_X);
  if(analogVal >= DEADZONEHIGH){
    return RIGHT;
  } else if(analogVal <= DEADZONELOW){
    return LEFT;
  }
  analogVal = analogRead(JOYSTICK_Y);
  if(analogVal >= DEADZONEHIGH){
    return DOWN;
  } else if(analogVal <= DEADZONELOW){
    return UP;
  }
  if(button1.pressed()){
    return SELECT;
  }
  if(joyButton.pressed()){
    return BACK;
  }
}
