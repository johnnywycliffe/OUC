#include<iostream>


// MenuItem Struc for objects
typedef struct{
  char *title; //Title text
  char *desc; //Description text
  int8_t pid; //For storing PIDs
} MenuItem;

// Menu class
class Menu{
  MenuItem *itemArr;
  int8_t len;
  int8_t ID;
  Menu *prevMenu;
public:
  Menu(Menu *prev = NULL){
    Menu *prevMenu;
  }
  ~Menu(){
    delete itemArr;
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
    itemArr[pos].title = t;
    itemArr[pos].desc = d;
    itemArr[pos].pid = PID;
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
  Menu* getPrevMenu(){
    return prevMenu;
  }
  void generateMenu(uint8_t arrLen){
    itemArr = new MenuItem[arrLen];
    len = arrLen;
  }
  void setPrev(Menu *prev){
    prevMenu = prev;
  }
};

void mendostuff(Menu *m1, Menu *m2){
    std::cout << m1 << std::endl;
    std::cout << m1->getDesc(0) << std::endl;
    std::cout << m1 << std::endl;
    std::cout << m2->getPrevMenu() << std::endl;
}

Menu m1;
Menu *ptr = &m1;
Menu m2(ptr);
Menu *ptr2 = &m2;
    
int main()
{
    ptr->generateMenu(4);
    if(true){
        ptr2->generateMenu(4);
    }
    ptr = ptr2;
    ptr->setItem(0,"12345678901","abc",34);
    m1.setItem(2,"elevenchars","def",69);
    m1.setItem(1,"fuckyoutoo!","ghi",420);
    mendostuff(ptr, ptr2);
}
