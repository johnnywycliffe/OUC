// UnderglowController.ino
// Copyright (c) 2020 Jeremy Stintzcum, all rights reserved.
// License: MIT
// Description: A controller for underglow/LED strips. Designed to grab infor frm a vehicle
// through ODB-II port, display a preset or custom pattern, or act as additional turn signals 
// or brake lights.

//=============================================================================================
// TODO (rough order of priority):
// Non-submenu functionality / options
// - Add in menu for Spare 1 and Spare 2
// - Remove LED type menu items. 
// LEDHardware settings adjustment
// LED setters
// LED preset modes
// Brake/Turn signal modes
// Bluetooth
// App (Android)
// Chain between vehicles

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
 
//Libraries
#include <EEPROM.h>
#include <CAN.h>
#include <OBD2.h>
#include "SSD1306Spi.h"
#include <FastLED.h>
#include <Button.h>

//Defines
#define EEPROM_SIZE 8 //Adjust based on actual need (Get sizeof settings struct)
#define RETRY_SPEED 1000 //In milliseconds, higher is slower
#define DEADZONELOW 300
#define DEADZONEHIGH 3896
#define MAX_BRIGHTNESS 200
//Input
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SELECT 4
#define BACK 5
//Hardware pins - ESP32 defined
#define SPI_MOSI 23 
#define SPI_MISO 19
#define SPI_CLK 18
//Pins - Display
#define DISP_RESET 5
#define DISP_CS 4
#define DISP_DC 2
//Pins - input
#define JOYSTICK_X 32
#define JOYSTICK_Y 33
#define JOYSTICK_BUTTON 21
#define BUTTON_0 22
//Pins - LED outputs
#define LED_FRONT 26
#define LED_REAR 27
#define LED_LEFT 13
#define LED_RIGHT 25
#define LED_SPARE1 15
#define LED_SPARE2 14
//Pins - Input signals
#define LTURNSIGNAL 22
#define RTURNSIGNAL 21
#define BRAKE 16
#define CANH 36
#define CANL 39
//Pins - other
#define RELAY 12

//Globals/enums
//Menu
enum State{
  main, settings, gauge1, pattern1, pattern2, pattern3, bluetooth, ledsetting1, ledsetting2,
  brakeandturn, brake, turn, color, pickpattern, animation, ledorder
};
int sel = 0; //current selection
bool demo = false; //if true, lets user preview LEDS as they scroll through options
bool screenUpdated = false; //Updates screen if true
//LEDs
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
bool preset = false; //TODO: move to EERPOM saved settings

//Structs/Classes
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

// Settings struct for running program.
typedef struct{
  //Holds setting data to be written to EEPROM
  //ADMIN:
  char deviceID[10]; //Custom ID for ease of connection
  bool AllowSerial;
  //LED hardware
  
  //Light patterns
  LEDPatterns curr;
  LEDPatterns preset1;
  LEDPatterns preset2;
  LEDPatterns preset3;
  LEDPatterns preset4;
  LEDPatterns preset5;
  //User settings
  bool Bluetooth; //Bluetooth on or off
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
  MenuItem itemArr[100];
  int8_t len;
  char *menuTitle;
  State s;
  State prev;
public:
  Menu(){
    len = 0;
  }
  ~Menu(){
    delete itemArr;
  }
  //Set
  void setState(State state){
    s = state;
  }
  void resetLen(){
    len = 0;
  }
  void setItem(char *t, char *d, int8_t PID = 0){
    itemArr[len].title = t;
    itemArr[len].desc = d;
    itemArr[len].pid = PID;
    len++;
  }
  void setMenuTitle(char *mt){
    menuTitle = mt;
  }
  void setPrevMenu(State s){
    prev = s;
  }
  //Get
  int8_t getLen(){
    return len;
  }
  char* getMenuTitle(){
    return menuTitle;
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
  State getState(){
    return s;
  }
  State getPrevMenu(){
    return prev;
  }
};

//Initialization
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);
Settings settingStruct;
Button joyButton(JOYSTICK_BUTTON, PULLUP);
Button button1(BUTTON_0, PULLUP);
CRGB *underglow;

Menu mMenu;
State s;
LEDPatterns active;
SelectedString sLEDString;
LEDHardware frontLH;
LEDHardware rightLH;
LEDHardware rearLH;
LEDHardware leftLH;
LEDHardware spare1LH;
LEDHardware spare2LH;

void setup() {
  // Set up each system
  EEPROM.begin(EEPROM_SIZE);
  // Display
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  // Controls
  joyButton.begin();
  button1.begin();
  // LEDs
  setupLEDs();
  // OBD2
  /*while(!OBD2.begin()){
    //Ask user to check connection to vehicle, pause for a second
    delay(RETRY_SPEED);
  }*/
  //Menu
  setupMenu(main);
  // load settings
  loadSetting();
}

void loop() {
  // Bluetooth input
  // Menu - Change to only be active when needed
  int result = menuSelect(&mMenu, sel);
  //Return to main menu
  if(result==-1){
    sel = 0;
    if(s != main){
      setupMenu(mMenu.getPrevMenu());
    } 
  }
  //Behaviours for menu states
  switch(s){
    default:
      //Do nothing
      break;
  }
  //Refresh rate (Faster allows for more inputs, but thumbstick will scroll rapidly)
  delay(150);
}

//Set menu options and prepare for display
void setupMenu(State sel){
  mMenu.resetLen();
  mMenu.setState(sel);
  switch(sel){
    default: //If a menu hasn't been implemented or an error occurs, return to main
      pError("Not defined, returning to main");
    case main: //Default menu
      mMenu.setMenuTitle("Main Menu");
      mMenu.setPrevMenu(main);
      mMenu.setItem("Display","Display current selection");
      mMenu.setItem("Gauges","Choose a Gauge");
      mMenu.setItem("Animations","Choose an LED Animation");
      mMenu.setItem("Settings","Display current settings");
      break;
    case gauge1: //Top level gauge menu
      mMenu.setMenuTitle("");
      mMenu.setPrevMenu(main);
      mMenu.setItem("","");
      break;
    case pattern1: //Top level pattern menu
      mMenu.setMenuTitle("Pattern selection");
      mMenu.setPrevMenu(main);
      mMenu.setItem("Custom pattern","Choose colors, pattern and animations");
      mMenu.setItem("Preset patterns","A selection of pre-made patterns");
      break;
    case pattern2: //Custom patterns menu
      mMenu.setMenuTitle("Custom patterns");
      mMenu.setPrevMenu(pattern1);
      mMenu.setItem("Set Pattern","Choose specific pattern to display");
      mMenu.setItem("Set Colors","Choose colors for display");
      mMenu.setItem("Set Animation","Choose animation to display");
      break;
    case pickpattern: //Choose a pattern + offset
      mMenu.setMenuTitle("Pattern Selection");
      mMenu.setPrevMenu(pattern2);
      mMenu.setItem("Narrow","1 pixels wide");
      mMenu.setItem("Medium","2 pixels wide");
      mMenu.setItem("Large","3 pixels wide");
      mMenu.setItem("Wide","6 pixels wide");
      mMenu.setItem("Half-n-Half","2 colors");
      mMenu.setItem("Quarters","4 colors");
      mMenu.setItem("Dots","Includes trail");
      break;
    case color: //Pick a method of choosing color (Next level down, include fade or no fade options)
      mMenu.setMenuTitle("Color selection");
      //TODO: Set prev menus from calling menu
      mMenu.setItem("Standard colors","A list of common colors");
      mMenu.setItem("Pallete","Pick from a pre-made pallete");
      mMenu.setItem("RGB","Pick values by RGB values");
      mMenu.setItem("HSV","Pick values by HSV method");
      break;
    case animation: //Choose animation + Animation speed
      mMenu.setMenuTitle("Animation Selection");
      mMenu.setPrevMenu(pattern2);
      mMenu.setItem("Cycle CW","Cycles pattern clockwise");
      mMenu.setItem("Cycle CCW","Cycles pattern counterclockwise");
      mMenu.setItem("Breathe","LEDs fade in and out all at once");
      mMenu.setItem("Fade","LEDs alternate fading in and out");
      mMenu.setItem("Random Fade","LEDs fade randomly");
      mMenu.setItem("Cylon","Back and forth");
      mMenu.setItem("Color pop","Sparks of color");
      mMenu.setItem("Splatter","Like plowing through somethign");
      mMenu.setItem("Drip","Color dripping from under car");
      break;
    case pattern3: //Premade patterns menu
      mMenu.setMenuTitle("Premade patterns");
      mMenu.setPrevMenu(pattern1);
      mMenu.setItem("Pacifica","A calming water effect");
      mMenu.setItem("Rising Flames","A not-so-calming flame effect");
      mMenu.setItem("TwinkleFox","I got nothin'");
      mMenu.setItem("MURICA","FREEDOM MODE");
      mMenu.setItem("Valentines","Think fluffy thoughts");
      mMenu.setItem("Shamrock","Top o' the mornin'");
      mMenu.setItem("Halloween","Spooky");
      mMenu.setItem("Christmas","Commercialization!");
      break;
    case settings: //Main settings menu
      mMenu.setMenuTitle("Settings");
      mMenu.setPrevMenu(main);
      mMenu.setItem("LED Settings","Set up and tweak LEDs");
      mMenu.setItem("Brake and Turn","Brake and turn signal wiring");
      mMenu.setItem("Bluetooth","Set up bluetooth");
      mMenu.setItem("Driving shutoff","Automatically cut lights when car in motion");
      mMenu.setItem("Device Info","Licenses, credits, stuff like that");
      break;
    case ledsetting1: //Pick which LED string is being accessed.
      mMenu.setMenuTitle("Set up LED string.");
      mMenu.setPrevMenu(settings);
      mMenu.setItem("Front string","Direction, number, etc.");
      mMenu.setItem("Passenger string","Direction, number, etc.");
      mMenu.setItem("Rear string","Direction, number, etc.");
      mMenu.setItem("Driver string","Direction, number, etc.");
      mMenu.setItem("Spare string 1","Direction, number, etc.");
      mMenu.setItem("Spare string 2","Direction, number, etc.");
      break;
    case ledsetting2:
      //TODO: Change title based on LED string selected
      mMenu.setPrevMenu(ledsetting1);
      mMenu.setItem("Color order","Change RGB color order"); //Default for WS2811s is GRB
      mMenu.setItem("Number","Change number of LEDs on the strip"); //Remember to put notice for WS2811s
      mMenu.setItem("Direction","Change direction of led flow"); //Reverse strip if installed backwards
      break;
    case ledorder: //Top level gauge menu
      mMenu.setMenuTitle("Select color Order");
      mMenu.setPrevMenu(ledsetting2);
      mMenu.setItem("RGB","Should display red, green, blue");
      mMenu.setItem("RBG","Should display red, green, blue");
      mMenu.setItem("GRB","Should display red, green, blue");
      mMenu.setItem("GBR","Should display red, green, blue");
      mMenu.setItem("BRG","Should display red, green, blue");
      mMenu.setItem("BGR","Should display red, green, blue");
      break;
    case brakeandturn: //Set brake and turn signal behavior
      mMenu.setMenuTitle("Brakes and signals");
      mMenu.setPrevMenu(settings);
      mMenu.setItem("Brakes","Set brake behavior");
      mMenu.setItem("Turn signals","Set turn signal behavior");
      break;
    case bluetooth: //Bluetooth settings
      mMenu.setMenuTitle("Bluetooth settings");
      mMenu.setPrevMenu(settings);
      mMenu.setItem("BT Enabled","Toggle bluttooth on or off.");
      mMenu.setItem("BT Name","Name for identifying device.");
      mMenu.setItem("BT Pair","Pair a new bluetooth device.");
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

//Defines menu behaviour
int executionTable(State s, int &sel){
  switch(s){ //Switch based on menu
    case main: //Main menu
      switch(sel){
        case 0: //Display
          //Show present configuration
          pError("TODO: CURR CONFIG"); //Scrolling text
          break;
        case 1: //Gauge menu selected
          sel = 0;
          setupMenu(s=gauge1);
          break;
        case 2: //Pattern menu selected
          sel = 0;
          setupMenu(s=pattern1);
          break;
        case 3: //Settings menu selected
          sel = 0;
          setupMenu(s=settings);
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case gauge1:
      switch(sel){
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case pattern1:
      switch(sel){
        case 0: //Show custom pattern menu
          sel = 0;
          setupMenu(s=pattern2);
          break;
        case 1: //Show premade pattern menu
          sel = 0;
          setupMenu(s=pattern3);
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case pattern2:
      switch(sel){
        case 0: //Show pattern menu
          sel = 0;
          setupMenu(s=pickpattern);
          break;
        case 1: //Show Color picker menu
          sel = 0;
          setupMenu(s=color);
          break;
        case 2: //Show Animation menu
          sel = 0;
          setupMenu(s=animation);
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case pickpattern:
      switch(sel){
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case color:
      switch(sel){
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case animation:
      switch(sel){
        case 4: //Color Pop
          pError("TODO: COLORPOP"); //Pattern selection
          pError("CHOOSE SPEED"); //Pick speed w/ default
          break;
        case 5: //Splatter
          pError("TODO: SPLATTER"); //Pattern selection
          pError("CHOOSE SPEED"); //Pick speed w/ default
          break;
        case 6: //Drip
          pError("TODO: DRIP"); //Pattern selection
          pError("CHOOSE SPEED"); //Pick speed w/ default
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case pattern3:
      switch(sel){
        case 0: //Pacifica
          pError("TODO: PACIFICA"); //Pattern selection
          break;
        case 1: //Rising FLames
          pError("TODO: RISING FLAMES"); //Pattern selection
          break;
        case 2: //TwinkleFox
          pError("TODO: TWINKLEFOX"); //Pattern selection
          break;
        case 3: //MURICA
          pError("TODO: MURICA"); //Pattern selection
          break;
        case 4: //Valentine's
          pError("TODO: VAENTINES"); //Pattern selection
          break;
        case 5: //Shamrock
          pError("TODO: SHAMROCK"); //Pattern selection
          break;
        case 6: //Halloween
          pError("TODO: HALLOWEEN"); //Pattern selection
          break;
        case 7: //Christmas
          pError("TODO: CHRISTMAS"); //Pattern selection
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case settings:
      switch(sel){
        case 0: //Set up LED Settings
          sel = 0;
          setupMenu(s=ledsetting1);
          break;
        case 1: //Set up Brakes and turn signals
          sel = 0;
          setupMenu(s=brakeandturn);
          break;
        case 2: //Set up bluetooth
          sel = 0;
          setupMenu(s=bluetooth);
          break;
        case 3: //Set up auto shutoff when driving
          pError("TODO: AUTOSHUTOFF"); //Bool (Add warning for ODB-II?)
          break;
        case 4: //Display device info
          pError("TODO: DEVICE INFO"); //Scrolling text
          break;
        default: 
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case ledsetting1:
      switch(sel){
        case 0: //Front LED String
          sel = 0;
          setupMenu(s=ledsetting2);
          mMenu.setMenuTitle("Front String");
          sLEDString = front;
          break;
        case 1: //Passenger LED String
          sel = 0;
          setupMenu(s=ledsetting2);
          mMenu.setMenuTitle("Passenger String");
          sLEDString = passenger;
          break;
        case 2: //Rear LED String
          sel = 0;
          setupMenu(s=ledsetting2);
          mMenu.setMenuTitle("Rear String");
          sLEDString = rear;
          break;
        case 3: //Driver LED String
          sel = 0;
          setupMenu(s=ledsetting2);
          mMenu.setMenuTitle("Driver String");
          sLEDString = driver;
          break;
        case 4: //Spare LED String 1
          sel = 0;
          setupMenu(s=ledsetting2);
          mMenu.setMenuTitle("Spare String 1");
          sLEDString = spare1;
          break;
        case 5: //Spare LED String 2
          sel = 0;
          setupMenu(s=ledsetting2);
          mMenu.setMenuTitle("Spare String 2");
          sLEDString = spare2;
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case ledsetting2:
      switch(sel){
        case 0: //RGB order
          pError("TODO: RGB ORDER MENU"); //Sub menu
          break;
        case 1: // Number of LEDs
          pError("TODO: STRING SIZE SELECTION"); //Int
          break;
        case 2: //Direction
          pError("TODO: REVERSE DIRECTION"); //Bool
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case ledorder:
      //Set LEDS strip type after user verifies
      //use sLEDSetring to save to correct string
      switch(sLEDString){
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case brakeandturn:
      switch(sel){
        case 0: //Set brakes
          pError("TODO: BRAKE MENU"); //Sub menu
          break;
        case 1: //Turn signal behaviours
          pError("TODO: TURN SIGNAL MENU"); //Sub menu
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case bluetooth:
      switch(sel){
        case 0: //BT enabled
          pError("TODO: BLUETOOTH ENABLE/DISABLE"); //bool
          break;
        case 1: //BT name
          pError("TODO: RENAME BT DEVICE"); //Text entry
          break;
        case 2: //BT pair
          pError("TODO: BT PAIRING"); //Pair device function
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    default://Menu ID unknown
      pError("Error: Menu does not exist");
      return -1;
      break;
  }
}

//Set PID to control - FIXME
void PIDMode(int PID){
  if (!OBD2.pidSupported(PID)){
    pError("Unsupported");
  } else {
    //s.curr.trackedPID = PID;
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

// Select a PID via menu - FIXME (New menu system)
int selectPID(){
  int tempArray[96] = {0};
  int8_t len = getValidPIDs(tempArray);
  /*Menu pidMenu(len);
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
  }*/
}

//Sends text to screen - Used for errors and some other popups - COMPLETE
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
  //Print to Serial
  Serial.println(eText);
  //Clear screen
  display.clear();
  display.display();
  
}

//EEPROM - get setting value - COMPLETE
char loadSetting(){
  EEPROM.get(0,settingStruct);
}

//EEPROM - save value - COMPLETE
void saveSetting(){
  EEPROM.put(0,settingStruct);
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
    temp.deviceID[i] = settingStruct.deviceID[i];
  }
  //Save settings
  settingStruct = temp;
  saveSetting;
}

//Take input from joystick and button(s). Only accepts one input per cycle. Not building an NES here.
int8_t getInput(){
  int analogVal = analogRead(JOYSTICK_Y);
  if(analogVal >= DEADZONEHIGH){
    return DOWN;
  } else if(analogVal <= DEADZONELOW){
    return UP;
  }
  analogVal = analogRead(JOYSTICK_X);
  if(analogVal >= DEADZONEHIGH){
    return LEFT;
  } else if(analogVal <= DEADZONELOW){
    return RIGHT;
  }
  if(button1.pressed()){
    return SELECT;
  }
  if(joyButton.pressed()){
    return BACK;
  }
}

//LED Initialization - Only run on init
//On reboot, initialize LEDS
void setupLEDs(){
  int LEDTotal = 0;
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

//Strip flipper. Flips strips.
void colorSorter(CRGB color, int led){
  if(led < frontLH.ledCount && frontLH.reversed){
    underglow[((frontLH.startPos+frontLH.ledCount-1)-(led-frontLH.startPos))] = color;
  } else if (led < rightLH.ledCount + rightLH.startPos && rightLH.reversed){
    underglow[((rightLH.startPos+rightLH.ledCount-1)-(led-rightLH.startPos))] = color;
  } else if (led < rearLH.ledCount + rearLH.startPos && rearLH.reversed){
    underglow[((rearLH.startPos+rearLH.ledCount-1)-(led-rearLH.startPos))] = color;
  } else if (led < leftLH.ledCount + leftLH.startPos && leftLH.reversed){
    underglow[((leftLH.startPos+leftLH.ledCount-1)-(led-leftLH.startPos))] = color;
  } else { //Not flipped, one of the spare strips.
    underglow[led] = color;
  } 
}
