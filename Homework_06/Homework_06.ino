#include <EEPROM.h>

const int trigPin = 7;
const int echoPin = 8;

const int ldrPin = A0;

const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

int currentIndex = 0;
int maxLoggingEntries = 10;
int currentMenu = 0;
// 0: main menu
// 1: sensor settings
// 2: reset logger data
// 3: System status
// 4: LED settings

int ldrThreshold = 200;
int ultrasonicThreshold = 20;
unsigned long samplingInterval = 2000;
unsigned long lastReadTime = 0;

int manualRedValue = 255;
int manualGreenValue = 255;
int manualBlueValue = 255;

int inputType = 0;
// 0: menu input
// 1: ldr threshold
// 2: ultrasonic threshold
// 3: led red
// 4: led green
// 5: led blue
// 6: sampling interval
int ledAutoMode = true;

long duration = 0;
long distance = 0;
int lightLevel = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  currentIndex = EEPROM.get(maxLoggingEntries * (sizeof(lightLevel) + sizeof(distance)), currentIndex);
  if (currentIndex > 9) {
    Serial.print(F("Error reading last index."));
    currentIndex = 0;
  }

  Serial.begin(9600);
  printMenu();
}

void loop() {
  // PROCESS SERIAL INPUT
  if (Serial.available() > 0) {
    int inputValue = Serial.parseInt();

    if (inputType != 0) {
      // if it's not 0 it's safe
      if (inputValue != 0) getUserConfig(inputValue);
      // if it's 0 check if there's still data in serial (trailing newline)
      // if not then the input itself is a newline and should be ignored
      else if (Serial.available() > 0) getUserConfig(inputValue);
    } else getMenuOption(inputValue);
  }


  // GET SENSOR DATA
  if (millis() - lastReadTime > samplingInterval) {
    lastReadTime = millis();

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);

    distance = duration * 0.034 / 2;
    lightLevel = analogRead(ldrPin);

    EEPROM.put(currentIndex * sizeof(lightLevel), lightLevel);
    EEPROM.put(maxLoggingEntries * sizeof(lightLevel) + currentIndex * sizeof(distance), distance);

    currentIndex++;
    EEPROM.put(maxLoggingEntries * (sizeof(lightLevel) + sizeof(distance)), currentIndex);
    if (currentIndex == maxLoggingEntries) {
      currentIndex = 0;
    }
  }


  // MAKE LED LOOK COOL
  if (ledAutoMode) {
    if (lightLevel < ldrThreshold && distance < ultrasonicThreshold) {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
    } else if (lightLevel >= ldrThreshold && distance >= ultrasonicThreshold) {
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
    } else {
      analogWrite(redPin, 255);
      analogWrite(greenPin, 128);
      analogWrite(bluePin, 0);
    }
  } else {
    analogWrite(redPin, manualRedValue);
    analogWrite(greenPin, manualGreenValue);
    analogWrite(bluePin, manualBlueValue);
  }
}

void clearLogs() {
  for (int i = 0; i < maxLoggingEntries; i++) {
    lightLevel = 0;
    distance = 0;
    EEPROM.put(i * sizeof(lightLevel), lightLevel);
    EEPROM.put(maxLoggingEntries * sizeof(lightLevel) + i * sizeof(distance), distance);

    currentIndex = 0;

    Serial.println(F("All data reset."));
  }
}

void dumpLogs() {
  for (int i = 0; i < maxLoggingEntries; i++) {
    int index = (currentIndex + i) % maxLoggingEntries;
    int currentLightLevel;
    long currentDistance;
    EEPROM.get(index * sizeof(lightLevel), currentLightLevel);
    EEPROM.get(maxLoggingEntries * sizeof(lightLevel) + index * sizeof(distance), currentDistance);

    Serial.print(F("Distance:    "));
    Serial.println(currentDistance);
    Serial.print(F("Light level: "));
    Serial.println(currentLightLevel);
  }
}

void getUserConfig(int value) {
  switch (inputType) {
    case 1:
      ultrasonicThreshold = value;
      inputType = 0;
      break;
    case 2:
      ldrThreshold = value;
      inputType = 0;
      break;
    case 3:
      manualRedValue = value;
      inputType = 4;
      break;
    case 4:
      manualGreenValue = value;
      inputType = 5;
      break;
    case 5:
      manualBlueValue = value;
      inputType = 0;
      break;
    case 6:
      samplingInterval = value;
      inputType = 0;
      break;
  }

  Serial.println(F("Input registered."));
  if (inputType == 0) printMenu();
}

void getMenuOption(int value) {
  if (value != 0) {
    if (currentMenu == 0) {
      if (value > 0 && value <= 4) currentMenu = value;
      else Serial.println(F("INPUT ERROR"));
    }

    else if (currentMenu == 1) {
      switch (value) {
        case 1:
          inputType = 6;
          break;
        case 2:
          inputType = 1;
          break;
        case 3:
          inputType = 2;
          break;
        case 4:
          currentMenu = 0;
          break;
        default:
          Serial.println(F("INPUT ERROR"));
          break;
      }
    }

    else if (currentMenu == 2) {
      switch (value) {
        case 1:
          clearLogs();
          currentMenu = 0;
          break;
        case 2:
          currentMenu = 0;
          break;
        default:
          Serial.println(F("INPUT ERROR"));
          break;
      }
    }

    else if (currentMenu == 3) {
      switch (value) {
        case 1:
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          duration = pulseIn(echoPin, HIGH);
          distance = duration * 0.034 / 2;
          lightLevel = analogRead(ldrPin);
          Serial.print(F("Distance:    "));
          Serial.println(distance);
          Serial.print(F("Light level: "));
          Serial.println(lightLevel);
          break;
        case 2:
          Serial.print(F("Sensor sampling interval:   "));
          Serial.println(samplingInterval);
          Serial.print(F("Ultrasonic alert threshold: "));
          Serial.println(ultrasonicThreshold);
          Serial.print(F("LDR alert threshold:        "));
          Serial.println(ldrThreshold);
          break;
        case 3:
          dumpLogs();
          break;
        case 4:
          currentMenu = 0;
          break;
        default:
          Serial.println(F("INPUT ERROR"));
          break;
      }
    }

    else if (currentMenu == 4) {
      switch (value) {
        case 1:
          inputType = 3;
          break;
        case 2:
          ledAutoMode = !ledAutoMode;
          break;
        case 3:
          currentMenu = 0;
          break;
        default:
          Serial.println(F("INPUT ERROR"));
          break;
      }
    }

    if (inputType == 0) printMenu();
  }
}

void printMenu() {
  switch (currentMenu) {
    case 0:
      Serial.println(F("1. Sensor Settings"));
      Serial.println(F("2. Reset logger data"));
      Serial.println(F("3. System status"));
      Serial.println(F("4. RGB LED Control"));
      break;
    case 1:
      Serial.println(F("  1. Sensors Sampling Interval"));
      Serial.println(F("  2. Ultrasonic Alert Threshold"));
      Serial.println(F("  3. LDR Alert Threshold"));
      Serial.println(F("  4. Back"));
      break;
    case 2:
      Serial.println(F("  Are you sure?"));
      Serial.println(F("  1. Yes"));
      Serial.println(F("  2. No"));
      break;
    case 3:
      Serial.println(F("  1. Current Sensor Readings"));
      Serial.println(F("  2. Current Sensor Settings"));
      Serial.println(F("  3. Display Logged Data"));
      Serial.println(F("  4. Back"));
      break;
    case 4:
      Serial.println(F("  1. Manual Color Control"));
      Serial.println(F("  2. Toggle LED Automatic ON/OFF"));
      Serial.println(F("  3. Back"));
      break;
    default:
      Serial.println(F("MENU ERROR"));
      break;
  }
}
