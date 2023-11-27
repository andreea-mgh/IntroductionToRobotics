#ifndef Player_h
#define Player_h
#include "Arduino.h"
#include "LedControl.h"

class Player {
  public:
    Player(LedControl* lc, int x = 0, int y = 0);
    void setBlinkRate(int blinkRate);
    void moveTo(int x, int y);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void draw();
    int getX();
    int getY();
  private:
    int x;
    int y;
    LedControl* ledMatrix;
    unsigned long blinkRate;
};

#endif