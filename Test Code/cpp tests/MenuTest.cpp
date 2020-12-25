#include<iostream>

//Menu state
enum State{
  emain, settings, gauge1, pattern1, pattern2, pattern3, bluetooth, ledsetting1, ledsetting2,
  brakeandturn, brake, turn, color, pickpattern, animation, ledorder, ledcount, ledflip,
  autoshutoff, screenbright, palette, presetcolor1, presetcolor2, colornumset, 
};

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

//Example
//title = "Forest Green"
//Desc = "Pretty greens"
//Passive = Save Active to Test
//Active = Set FG pallete to Active

typedef void (*PassiveFunc)(void*);

void getInt(void* ptr){
  int *iPtr; //Declare new ptr
  iPtr = (int*)ptr;
  std::cout << iPtr << std::endl;
}

void getChar(void* ptr){
  char *iPtr; //Declare new ptr
  iPtr = (char*)ptr;
  std::cout << iPtr << std::endl;
}

// MenuItem Struc for objects
typedef struct{
  char *title; //Title text
  char *desc; //Description text
  PassiveFunc p;
  typedef void (*ActiveFunc)(void*);
  void *item; //void pointer to held data
  int8_t pid;
} MenuItem;

MenuItem MI1;

int main(){
  int i = 1024;
  char text = 'g';
  MI1.p = getInt;
  MI1.p(&i);
  MI1.p = getChar;
  MI1.p(&text);
}













