#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "Player.h"
#include "Monster.h"
const byte dinPin = 13;
const byte clockPin = 12;
const byte loadPin = 11;
const byte matrixSize = 8;
LedControl ledMatrix(dinPin, clockPin, loadPin, 1);
const byte lcd_rs = 9;
const byte lcd_en = 8;
const byte lcd_d4 = 7;
const byte lcd_d5 = 6;
const byte lcd_d6 = 5;
const byte lcd_d7 = 4;
const byte lcd_bl = 10;
LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

byte lcdBrightness = 128;
byte lcdBrightnessMax = 255;
byte matrixBrightness = 2;
byte matrixBrightnessMax = 15;

const int pinX = A0;
const int pinY = A1;
const int buttonPin = 2;

const int buzzPin = 3;

// joystick parameters
const int joystickMinThreshold = 384;
const int joystickMaxThreshold = 640;

// movement speed
unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 250;
const unsigned long menuMoveDelay = 250;

unsigned long levelStartTime = 0;
unsigned long currentScore = 0;

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

// settings
int playerBlinkRate = 400;
int bombBlinkRate = 100;

unsigned long flashStartTime = 0;
const unsigned long flashDuration = 1500;
bool flash = false;

int cameraX = 0;
int cameraY = 0;

Player survivor(&ledMatrix, 1, 1);
Monster monsters[10];
int numMonsters = 0;

const int mapSize = 16;
const int numLevels = 5;
int currentLevel = 0;

const int winBonus = 1000;
const int levelCompleteBonus = 100;
const unsigned long bonusTimeLimit[numLevels] = {60, 90, 120, 180, 240};


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

byte timeChar[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b10101,
	0b10111,
	0b10001,
	0b01110,
	0b00000
};

byte flashChar[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b10101,
	0b10101,
	0b01110,
	0b00000,
	0b00000
};

const uint64_t matrixImages[] = {
  0x6699997e9999a5c3
};

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
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
  },
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1 },
    { 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1 },
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
  survivor = Player(&ledMatrix, 2, 2);
  cameraX = 0;
  cameraY = 0;
  flash = false;
  ledMatrix.clearDisplay(0);
  lcd.clear();
  if(level < numLevels) {
    for(int i = 0; i < mapSize; i++) {
      for(int j = 0; j < mapSize; j++) {
        gameMap[i][j] = pgm_read_byte(&gameLevels[level][i][j]);
      }
    }
  }
  levelStartTime = millis();
}

void drawMap(int x, int y) {
  Serial.print(F("drawMap: "));
  Serial.print(x);
  Serial.print(F(" "));
  Serial.print(y);
  Serial.print(F("\n"));
  ledMatrix.clearDisplay(0);
  for (int i = y; i < y + matrixSize; i++) {
    for (int j = x; j < x + matrixSize; j++) {
      ledMatrix.setLed(0, i-y, j-x, gameMap[i][j]);
    }
  }
}

void see(int cameraX, int cameraY, int x, int y) {
  for(int i=-1; i<=1; i++) {
    for(int j=-1; j<=1; j++) {
      if(x+i >= 0 && x+i < mapSize && y+j >= 0 && y+j < mapSize) {
        ledMatrix.setLed(0, y+j-cameraY, x+i-cameraX, gameMap[y+j][x+i]);
        Serial.print(F("see: "));
        Serial.print(x+i-cameraX);
        Serial.print(F(" "));
        Serial.print(y+j-cameraY);
        Serial.print(F(" "));
        Serial.print(gameMap[y+j][x+i]);
        Serial.print(F("\n"));
      }
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
  Serial.println(sizeof(gameLevels));

  lcdBrightness = EEPROM.read(0);
  matrixBrightness = EEPROM.read(1);

  ledMatrix.shutdown(0, false);
  ledMatrix.setIntensity(0, matrixBrightness);
  ledMatrix.clearDisplay(0);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPress, CHANGE);

  pinMode(lcd_bl, OUTPUT);
  
  lcd.begin(16, 2);

  lcd.createChar(0, lifeChar);
  lcd.createChar(1, timeChar);
  lcd.createChar(2, flashChar);

  randomSeed(analogRead(A5));

  noTone(buzzPin);
  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///SHADOW ESCAPE"));
  tone(buzzPin, 100);
  delay(100);
  analogWrite(lcd_bl, lcdBrightness);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///             "));
  noTone(buzzPin);
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///SHADOW ESCAPE"));
  tone(buzzPin, 100);
  delay(200);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///             "));
  noTone(buzzPin);
  delay(200);

  lcd.setCursor(0, 0);
  lcd.print(F("////////////////"));
  lcd.setCursor(0, 1);
  lcd.print(F("///SHADOW ESCAPE"));
  tone(buzzPin, 100);
  delay(400);

  lcd.setCursor(0, 0);
  lcd.print(F("||||||||||||||||"));
  lcd.setCursor(0, 1);
  lcd.print(F("||||||||||||||||"));
  delay(1000);
  noTone(buzzPin);

  displayImage(matrixImages[0]);
}

void loop() {

  // MENU
  if(gameStatus == 0) {
    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);

    // MAIN MENU
    if(currentMenu == 0) {
      lcd.setCursor(0, 1);
      lcd.print(F("///SHADOW ESCAPE"));

      lcd.setCursor(0, 0);
      if(currentOption == 0) {
        lcd.print(F("   START       >"));
        if(buttonTrigger) {
          gameStatus = 1;
          currentLevel = 0;
          loadLevel(currentLevel);
          buttonTrigger = false;
        }
      }
      else if(currentOption == 1) {
        lcd.print(F("<  SETTINGS    >"));
        if(buttonTrigger) {
          lcd.clear();
          currentMenu = 1;
          currentOption = 0;
          buttonTrigger = false;
        }
      }
      else {
        lcd.print(F("<  ABOUT        "));
        if(buttonTrigger) {
          lcd.clear();
          currentMenu = 2;
          currentOption = 0;
          buttonTrigger = false;
        }
      }

      if (millis() - lastMoveTime > menuMoveDelay) {
        if (xValue < joystickMinThreshold && currentOption > 0) {
          lcd.clear();
          currentOption--;
          lastMoveTime = millis();
        }
        if (xValue > joystickMaxThreshold && currentOption < 2) {
          lcd.clear();
          currentOption++;
          lastMoveTime = millis();
        }
      }
    }

    // SETTINGS
    else if(currentMenu == 1) {
      lcd.setCursor(0, 0);
      if(currentOption == 0) {
        lcd.print(F("   LCD         >"));
        lcd.setCursor(0, 1);
        lcd.print(lcdBrightness);

      }
      else if(currentOption == 1) {
        lcd.print(F("<  MATRIX      >"));
        lcd.setCursor(0, 1);
        lcd.print(matrixBrightness);
      }
      else {
        lcd.print(F("<  BACK         "));
        if(buttonTrigger) {
          currentMenu = 0;
          currentOption = 0;
          buttonTrigger = false;
          EEPROM.update(0, lcdBrightness);
          EEPROM.update(1, matrixBrightness);
        }
      }

      if (millis() - lastMoveTime > menuMoveDelay) {
        if (xValue < joystickMinThreshold && currentOption > 0) {
          lcd.clear();
          currentOption--;
          lastMoveTime = millis();
        }
        if (xValue > joystickMaxThreshold && currentOption < 2) {
          lcd.clear();
          currentOption++;
          lastMoveTime = millis();
        }
        if (yValue < joystickMinThreshold) {
          if(currentOption == 0 && lcdBrightness < lcdBrightnessMax) {
            lcdBrightness++;
            analogWrite(lcd_bl, lcdBrightness);
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print(lcdBrightness);
            lcd.setCursor(0, 0);
            lcd.print(F("   LCD         >"));
            lcd.setCursor(0, 1);
            lcd.print(lcdBrightness);
            analogWrite(lcd_bl, lcdBrightness);
            Serial.println(lcdBrightness);
            lastMoveTime = millis();
          }
          else if(currentOption == 1 && matrixBrightness < matrixBrightnessMax) {
            matrixBrightness++;
            ledMatrix.setIntensity(0, matrixBrightness);
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print(matrixBrightness);
            lcd.setCursor(0, 0);
            lcd.print(F("   MATRIX      >"));
            lcd.setCursor(0, 1);
            lcd.print(matrixBrightness);
            lastMoveTime = millis();
          }
        }
        if(yValue > joystickMaxThreshold) {
          if(currentOption == 0 && lcdBrightness > 0) {
            lcdBrightness--;
            lcd.setCursor(0, 1);
            lcd.print(lcdBrightness);
            lcd.setCursor(0, 0);
            lcd.print(F("   LCD         >"));
            lcd.setCursor(0, 1);
            lcd.print(lcdBrightness);
            analogWrite(lcd_bl, lcdBrightness);
            Serial.println(lcdBrightness);
            lastMoveTime = millis();
          }
          else if(currentOption == 1 && matrixBrightness > 0) {
            matrixBrightness--;
            ledMatrix.setIntensity(0, matrixBrightness);
            lcd.setCursor(0, 1);
            lcd.print(matrixBrightness);
            lcd.setCursor(0, 0);
            lcd.print(F("   MATRIX      >"));
            lcd.setCursor(0, 1);
            lcd.print(matrixBrightness);
            lastMoveTime = millis();
          }
        }
      }
    }

    // ABOUT
    else if(currentMenu == 2) {
      if(currentOption == 0) {
        lcd.setCursor(0, 0);
        lcd.print(F("Shadow Escape  "));
        lcd.setCursor(0, 1);
        lcd.print(F("               "));
      }
      else if(currentOption == 1) {
        lcd.setCursor(0, 0);
        lcd.print(F("Author: Andreea"));
        lcd.setCursor(0, 1);
        lcd.print(F("     Megherlich"));
      }
      else if(currentOption == 2) {
        lcd.setCursor(0, 0);
        lcd.print(F("GitHub:         "));
        lcd.setCursor(0, 1);
        lcd.print(F("     andreea-mgh"));
      }
      else if(currentOption == 3) {
        lcd.setCursor(0, 0);
        lcd.print(F(" Introduction to"));
        lcd.setCursor(0, 1);
        lcd.print(F(" Robotics - 2023"));
      }
      else if(currentOption == 4) {
        lcd.setCursor(0, 0);
        lcd.print(F(" CLICK TO RETURN"));
        lcd.setCursor(0, 1);
        lcd.print(F("TO THE MAIN MENU"));
      }
      
      if(buttonTrigger) {
        if(currentOption < 4) {
          currentOption ++;
        }
        else {
          currentMenu = 0;
          currentOption = 0;
        }
        buttonTrigger = false;
      }
    }


  }



  // GAME
  else if(gameStatus == 1) {
    if(millis() - levelStartTime < 100) tone(buzzPin, 440);
    else if(millis() - levelStartTime < 400) tone(buzzPin, 587);
    else noTone(buzzPin);

    // player movement
    int xValue = analogRead(pinX);
    int yValue = analogRead(pinY);
    bool playerMoved = false;
    if (millis() - lastMoveTime > moveDelay) {
      if (xValue < joystickMinThreshold) {
        if (survivor.getX() > 0) {

          if (gameMap[survivor.getY()][survivor.getX() - 1] == 0) {
            survivor.moveLeft();
            playerMoved = true;
            lastMoveTime = millis();
            tone(buzzPin, 100, 50);
          }
        }
      }
      if (xValue > joystickMaxThreshold) {
        if (survivor.getX() < mapSize) {

          if (gameMap[survivor.getY()][survivor.getX() + 1] == 0) {
            survivor.moveRight();
            playerMoved = true;
            lastMoveTime = millis();
            tone(buzzPin, 100, 50);
          }
        }
      }
      if (yValue < joystickMinThreshold) {
        if (survivor.getY() > 0) {

          if (gameMap[survivor.getY() - 1][survivor.getX()] == 0) {
            survivor.moveUp();
            playerMoved = true;
            lastMoveTime = millis();
            tone(buzzPin, 100, 50);
          }
        }
      }
      if (yValue > joystickMaxThreshold) {
        if (survivor.getY() < mapSize) {

          if (gameMap[survivor.getY() + 1][survivor.getX()] == 0) {
            survivor.moveDown();
            playerMoved = true;
            lastMoveTime = millis();
            tone(buzzPin, 100, 50);
          }
        }
      }
    }

    // camera movement
    if(survivor.getX() - cameraX > 5 && cameraX < mapSize - matrixSize) {
      cameraX = survivor.getX() - 5;
      ledMatrix.clearDisplay(0);
      if(flash) {
        drawMap(cameraX, cameraY);
      }
      else {
        see(cameraX, cameraY, survivor.getX(), survivor.getY());
      }
    }
    else if(survivor.getY() - cameraY > 5 && cameraY < mapSize - matrixSize) {
      cameraY = survivor.getY() - 5;
      ledMatrix.clearDisplay(0);
      if(flash) {
        drawMap(cameraX, cameraY);
      }
      else {
        see(cameraX, cameraY, survivor.getX(), survivor.getY());
      }
    }
    else if(survivor.getX() - cameraX < 2 && cameraX > 0) {
      cameraX = survivor.getX() - 2;
      ledMatrix.clearDisplay(0);
      if(flash) {
        drawMap(cameraX, cameraY);
      }
      else {
        see(cameraX, cameraY, survivor.getX(), survivor.getY());
      }
    }
    else if(survivor.getY() - cameraY < 2 && cameraY > 0) {
      cameraY = survivor.getY() - 2;
      ledMatrix.clearDisplay(0);
      if(flash) {
        drawMap(cameraX, cameraY);
      }
      else {
        see(cameraX, cameraY, survivor.getX(), survivor.getY());
      } 
    }
    else if(playerMoved) {
      ledMatrix.clearDisplay(0);
      if(flash) {
        drawMap(cameraX, cameraY);
      }
      else {
        see(cameraX, cameraY, survivor.getX(), survivor.getY());
      }
    }

    //flash
    if(buttonPressed) {
      Serial.println(F("FLASH"));
      drawMap(cameraX, cameraY);
      flash = true;
      flashStartTime = millis();
      buttonPressed = false;
    }
    
    if(flash) {
      lcd.setCursor(14, 1);
      lcd.write(byte(2));
      if(millis() - flashStartTime > flashDuration) {
        flash = false;
        ledMatrix.clearDisplay(0);
      }
    }
    else {
      lcd.setCursor(14, 1);
      lcd.print(F(" "));
    }

    // win/lose conditions
    if(survivor.getX() == mapSize - 2 && survivor.getY() == mapSize - 2) {
      if(currentLevel < numLevels - 1) {
        currentScore += levelCompleteBonus * (currentLevel + 1);
        unsigned long timeBonus = millis() - bonusTimeLimit[currentLevel];
        currentScore += max(0, timeBonus);
        currentLevel ++;
        loadLevel(currentLevel);
      }
      else {
        gameStatus = 2;
        gameWon = true;
      }
    }

    if(survivor.getLives() == 0) {
      gameStatus = 2;
      gameWon = false;
    }
    
    // draw player
    survivor.draw(playerBlinkRate);

    // lcd
    lcd.setCursor(0, 0);
    lcd.write(byte(1));
    lcd.setCursor(2, 0);
    lcd.print((millis() - levelStartTime) / 60000);
    lcd.print(F(":"));
    lcd.print(((millis() - levelStartTime) / 1000) % 60 / 10);
    lcd.print(((millis() - levelStartTime) / 1000) % 60 % 10);
    for(int i = 15; i > 15-survivor.getLives(); i--) {
      lcd.setCursor(i, 0);
      lcd.write(byte(0));
    }
    lcd.setCursor(0, 1);
    lcd.print(F("LV "));
    lcd.print(currentLevel + 1);

    if(gameStatus == 2) {
      displayImage(matrixImages[0]);
      if(gameWon) {
        lcd.setCursor(0, 0);
        lcd.print(F(" YOU ESCAPED.    "));
        lcd.setCursor(0, 1);
        currentScore += winBonus;
        lcd.print(F("/// SCORE: "));
        lcd.print(currentScore);
      }
      else {
        lcd.setCursor(0, 0);
        lcd.print(F(" YOU DIED.       "));
        lcd.setCursor(0, 1);
        lcd.print(F("/// SCORE: "));
        lcd.print(currentScore);
      }
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("////////////////"));
      lcd.setCursor(0, 1);
      lcd.print(F("/// SCORE: "));
      lcd.print(currentScore);
    }

  }



  // GAME OVER
  else if(gameStatus == 2) {
    if(buttonPressed) {
      gameStatus = 0;
      buttonPressed = false;
    }
  }

}