#include "Arduino.h"
#include "LedControl.h"
#include "Player.h"
Player::Player(LedControl* lc, int x = 0, int y = 0) {
	this->x = x;
  this->y = y;
  this->lives = 3;
  ledMatrix = lc;
}

void Player::moveTo(int x, int y){
  this->x = x;
  this->y = y;
}

void Player::moveUp(){
  ledMatrix->setLed(0, y-cameraY, x-cameraX, false);
  if (y > 0) {
    y--;
  }
}

void Player::moveDown(){
  ledMatrix->setLed(0, y-cameraY, x-cameraX, false);
  if (y < 15) {
    y++;
  }
}

void Player::moveLeft(){
  ledMatrix->setLed(0, y-cameraY, x-cameraX, false);
  if (x > 0) {
    x--;
  }
}

void Player::moveRight(){
  ledMatrix->setLed(0, y-cameraY, x-cameraX, false);
  if (x < 15) {
    x++;
  }
}

void Player::draw(unsigned long blinkRate){
  if(millis() % blinkRate < blinkRate / 2){
    ledMatrix->setLed(0, y-cameraY, x-cameraX, false);
  } else {
    ledMatrix->setLed(0, y-cameraY, x-cameraX, true);
  }
}

int Player::getX(){
  return x;
}

int Player::getY(){
  return y;
}

int Player::getLives(){
  return lives;
}

void Player::addLife(){
  lives++;
}

void Player::removeLife(){
  lives--;
}

void Player::setLives(int lives){
  this->lives = lives;
}