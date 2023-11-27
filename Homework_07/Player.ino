#include "Arduino.h"
#include "LedControl.h"
#include "Player.h"
Player::Player(LedControl* lc, int x = 0, int y = 0) {
	this->x = x;
  this->y = y;
  ledMatrix = lc;
}

void Player::moveTo(int x, int y){
  this->x = x;
  this->y = y;
}

void Player::moveUp(){
  if (y > 0) {
    ledMatrix->setLed(0, y, x, false);
    y--;
    ledMatrix->setLed(0, y, x, true);
  }
}

void Player::moveDown(){
  if (y < 7) {
    ledMatrix->setLed(0, y, x, false);
    y++;
    ledMatrix->setLed(0, y, x, true);
  }
}

void Player::moveLeft(){
  if (x > 0) {
    ledMatrix->setLed(0, y, x, false);
    x--;
    ledMatrix->setLed(0, y, x, true);
  }
}

void Player::moveRight(){
  if (x < 7) {
    ledMatrix->setLed(0, y, x, false);
    x++;
    ledMatrix->setLed(0, y, x, true);
  }
}

void Player::draw(unsigned long blinkRate){
  if(millis() % blinkRate < blinkRate / 2){
    ledMatrix->setLed(0, y, x, false);
  } else {
    ledMatrix->setLed(0, y, x, true);
  }
}

int Player::getX(){
  return x;
}

int Player::getY(){
  return y;
}
