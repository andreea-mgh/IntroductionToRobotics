const int floors = 3;
const int buttonPins[floors] = { 2, 4, 6 };
const int LEDPins[floors] = { 3, 5, 7 };
const int buzzerPin = 11;
const int stateLEDPin = 10;

unsigned int currentFloor = 0;
unsigned int targetFloor = 0;
bool elevatorMoving = false;
bool doorsClosing = false;

// button debouncing
byte reading[floors];
byte lastReading[floors];
byte buttonState[floors];
unsigned long lastDebounceTime[floors];
unsigned long debounceDelay = 50;

// timing
unsigned long lastFloorTime = 0;
unsigned long doorCloseTime = 0;

// simulation parameters
const int blinkTime = 500;
const int floorDelay = 2500;
const int doorDelay = 500;

const int moveTone = 96;
const int openTone = 440;
const int closeTone = 220;


void setup() {
  for (int i = 0; i < floors; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(LEDPins[i], OUTPUT);
    reading[i] = HIGH;
    lastReading[i] = HIGH;
    buttonState[i] = HIGH;

    lastDebounceTime[i] = 0;
  }

  Serial.begin(9600);
}


void loop() {
  // start reading buttons and debounce
  for (int i = 0; i < floors; i++) {
    reading[i] = digitalRead(buttonPins[i]);

    if (reading[i] != lastReading[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading[i] != buttonState[i]) {
        buttonState[i] = reading[i];
        if (buttonState[i] == HIGH && i != currentFloor) {
          // button i was pressed
          targetFloor = i;
          doorsClosing = true;
          doorCloseTime = millis();
        }
      }
    }
    lastReading[i] = reading[i];

    // update floor LEDs
    if(i != currentFloor) digitalWrite(LEDPins[i], LOW);
    else digitalWrite(LEDPins[i], HIGH);
  }

  // door operation
  if(doorsClosing) {
    tone(buzzerPin, closeTone);
    if(millis() - doorCloseTime > doorDelay) {
      doorsClosing = false;
      elevatorMoving = true;
      lastFloorTime = millis();
      // tone(buzzerPin, moveTone);
    }
    
  }

  // elevator operation
  if (elevatorMoving) {
    tone(buzzerPin, moveTone);

    if (currentFloor == targetFloor) {
      elevatorMoving = false;
      tone(buzzerPin, openTone, 200);
    }
    
    // move elevator
    else if(millis() - lastFloorTime > floorDelay) {
      lastFloorTime = millis();
      if (currentFloor > targetFloor) currentFloor--;
      else currentFloor++;
    }

    // blink state LED
    if (millis() % blinkTime < blinkTime/2) digitalWrite(stateLEDPin, HIGH);
    else digitalWrite(stateLEDPin, LOW);
  }

  // elevator idle
  else {
    digitalWrite(stateLEDPin, LOW);
  }
}