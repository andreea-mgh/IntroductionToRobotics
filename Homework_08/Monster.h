#ifndef Monster_h
#define Monster_h
#include "Arduino.h"
#include "LedControl.h"

class Monster {
  public:
    Monster();
    Monster(LedControl* lc, int x = 0, int y = 0);
    void draw(unsigned long blinkRate);
    int getX();
    int getY();
  private:
    int x;
    int y;
    LedControl* ledMatrix;
};

#endif