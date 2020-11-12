#include<iostream>

typedef struct{
  char *title; //Title text
  char *desc; //Description text
  int returnVal; //Value to return to 
} MenuItem;

class Menu{
  MenuItem *items;
  int8_t len;
public:
  Menu(int8_t arrLen){
    len = arrLen;
    items = new MenuItem[arrLen];
  }
  ~Menu(){
    delete items;
  }
  int8_t getLen(){
    return len;
  }
  void setItem(int8_t pos, char *t, char *d, int retVal){
    items[pos].title = t;
    items[pos].desc = d;
    items[pos].returnVal = retVal;
  }
  char* getTitle(int pos){
    return items[pos].title;
  }
  char* getDesc(int pos){
    return items[pos].desc;
  }
  int getReturnVal(int pos){
    return items[pos].returnVal;
  }
};

void mendostuff(Menu *m){
    std::cout << m << std::endl;
    //std::cout << *m << std::endl;
    std::cout << m->getDesc(2) << std::endl;
}

int main()
{
    Menu m1(4);
    Menu *ptr = &m1;
    ptr->setItem(0,"12345678901","abc",34);
    m1.setItem(2,"elevenchars","def",69);
    m1.setItem(1,"fuckyoutoo!","ghi",420);
    std::cout << m1.getTitle(0) << std::endl;
    std::cout << m1.getDesc(0) << std::endl;
    std::cout << m1.getReturnVal(0) << std::endl;
    std::cout << m1.getTitle(1) << std::endl;
    std::cout << m1.getDesc(1) << std::endl;
    std::cout << m1.getReturnVal(1) << std::endl;
    std::cout << m1.getTitle(2) << std::endl;
    std::cout << m1.getDesc(2) << std::endl;
    std::cout << m1.getReturnVal(2) << std::endl;
    mendostuff(ptr);
}
