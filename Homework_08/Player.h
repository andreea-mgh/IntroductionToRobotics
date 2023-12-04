#ifndef Player_h
#define Player_h
#include "Arduino.h"
#include "LedControl.h"

class Player {
  public:
    Player(LedControl* lc, int x = 0, int y = 0);
    void moveTo(int x, int y);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void draw(unsigned long blinkRate, int cameraX, int cameraY);
    int getX();
    int getY();
  private:
    int x;
    int y;
    bool moved;
    int lastX;
    int lastY;
    LedControl* ledMatrix;
};

#endif