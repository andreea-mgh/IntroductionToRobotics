#ifndef Bomb_h
#define Bomb_h
#include "Arduino.h"
#include "LedControl.h"

class Bomb {
  public:
    Bomb();
    Bomb(LedControl* lc, int x = 0, int y = 0);
    void setExplodeTime(unsigned long explodeTime);
    void draw(unsigned long blinkRate);
    int getX();
    int getY();
    bool isExploded();
  private:
    int x;
    int y;
    LedControl* ledMatrix;
    unsigned long placeTime;
    unsigned long explodeTime;
};

#endif