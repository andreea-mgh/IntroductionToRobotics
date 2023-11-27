#include "Arduino.h"
#include "LedControl.h"
#include "Bomb.h"

Bomb::Bomb() {}

Bomb::Bomb(LedControl* lc, int x = 0, int y = 0) {
  this->x = x;
  this->y = y;
  ledMatrix = lc;
  placeTime = millis();
  blinkRate = 150;
  explodeTime = 2000;
}

void Bomb::setBlinkRate(int blinkRate){
  this->blinkRate = blinkRate;
}

void Bomb::setExplodeTime(unsigned long explodeTime){
  this->explodeTime = explodeTime;
}

void Bomb::draw(){
  if(millis() % blinkRate < blinkRate / 2){
    ledMatrix->setLed(0, y, x, false);
  } else {
    ledMatrix->setLed(0, y, x, true);
  }
}

int Bomb::getX(){
  return x;
}

int Bomb::getY(){
  return y;
}

bool Bomb::isExploded(){
  return millis() - placeTime > explodeTime;
}