#include <LedControl.h>
#include <LiquidCrystal.h>
#include "Player.h"
#include "Monster.h"
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl ledMatrix(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = 2;
const byte lcd_rs = 9;
const byte lcd_en = 8;
const byte lcd_d4 = 7;
const byte lcd_d5 = 6;
const byte lcd_d6 = 5;
const byte lcd_d7 = 4;
LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

byte lifeChar[8] = {
	0b00000,
	0b00000,
	0b01010,
	0b11111,
	0b11111,
	0b01110,
	0b00100,
	0b00000
};

const int pinX = A0;
const int pinY = A1;
const int buttonPin = 2;

const int buzzPin = 13;

// joystick parameters
const int joystickMinThreshold = 384;
const int joystickMaxThreshold = 640;

// movement speed
unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 100;
const unsigned long menuMoveDelay = 300;


bool buttonPressed = false;
unsigned long lastButtonPressTime = 0;
unsigned long debounceDelay = 100;
volatile bool buttonTrigger = false;

bool gameWon = false;
int currentOption = 0;
int currentMenu = 0;
// 0 = main menu
// 1 = LCD brightness
// 2 = Matrix brightness
int gameStatus = 0;
// 0 = main menu
// 1 = game
// 2 = game over
// 3 = 

// settings
int playerBlinkRate = 400;
int bombBlinkRate = 100;
int flashTime = 500; 

bool flash = true;
int cameraX = 0;
int cameraY = 0;

Player survivor(&ledMatrix, 1, 1);
Monster monsters[10];
int numMonsters = 0;


const int mapSize = 16;
const int numLevels = 1;

byte gameMap[mapSize][mapSize];

const byte gameLevels[numLevels][mapSize][mapSize] PROGMEM = {
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  }
};

const uint64_t winAnimation[] PROGMEM = {
  0x0008142240800000,
  0x08142a55a2408000
};

const uint64_t loseAnimation[] PROGMEM = {
  0x0022140814220000,
  0x22552a142a552200
};

void loadLevel(int level = 0) {
  if(level < numLevels) {
    for(int i = 0; i < mapSize; i++) {
      for(int j = 0; j < mapSize; j++) {
        gameMap[i][j] = pgm_read_byte(&gameLevels[level][i][j]);
        Serial.print(gameMap[i][j]);
        Serial.print(" ");
      }
      Serial.print("\n");
    }
  }
}

void drawMap(int x, int y) {
  Serial.print("drawMap: ");
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print("\n");
  ledMatrix.clearDisplay(0);
  for (int i = y; i < y + matrixSize; i++) {
    for (int j = x; j < x + matrixSize; j++) {
      ledMatrix.setLed(0, i-y, j-x, gameMap[i][j]);
    }
  }
}

void displayImage(uint64_t image) {
  // SOURCE: xantorohara.github.io/led-matrix-editor
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      ledMatrix.setLed(0, i, j, bitRead(row, j));
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
  ledMatrix.shutdown(0, false);
  ledMatrix.setIntensity(0, matrixBrightness);
  ledMatrix.clearDisplay(0);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, CHANGE);
  
  lcd.begin(16, 2);

  lcd.createChar(0, lifeChar);

  randomSeed(analogRead(A5));

  noTone(buzzPin);
  lcd.setCursor(0, 0);
  lcd.print("////////////////");
  lcd.setCursor(0, 1);
  lcd.print("///THE DEEP DARK");
  tone(buzzPin, 100);
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("////////////////");
  lcd.setCursor(0, 1);
  lcd.print("///             ");
  noTone(buzzPin);
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("////////////////");
  lcd.setCursor(0, 1);
  lcd.print("///THE DEEP DARK");
  tone(buzzPin, 100);
  delay(200);

  lcd.setCursor(0, 0);
  lcd.print("////////////////");
  lcd.setCursor(0, 1);
  lcd.print("///             ");
  noTone(buzzPin);
  delay(200);

  lcd.setCursor(0, 0);
  lcd.print("////////////////");
  lcd.setCursor(0, 1);
  lcd.print("///THE DEEP DARK");
  tone(buzzPin, 100);
  delay(400);

  lcd.setCursor(0, 0);
  lcd.print("||||||||||||||||");
  lcd.setCursor(0, 1);
  lcd.print("||||||||||||||||");
  delay(1000);
  noTone(buzzPin);

  survivor = Player(&ledMatrix, 1, 1);
  loadLevel(0);
  drawMap(cameraX, cameraY);
}

void loop() {

  if(gameStatus == 0) {
    lcd.setCursor(0, 1);
    lcd.print("///THE DEEP DARK");
    lcd.setCursor(0, 0);
    if(currentOption == 0) {
      lcd.print("   START       >");
    }
    else if(currentOption == 1) {
      lcd.print("<  SETTINGS    >");
    }
    else {
      lcd.print("<  ABOUT        ");
    }

    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);

    if (millis() - lastMoveTime > menuMoveDelay) {
      if (xValue < joystickMinThreshold && currentOption > 0) {
        currentOption--;
      }
      if (xValue > joystickMaxThreshold) {
        if(currentOption < 2) {
          currentOption++;
        }
      }
      lastMoveTime = millis();
    }

    if(buttonTrigger) {
      if(currentOption == 0) {
        gameStatus = 1;
      }
      else if (currentOption == 1) {
        // settings
        ;
      }
      else {
        // about
        ;
      }
      buttonTrigger = false;
    }
  }



  if(gameStatus == 1) {

    // player movement
    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);

    if (millis() - lastMoveTime > moveDelay) {
      if (xValue < joystickMinThreshold) {
        if (survivor.getX() > 0) {

          if (gameMap[survivor.getY()][survivor.getX() - 1] == 0) {
            survivor.moveLeft();
            lastMoveTime = millis();
          }
        }
      }
      if (xValue > joystickMaxThreshold) {
        if (survivor.getX() < mapSize) {

          if (gameMap[survivor.getY()][survivor.getX() + 1] == 0) {
            survivor.moveRight();
            lastMoveTime = millis();
          }
        }
      }
      if (yValue < joystickMinThreshold) {
        if (survivor.getY() > 0) {

          if (gameMap[survivor.getY() - 1][survivor.getX()] == 0) {
            survivor.moveUp();
            lastMoveTime = millis();
          }
        }
      }
      if (yValue > joystickMaxThreshold) {
        if (survivor.getY() < mapSize) {

          if (gameMap[survivor.getY() + 1][survivor.getX()] == 0) {
            survivor.moveDown();
            lastMoveTime = millis();
          }
        }
      }
    }

    if(survivor.getX() - cameraX > 5 && cameraX < mapSize - matrixSize) {
      cameraX = survivor.getX() - 5;
      drawMap(cameraX, cameraY);
    }
    if(survivor.getY() - cameraY > 5 && cameraY < mapSize - matrixSize) {
      cameraY = survivor.getY() - 5;
      drawMap(cameraX, cameraY);
    }
    if(survivor.getX() - cameraX < 2 && cameraX > 0) {
      cameraX = survivor.getX() - 2;
      drawMap(cameraX, cameraY);
    }
    if(survivor.getY() - cameraY < 2 && cameraY > 0) {
      cameraY = survivor.getY() - 2;
      drawMap(cameraX, cameraY);
    }

    if(survivor.getX() == mapSize - 2 && survivor.getY() == mapSize - 2) {
      gameWon = true;
      gameStatus = 2;
    }
    

    // draw player
    survivor.draw(playerBlinkRate, cameraX, cameraY);

    // Serial.print(cameraX);
    // Serial.print(cameraY);
    // Serial.print(survivor.getX());
    // Serial.print(survivor.getY());
    // Serial.print("\n");
  }

}