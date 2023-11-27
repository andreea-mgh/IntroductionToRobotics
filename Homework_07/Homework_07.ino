#include <LedControl.h>  // need the library
#include <Vector.h>
#include "Player.h"
#include "Bomb.h"
const byte dinPin = 12;                                    // pin 12 is connected to the MAX7219 pin 1
const byte clockPin = 11;                                  // pin 11 is connected to the CLK pin 13
const byte loadPin = 10;                                   // pin 10 is connected to LOAD pin 12
const byte matrixSize = 8;                                 // 1 as we are only using 1 MAX7219
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;

const int pinX = A0;
const int pinY = A1;
const int buttonPin = 2;

const int joystickMinThreshold = 384;
const int joystickMaxThreshold = 640;

unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 100;

bool buttonPressed = false;
unsigned long lastButtonPressTime = 0;
unsigned long debounceDelay = 100;
volatile bool bombPlaced = false;

bool gameLost = false;
bool gameWon = false;
int bombRadius = 1;


Player bomber = Player(&lc, 0, 0);
// Vector<Bomb> bombs;
Bomb bombs[10];
int numBombs = 0;

byte gameMap[matrixSize][matrixSize] = {
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
      if (gameMap[i][j] == 1) {
        lc.setLed(0, i, j, true);
      } else {
        lc.setLed(0, i, j, false);
      }
    }
  }
}

void buttonPress() {

  static unsigned long interruptTime = 0;
  interruptTime = micros();

  if (interruptTime - lastButtonPressTime > debounceDelay * 1000) {
    buttonPressed = !buttonPressed;
    if (buttonPressed) {
      bombPlaced = true;
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

    if (bombPlaced) {
      bombs[numBombs] = Bomb(&lc, bomber.getX(), bomber.getY());
      numBombs++;
      bombPlaced = false;
    }

    // are there bombs in the queue?
    if (numBombs > 0) {
      // did the first one detonate?
      if (bombs[0].isExploded()) {
        numBombs--;
        lc.setLed(0, bombs[0].getX(), bombs[0].getY(), false);
        for (int i = max(0, bombs[0].getY() - bombRadius); i <= min(7, bombs[0].getY() + bombRadius); i++) {
          for (int j = max(0, bombs[0].getX() - bombRadius); j <= min(7, bombs[0].getX() + bombRadius); j++) {
            // Serial.print("removed:");
            // Serial.print(i);
            // Serial.print(",");
            // Serial.println(j);
            gameMap[i][j] = 0;
            if (bomber.getX() == j && bomber.getY() == i) {
              gameLost = true;
            }
          }
        }
        drawMap();
        for (int i = 0; i < numBombs; i++) {
          bombs[i] = bombs[i + 1];
        }
      }
      // draw bombs
      for (int i = 0; i < numBombs; i++) {
        bombs[i].draw();
      }
    }
    if(bomber.getX() == 7 && bomber.getY() == 7){
      gameWon = true;
    }
    bomber.draw();
  }
  

  else if (gameLost) {
    lc.clearDisplay(0);
    lc.setLed(0, 0, 0, true);
    lc.setLed(0, 0, 7, true);
    lc.setLed(0, 7, 0, true);
    lc.setLed(0, 7, 7, true);
    Serial.println("Game Over");
  }

  else if(gameWon){
    lc.clearDisplay(0);
    lc.setLed(0, 0, 0, true);
    lc.setLed(0, 0, 7, true);
    lc.setLed(0, 7, 0, true);
    lc.setLed(0, 7, 7, true);
    Serial.println("You Win!");
  }
}