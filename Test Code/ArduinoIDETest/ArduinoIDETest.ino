#include <Button.h>
#include "SSD1306Spi.h"
#include "bitmaps.h"

#define SPI_MOSI 23 
#define SPI_MISO 19
#define SPI_CLK 18
#define DISP_RESET 5
#define DISP_CS 4
#define DISP_DC 2
#define JOYSTICK_X 32
#define JOYSTICK_Y 33
#define JOYSTICK_BUTTON 22

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SELECT 4
#define BACK 5

#define LEDSTRIPCAP 100

#define DEADZONELOW 300
#define DEADZONEHIGH 3800

//Menu state
enum State{
  main, settings, gauge1, pattern1, pattern2, pattern3, bluetooth, ledsetting1, ledsetting2,
  brakeandturn, brake, turn, color, pickpattern, animation, ledorder, ledcount, ledflip,
  autoshutoff, screenbright
};
//Color order for LED strips
enum ColorOrder {rgb, rbg, grb, gbr, brg, bgr};
//Currently enabled pattern
enum SelectedPattern {
  narrow, medium, large, wide, halfandhalf, quarters, dots, pacifica, risingflames, twinklefox,
  murica, colorpop, splatter, drip, christmas, valentines, shamrock, halloween, turnsignaloff, 
  solid, chase, solidnrear, chasenrear, solidnboth, chasenboth, brakesignaloff, onnodim, onwithdim
};

//Device settings
typedef struct {
  //User settings
  bool autoshutoff; //If device automatically shuts off when driving
  uint8_t brightness; //Screen brightness
  SelectedPattern turn; //Turn signal behaviour
  SelectedPattern brake; //Brake light behaviour
} Settings;

//Animation controller
typedef struct {
  uint8_t offsetPos; //How offset from default position pattern is.
  //CRGBPalette16 RGBP; //RGB color pallette
  uint8_t brightness; //Brightness level of LEDs
  uint8_t animSpd; //Animation speed
  uint8_t trackedPID; // OBD-II PID Datsa to be read (if relevant)
  //SelectedPattern sp; //Current pattern used
} LEDPatterns;

// Hardware definition struct
typedef struct {
  bool reversed = false; //True for backwards, false for forwards
  uint8_t ledCount; //num of LEDs on strip.
  ColorOrder order; //BRG for test code
} LEDHardware;

// MenuItem Struc for objects
typedef struct{
  char *title; //Title text
  char *desc; //Description text
  int8_t pid; //For storing PIDs
} MenuItem;

// Menu class
class Menu{
  MenuItem itemArr[100]; //Array of menu items
  int len;            //Actual length
  char *menuTitle;       //Title of the menu
  State s;               //Current state of the menu
  State prev;            //Menu to return to
  int currVal;           //Value to print as current
  //Add PID value
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
  void setLen(int l){ //used for int as maxVal, bool as true(1) or false(0), and progBar as maxVal
    len = l;
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
  void setCurrent(int cv){
    currVal = cv;
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
  int getCurrent(){
    return currVal;
  }
};

Button joyButton(JOYSTICK_BUTTON,PULLUP);
SSD1306Spi display(DISP_RESET, DISP_DC, DISP_CS);

//Menu init
Menu mMenu;

//LED init
LEDHardware frontLH;
LEDHardware rightLH;
LEDHardware rearLH;
LEDHardware leftLH;
LEDHardware spare1LH;
LEDHardware spare2LH;
LEDHardware *sLEDString; //Pointer for settings

Settings deviceSettings;

int sel = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Start new test");
  joyButton.begin();
  //Display
  if(!display.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.flipScreenVertically();
  deviceSettings.brightness = 255;
  display.setBrightness(deviceSettings.brightness);
  mMenu.setState(main);
  setupMenu(mMenu.getState());
  pError("Press any button to continue");
}

void loop() {
  
  int result = menuSelect(&mMenu, sel);
  if(result==-1){
    sel = 0;
    if(mMenu.getState() != main){
      setupMenu(mMenu.getPrevMenu());
    }
  }
  menuDisplayMode();
  delay(250);
}

void menuDisplayMode(){
  switch(mMenu.getState()){
    default: //Regular menu
      ShowMenu(&mMenu, sel);
      break;
    case ledcount: //Integer
      showIntMenu(&mMenu, sel);
      break;
    case ledflip: //Boolean
    case autoshutoff:
      showBoolMenu(&mMenu, sel);
      break;
    case screenbright:
      display.setBrightness(sel*16);
      Serial.println("pulse");
      showIntMenu(&mMenu, sel);
      break;
  }
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
      ShowMenu(&mMenu, sel);
      break;
    case gauge1: //Top level gauge menu
      mMenu.setMenuTitle("t");
      mMenu.setPrevMenu(main);
      mMenu.setItem("t","t");
      ShowMenu(&mMenu, sel);
      break;
    case pattern1: //Top level pattern menu
      mMenu.setMenuTitle("Pattern selection");
      mMenu.setPrevMenu(main);
      mMenu.setItem("Custom pattern","Choose colors, pattern and animations");
      mMenu.setItem("Preset patterns","A selection of pre-made patterns");
      ShowMenu(&mMenu, sel);
      break;
    case pattern2: //Custom patterns menu
      mMenu.setMenuTitle("Custom patterns");
      mMenu.setPrevMenu(pattern1);
      mMenu.setItem("Set Pattern","Choose specific pattern to display");
      mMenu.setItem("Set Colors","Choose colors for display");
      mMenu.setItem("Set Animation","Choose animation to display");
      mMenu.setItem("Set Speed","Choose animation speed");
      ShowMenu(&mMenu, sel);
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
      ShowMenu(&mMenu, sel);
      break;
    case color: //Pick a method of choosing color (Next level down, include fade or no fade options)
      mMenu.setMenuTitle("Color selection");
      //TODO: Set prev menus from calling menu
      mMenu.setItem("Standard colors","A list of common colors");
      mMenu.setItem("Pallete","Pick from a pre-made pallete");
      mMenu.setItem("RGB","Pick values by RGB values");
      mMenu.setItem("HSV","Pick values by HSV method");
      mMenu.setItem("Current","Adjust currently displayed colors");
      ShowMenu(&mMenu, sel);
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
      ShowMenu(&mMenu, sel);
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
      ShowMenu(&mMenu, sel);
      break;
    case settings: //Main settings menu
      mMenu.setMenuTitle("Settings");
      mMenu.setPrevMenu(main);
      mMenu.setItem("LED Settings","Set up and tweak LEDs");
      mMenu.setItem("Brake and Turn","Brake and turn signal wiring");
      mMenu.setItem("Bluetooth","Set up bluetooth");
      mMenu.setItem("Driving shutoff","Automatically cut lights when car in motion");
      mMenu.setItem("Screen Brightness","Adjust brightness of screen");
      mMenu.setItem("Device Info","Licenses, credits, stuff like that");
      ShowMenu(&mMenu, sel);
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
      ShowMenu(&mMenu, sel);
      break;
    case ledsetting2:
      mMenu.setPrevMenu(ledsetting1);
      mMenu.setItem("Color order","Change RGB color order"); //Default for WS2811s is GRB
      mMenu.setItem("Number","Change number of LEDs on the strip"); //Remember to put notice for WS2811s
      mMenu.setItem("Direction","Change direction of led flow"); //Reverse strip if installed backwards
      ShowMenu(&mMenu, sel);
      break;
    case ledorder: //Top level gauge menu
      mMenu.setMenuTitle("Select Color Order");
      mMenu.setPrevMenu(ledsetting2);
      mMenu.setItem("RGB","Should display red, green, blue");
      mMenu.setItem("RBG","Should display red, green, blue");
      mMenu.setItem("GRB","Should display red, green, blue");
      mMenu.setItem("GBR","Should display red, green, blue");
      mMenu.setItem("BRG","Should display red, green, blue");
      mMenu.setItem("BGR","Should display red, green, blue");
      ShowMenu(&mMenu, sel);
      break;
    case ledcount:
      mMenu.setPrevMenu(ledsetting2);
      mMenu.setMenuTitle("Select Number of LEDs");
      mMenu.setLen(LEDSTRIPCAP);
      mMenu.setCurrent(sLEDString->ledCount);
      break;
    case ledflip:
      mMenu.setPrevMenu(ledsetting2);
      mMenu.setMenuTitle("Reverse direction of LEDs");
      mMenu.setLen(2); //Bool
      if(sLEDString->reversed == true){
        mMenu.setCurrent(1);
      } else {
        mMenu.setCurrent(0);
      }
      break;
    case brakeandturn: //Set brake and turn signal behavior
      mMenu.setMenuTitle("Brakes and signals");
      mMenu.setPrevMenu(settings);
      mMenu.setItem("Brakes","Set brake behavior");
      mMenu.setItem("Turn signals","Set turn signal behavior");
      ShowMenu(&mMenu, sel);
      break;
    case brake:
      mMenu.setMenuTitle("Brake Behaviour");
      mMenu.setPrevMenu(brakeandturn);
      mMenu.setItem("Off","Turn of brake light behavior");
      mMenu.setItem("On Brake","Lights up under brakes, off otherwise");
      mMenu.setItem("Dim + brake","Lights are dim until brake is pressed");
      break;
    case turn:
      mMenu.setMenuTitle("Turn Signal Behaviour");
      mMenu.setPrevMenu(brakeandturn);
      mMenu.setItem("Off","Turn of turn signal behavior");
      mMenu.setItem("Solid/Sides","Solid yellow, only on sides");
      mMenu.setItem("Solid/Extended","Solid yellow, with front and rear flashing as well");
      mMenu.setItem("Solid/Red","Solid yellow, rear flashes red");
      mMenu.setItem("Chase/Sides","Chasing yellow, only on sides");
      mMenu.setItem("Chase/Extended","Chasing yellow, with front and rear flashing as well");
      mMenu.setItem("Chase/Red","Chasing yellow, rear flashes red");
      break;
    case bluetooth: //Bluetooth settings
      mMenu.setMenuTitle("Bluetooth settings");
      mMenu.setPrevMenu(settings);
      mMenu.setItem("BT Enabled","Toggle bluttooth on or off."); 
      mMenu.setItem("BT Name","Name for identifying device.");
      mMenu.setItem("BT Pair","Pair a new bluetooth device.");
      ShowMenu(&mMenu, sel);
      break;
    case autoshutoff:
      mMenu.setPrevMenu(settings);
      mMenu.setMenuTitle("Auto shut off underglow");
      mMenu.setLen(2); //Bool
      if(deviceSettings.autoshutoff == true){
        mMenu.setCurrent(1);
      } else {
        mMenu.setCurrent(0);
      }
      break;
    case screenbright:
      mMenu.setPrevMenu(settings);
      mMenu.setMenuTitle("Adjust screen brightness");
      mMenu.setLen(16);
      mMenu.setCurrent((deviceSettings.brightness / 16)+1);
      break;
  }
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
  //Serial.println(select);
  return 0;
}

//Displays menu item
void ShowMenu(Menu *m, int select){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(63,0,128,m->getMenuTitle());
  display.drawLine(0,10,128,10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0,10,128,m->getTitle(select));
  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0,26,128,m->getDesc(select));
  display.display();
}

void showBoolMenu(Menu *m, int sel){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(63,0,128,m->getMenuTitle());
  display.drawLine(0,10,128,10);
  String text = "false";
  if(m->getCurrent() == 1){
    text = "true";
  }
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(63,14,128,"Current: " + text);
  text = "false";
  if(sel == 1){
    text = "true";
  }
  display.drawStringMaxWidth(63,36,128,"New: " + text);
  display.display();
}

void showIntMenu(Menu *m, int sel){
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawStringMaxWidth(63,0,128,m->getMenuTitle());
  display.drawLine(0,10,128,10);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(63,10,128,"Current: "+String(m->getCurrent()));
  display.drawStringMaxWidth(63,36,128,"New: "+String(sel + 1));
  if(sel != 0){
    display.drawFastImage(60,30,8,8,upArrowBitmap);
  } 
  if (sel != mMenu.getLen()-1) {
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
  while(getInput() == -1){
    continue;
  }
  //Clear screen
  display.clear();
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
          setupMenu(gauge1);
          break;
        case 2: //Pattern menu selected
          sel = 0;
          setupMenu(pattern1);
          break;
        case 3: //Settings menu selected
          sel = 0;
          setupMenu(settings);
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
          setupMenu(pattern2);
          break;
        case 1: //Show premade pattern menu
          sel = 0;
          setupMenu(pattern3);
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
          setupMenu(pickpattern);
          break;
        case 1: //Show Color picker menu
          sel = 0;
          setupMenu(color);
          break;
        case 2: //Show Animation menu
          sel = 0;
          setupMenu(animation);
          break;
        case 3: //Show Animation speed selection
          sel = 0;
          setupMenu(animation);
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
          pError("TODO: VALENTINES"); //Pattern selection
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
          setupMenu(ledsetting1);
          break;
        case 1: //Set up Brakes and turn signals
          sel = 0;
          setupMenu(brakeandturn);
          break;
        case 2: //Set up bluetooth
          sel = 0;
          setupMenu(bluetooth);
          break;
        case 3: //Set up auto shutoff when driving
          pError("Ensure OBD-II is plugged in and active");
          setupMenu(autoshutoff);
          break;
        case 4: //Adjust screen brightness
          sel = deviceSettings.brightness / 16;
          setupMenu(screenbright);
          break;
        case 5: //Display device info
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
          setupMenu(ledsetting2);
          mMenu.setMenuTitle("Front String");
          sLEDString = &frontLH;
          break;
        case 1: //Passenger LED String
          sel = 0;
          setupMenu(ledsetting2);
          mMenu.setMenuTitle("Passenger String");
          sLEDString = &rightLH;
          break;
        case 2: //Rear LED String
          sel = 0;
          setupMenu(ledsetting2);
          mMenu.setMenuTitle("Rear String");
          sLEDString = &rearLH;
          break;
        case 3: //Driver LED String
          sel = 0;
          setupMenu(ledsetting2);
          mMenu.setMenuTitle("Driver String");
          sLEDString = &leftLH;
          break;
        case 4: //Spare LED String 1
          sel = 0;
          setupMenu(ledsetting2);
          mMenu.setMenuTitle("Spare String 1");
          sLEDString = &spare1LH;
          break;
        case 5: //Spare LED String 2
          sel = 0;
          setupMenu(ledsetting2);
          mMenu.setMenuTitle("Spare String 2");
          sLEDString = &spare2LH;
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
          pError("Must reboot for changes to take effect");
          sel = 0;
          setupMenu(ledorder);
          break;
        case 1: //Number of LEDs
          pError("Must reboot for changes to take effect");
          setupMenu(ledcount);
          break;
        case 2: //Direction
          setupMenu(ledflip);
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case ledorder:
      //Set LEDS strip type after user verifies
      switch(sel){
        case 0:
          sLEDString->order = rgb;
          break;
        case 1:
          sLEDString->order = rbg;
          break;
        case 2:
          sLEDString->order = grb;
          break;
        case 3:
          sLEDString->order = gbr;
          break;
        case 4:
          sLEDString->order = brg;
          break;
        case 5:
          sLEDString->order = bgr;
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      pError("TODO: ADD SAVE");
      pError("Saved. Please reset.");
      return -1;
      break;
    case ledcount:
      sLEDString->ledCount = sel;
      pError("TODO: ADD SAVE");
      pError("Saved. Please reset.");
      return -1;
      break;
    case ledflip:
      if(sel == 1){
        sLEDString->reversed = true;
      } else {
        sLEDString->reversed = false;
      }
      pError("TODO: ADD SAVE");
      pError("Saved");
      return -1;
      break;
    case brakeandturn:
      switch(sel){
        case 0: //Set brakes
          pError("Ensure wires are connected properly");
          sel = 0;
          setupMenu(brake);
          break;
        case 1: //Turn signal behaviours
          pError("Ensure wires are connected properly");
          sel = 0;
          setupMenu(turn);
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      break;
    case brake:
      switch(sel){
        case 0:
          deviceSettings.brake = brakesignaloff;
          pError("TODO: save brake behaviour");
          pError("Saved");
          break;
        case 1:
          deviceSettings.brake = onnodim;
          pError("TODO: save brake behaviour");
          pError("Saved");
          break;
        case 2:
          deviceSettings.brake = onwithdim;
          pError("TODO: save brake behaviour");
          pError("Saved");
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      return -1;
      break;
    case turn:
      switch(sel){
        case 0:
          deviceSettings.turn = turnsignaloff;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        case 1:
          deviceSettings.turn = solid;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        case 2:
          deviceSettings.turn = solidnrear;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        case 3:
          deviceSettings.turn = solidnboth;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        case 4:
          deviceSettings.turn = chase;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        case 5:
          deviceSettings.turn = chasenrear;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        case 6:
          deviceSettings.turn = chasenboth;
          pError("TODO: save turn behaviour");
          pError("Saved");
          break;
        default:
          sel = 0;
          pError("Error: Out of bounds");
          break;
      }
      return -1;
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
    case autoshutoff:
      if(sel == 1){
        deviceSettings.autoshutoff = true;
      } else {
        deviceSettings.autoshutoff = false;
      }
      pError("TODO: Add save");
      pError("Saved");
      return -1;
      break;
    case screenbright:
      deviceSettings.brightness = sel*16;
      pError("TODO: ADD SAVE");
      pError("Saved");
      break;
    default://Menu ID unknown
      pError("Error: Menu does not exist");
      return -1;
      break;
  }
}

int8_t getInput(){
  int analogVal = analogRead(JOYSTICK_Y);
  //Serial.print("Joytick Y: ");
  //Serial.println(analogVal);
  if(analogVal >= DEADZONEHIGH){
    return DOWN;
  } else if(analogVal <= DEADZONELOW){
    return UP;
  }
  analogVal = analogRead(JOYSTICK_X);
  //Serial.print("Joytick X: ");
  //Serial.println(analogVal);
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
