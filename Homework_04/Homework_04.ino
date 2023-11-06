// 7 segment display initialization

const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

const int segSize = 8;
int index = segSize - 1;

const bool commonAnode = false;

const int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int segState[segSize];

byte state = HIGH;

const int flickerTime = 500;

// joystick initialization

const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
byte swState = LOW;
byte dpState = LOW;
byte lastSwState = LOW;
int xValue = 0;
int yValue = 0;

bool joyMoved = false;

int minThreshold = 384;
int maxThreshold = 640;

int movementMatrix[segSize][4] = {
  { -1, 6, 5, 1 },
  { 0, 6, 5, -1 },
  { 6, 3, 4, 7 },
  { 6, -1, 4, 2 },
  { 6, 3, -1, 2 },
  { 0, 6, -1, 1 },
  { 0, 3, -1, -1 },
  { -1, -1, 2, -1 }
};

// segment toggle routine

volatile bool buttonDown = false;
volatile bool segToggle = false;
volatile bool reset = false;

volatile unsigned long buttonDownTime = 0;
const unsigned long debounceDelay = 50;
const unsigned long longPressDelay = 1000;

void toggleSegment() {
  segToggle = true;

  // button pressed
  int currentState = digitalRead(pinSW);
  if (currentState == LOW) {
    if (!buttonDown) {
      Serial.println("pressed");
      buttonDown = true;
      buttonDownTime = micros();
    }
  }

  // button released
  else {
    if (buttonDown && micros() - buttonDownTime > debounceDelay * 1000) {
      Serial.println("released");
      buttonDown = false;

      if (micros() - buttonDownTime > longPressDelay * 1000) {
        Serial.println("long press");
        // reset = true;
        for (int i = 0; i < segSize; i++) {
          segState[i] = !state;
        }
      } else {
        Serial.println("toggle segment");
        // segToggle = true;
        segState[index] = !segState[index];
      }
    }
  }
}

void drawDisplay() {
  for (int i = 0; i < segSize; i++) {
    if (i == index) {
      if (millis() % flickerTime < flickerTime / 2) {
        digitalWrite(segments[i], HIGH);
      } else {
        digitalWrite(segments[i], LOW);
      }
    } else {
      digitalWrite(segments[i], segState[i]);
    }
  }
}

int readJoystick() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  // reset movement on resting position
  if (xValue < maxThreshold && xValue > minThreshold && yValue < maxThreshold && yValue > minThreshold) {
    joyMoved = false;
    return -1;
  }

  // joystick hasn't returned to default position
  if (joyMoved) return -1;

  if (yValue < minThreshold && joyMoved == false) {
    // JOYSTICK UP
    joyMoved = true;
    return 0;
  }

  if (yValue > maxThreshold && joyMoved == false) {
    // JOYSTICK DOWN
    joyMoved = true;
    return 1;
  }

  if (xValue < minThreshold && joyMoved == false) {
    // JOYSTICK LEFT
    joyMoved = true;
    return 2;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    // JOYSTICK RIGHT
    joyMoved = true;
    return 3;
  }
}



void setup() {
  if (commonAnode) state = !state;

  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
    segState[i] = !state;
  }

  pinMode(pinSW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinSW), toggleSegment, CHANGE);

  Serial.begin(9600);
}

void loop() {
  // read joystick position
  int position = readJoystick();

  // move current index
  if(position > -1) {
    if (movementMatrix[index][position] != -1) index = movementMatrix[index][position];
  }

  // draw segment state
  drawDisplay();
}
