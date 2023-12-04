#include "Arduino.h"
#include "LedControl.h"
#include "Monster.h"

Monster::Monster() {}

Monster::Monster(LedControl* lc, int x = 0, int y = 0) {
  this->x = x;
  this->y = y;
  ledMatrix = lc;
}

void Monster::draw(unsigned long blinkRate){
  if(millis() % blinkRate < blinkRate / 2){
    ledMatrix->setLed(0, y, x, false);
  } else {
    ledMatrix->setLed(0, y, x, true);
  }
}

int Monster::getX(){
  return x;
}

int Monster::getY(){
  return y;
}