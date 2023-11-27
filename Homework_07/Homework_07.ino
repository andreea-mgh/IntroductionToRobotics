#include <LedControl.h>
#include "Player.h"
#include "Bomb.h"
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = 2;

const int pinX = A0;
const int pinY = A1;
const int buttonPin = 2;

// joystick parameters
const int joystickMinThreshold = 384;
const int joystickMaxThreshold = 640;

// movement speed
unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 100;

bool buttonPressed = false;
unsigned long lastButtonPressTime = 0;
unsigned long debounceDelay = 100;
volatile bool buttonTrigger = false;

bool gameLost = false;
bool gameWon = false;

// settings
int bombRadius = 3;
int playerBlinkRate = 400;
int bombBlinkRate = 150;

Player bomber = Player(&lc, 0, 0);
Bomb bombs[10];
int numBombs = 0;

bool gameMap[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
};

void drawMap() {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, gameMap[i][j]);
    }
  }
}

void buttonPress() {

  static unsigned long interruptTime = 0;
  interruptTime = micros();

  if (interruptTime - lastButtonPressTime > debounceDelay * 1000) {
    buttonPressed = !buttonPressed;
    if (buttonPressed) {
      buttonTrigger = true;
    }
  }

  lastButtonPressTime = interruptTime;
}


void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, CHANGE);

  randomSeed(analogRead(A5));

  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      // wall around player
      if((i == 3 && j < 3) || i < 3 && j == 3){
        gameMap[i][j] = 1;
      }
      // wall around exit
      if((i == 5 && j > 5) || i > 5 && j == 5){
        gameMap[i][j] = 1;
      }
      // random blocks
      if ((i > 3 || j > 3) && (i < 5 || j < 5)) {
        int randNumber = random(0, 100);
        if (randNumber < 50) {
          gameMap[i][j] = 1;
        } else {
          gameMap[i][j] = 0;
        }
      }
    }
  }

  drawMap();
}

void loop() {
  if(!gameLost && !gameWon) {
    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);

    if (millis() - lastMoveTime > moveDelay) {
      if (xValue < joystickMinThreshold) {
        if (bomber.getX() > 0) {

          if (gameMap[bomber.getY()][bomber.getX() - 1] == 0) {
            bomber.moveLeft();
            lastMoveTime = millis();
          }
        }
      }
      if (xValue > joystickMaxThreshold) {
        if (bomber.getX() < 7) {

          if (gameMap[bomber.getY()][bomber.getX() + 1] == 0) {
            bomber.moveRight();
            lastMoveTime = millis();
          }
        }
      }
      if (yValue < joystickMinThreshold) {
        if (bomber.getY() > 0) {

          if (gameMap[bomber.getY() - 1][bomber.getX()] == 0) {
            bomber.moveUp();
            lastMoveTime = millis();
          }
        }
      }
      if (yValue > joystickMaxThreshold) {
        if (bomber.getY() < 7) {

          if (gameMap[bomber.getY() + 1][bomber.getX()] == 0) {
            bomber.moveDown();
            lastMoveTime = millis();
          }
        }
      }
    }

    if (buttonTrigger) {
      bombs[numBombs] = Bomb(&lc, bomber.getX(), bomber.getY());
      numBombs++;
      buttonTrigger = false;
    }

    // are there bombs in the queue?
    if (numBombs > 0) {
      // did the first one detonate?
      if (bombs[0].isExploded()) {
        numBombs--;
        lc.setLed(0, bombs[0].getX(), bombs[0].getY(), false);
        // for (int i = max(0, bombs[0].getY() - bombRadius); i <= min(7, bombs[0].getY() + bombRadius); i++) {
        //   for (int j = max(0, bombs[0].getX() - bombRadius); j <= min(7, bombs[0].getX() + bombRadius); j++) {
        //     gameMap[i][j] = 0;
        //     if (bomber.getX() == j && bomber.getY() == i) {
        //       gameLost = true;
        //     }
        //   }
        // }

        // north
        for(int i = max(0, bombs[0].getY() - 1); i >= max(0, bombs[0].getY() - bombRadius); i--){
          if(gameMap[i][bombs[0].getX()] == 1){
            gameMap[i][bombs[0].getX()] = 0;
            break;
          }
          if (bomber.getX() == bombs[0].getX() && bomber.getY() == i) {
            gameLost = true;
          }
        }
        // south
        for(int i = min(7, bombs[0].getY() + 1); i <= min(7, bombs[0].getY() + bombRadius); i++){
          if(gameMap[i][bombs[0].getX()] == 1){
            gameMap[i][bombs[0].getX()] = 0;
            break;
          }
          if (bomber.getX() == bombs[0].getX() && bomber.getY() == i) {
            gameLost = true;
          }
        }
        // west
        for(int i = max(0, bombs[0].getX() - 1); i >= max(0, bombs[0].getX() - bombRadius); i--){
          if(gameMap[bombs[0].getY()][i] == 1){
            gameMap[bombs[0].getY()][i] = 0;
            break;
          }
          if (bomber.getX() == i && bomber.getY() == bombs[0].getY()) {
            gameLost = true;
          }
        }
        // east
        for(int i = min(7, bombs[0].getX() + 1); i <= min(7, bombs[0].getX() + bombRadius); i++){
          if(gameMap[bombs[0].getY()][i] == 1){
            gameMap[bombs[0].getY()][i] = 0;
            break;
          }
          if (bomber.getX() == i && bomber.getY() == bombs[0].getY()) {
            gameLost = true;
          }
        }



        drawMap();
        for (int i = 0; i < numBombs; i++) {
          bombs[i] = bombs[i + 1];
        }
      }
      // draw remaining bombs
      for (int i = 0; i < numBombs; i++) {
        bombs[i].draw(bombBlinkRate);
      }
    }

    // did bomber reach the exit?
    if(bomber.getX() == 7 && bomber.getY() == 7){
      gameWon = true;
    }
    bomber.draw(playerBlinkRate);
  }
  

  else if (gameLost) {
    lc.clearDisplay(0);
    lc.setLed(0, 1, 1, true);
    lc.setLed(0, 2, 2, true);
    lc.setLed(0, 3, 3, true);
    lc.setLed(0, 4, 4, true);
    lc.setLed(0, 5, 5, true);
    lc.setLed(0, 5, 1, true);
    lc.setLed(0, 4, 2, true);
    lc.setLed(0, 2, 4, true);
    lc.setLed(0, 1, 5, true);
    Serial.println("Game Over");
  }


  else if(gameWon){
    lc.clearDisplay(0);
    lc.setLed(0, 4, 1, true);
    lc.setLed(0, 5, 2, true);
    lc.setLed(0, 6, 3, true);
    lc.setLed(0, 5, 4, true);
    lc.setLed(0, 4, 5, true);
    lc.setLed(0, 3, 6, true);
    lc.setLed(0, 2, 7, true);
    Serial.println("You Win!");
  }
}