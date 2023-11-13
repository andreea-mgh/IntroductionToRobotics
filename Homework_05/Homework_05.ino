// shift register
const int latchPin = 11;  // STCP
const int clockPin = 10;  // SHCP
const int dataPin = 12;   // DS

// 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int digitCount = 4;

const int encodingsNumber = 10;
byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
};



// buttons and controls

const int buttonPause = 2;
bool buttonPausePress = false;
unsigned long pauseLastInterruptTime = 0;

const int buttonLap = 3;
bool buttonLapPress = false;
unsigned long lapLastInterruptTime = 0;

const int buttonReset = 13;
int buttonResetState = HIGH;
int buttonResetReading = HIGH;
int buttonResetLastReading = HIGH;
unsigned long resetLastDebounce = 0;

const unsigned long debounceDelay = 50;

unsigned long currentTime = 0;
unsigned long lastIncrement = 0;

volatile bool pauseMode = true;
volatile bool lapViewMode = false;
volatile bool saveLap = false;
bool resetLaps = false;

const int maxLaps = 4;
int numberOfLaps = 0;
int lapIndex = 0;
int laps[maxLaps];

// indicator led

const int ledPin = 8;



void setup() {
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  for (int i = 0; i < digitCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  for (int i = 0; i < maxLaps; i++) {
    laps[i] = 0;
  }

  pinMode(buttonPause, INPUT_PULLUP);
  pinMode(buttonReset, INPUT_PULLUP);
  pinMode(buttonLap, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPause), togglePause, CHANGE);
  attachInterrupt(digitalPinToInterrupt(buttonLap), toggleLap, CHANGE);

  Serial.begin(9600);
}


void togglePause() {
  static unsigned long pauseInterruptTime = 0;
  pauseInterruptTime = micros();

  // Serial.println(pauseInterruptTime - pauseLastInterruptTime);

  
  if (pauseInterruptTime - pauseLastInterruptTime > (debounceDelay * 1000)) {
    buttonPausePress = !buttonPausePress;
    
    if(buttonPausePress){
      Serial.println("pause");

      if (!lapViewMode) {
        pauseMode = !pauseMode;
      }
    }
  }
  pauseLastInterruptTime = pauseInterruptTime;
}

void toggleLap() {
  // int buttonLapReading = digitalRead(buttonLap);

  static unsigned long lapInterruptTime = 0;
  lapInterruptTime = micros();

  if ((lapInterruptTime - lapLastInterruptTime) > debounceDelay * 1000) {
    buttonLapPress = !buttonLapPress;
    
    if(buttonLapPress) {
      Serial.println("lap");

      if (lapViewMode) {
        // cycle through laps
        lapIndex = (lapIndex + 1) % numberOfLaps;
      }
      else if (!lapViewMode && pauseMode && currentTime == 0) {
        // view laps
        lapViewMode = true;
        lapIndex = 0;
      }
      else if (!pauseMode) {
        // save lap
        saveLap = true;
      }
    }
  }
  lapLastInterruptTime = lapInterruptTime;
}


void loop() {

  // state changes
  if(resetLaps) {
    resetLaps = false;
    for (int i = 0; i < numberOfLaps; i++) {
      laps[i] = 0;
      lapViewMode = false;
      pauseMode = true;
    }
  }

  if(saveLap) {
    saveLap = false;
    if (numberOfLaps < maxLaps) {
      laps[numberOfLaps] = currentTime;
      numberOfLaps++;
    } else {
      for (int i = 0; i < maxLaps - 1; i++) {
        laps[i] = laps[i + 1];
      }
      laps[maxLaps - 1] = currentTime;
    }
  }

  // reset button action
  int buttonResetReading = digitalRead(buttonReset);

  if (buttonResetReading != buttonResetLastReading) {
    resetLastDebounce = millis();
  }

  if ((millis() - resetLastDebounce) > debounceDelay) {
    if (buttonResetReading != buttonResetState) {
      buttonResetState = buttonResetReading;
      if (buttonResetState == HIGH) {
        if (lapViewMode) {
          // reset all laps
          resetLaps = true;
        }
        if (pauseMode) {
          // reset time
          currentTime = 0;
        }
      }
    }
  }

  buttonResetLastReading = buttonResetReading;

  // writing numbers to the display
  if (!pauseMode) {
    digitalWrite(ledPin, HIGH);
    if (millis() - lastIncrement > 100) {
      currentTime++;
      lastIncrement = millis();
    }
  }
  else {
    digitalWrite(ledPin, LOW);
  }

  if (lapViewMode) {
    writeNumber(laps[lapIndex]);
  } else {
    writeNumber(currentTime);
  }

  // Serial.print(laps[0]);
  // Serial.print(" ");
  // Serial.print(laps[1]);
  // Serial.print(" ");
  // Serial.print(laps[2]);
  // Serial.print(" ");
  // Serial.print(laps[3]);
  // Serial.print("\n");
}



void writeReg(int digit) {
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < digitCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(int number) {
  // DONE: Initialize necessary variables for tracking the current number and digit position
  int currentNumber = number;
  int displayDigit = 3;  // Start with the least significant digit
  int lastDigit = 0;
  // DONE: Loop through each digit of the current number
  for (int i = 0; i < digitCount; i++) {
    // DONE: Extract the last digit of the current number
    lastDigit = currentNumber % 10;
    // DONE: Activate the current digit on the display
    activateDisplay(displayDigit);
    // DONE: Output the byte encoding for the last digit to the display
    if (displayDigit == 2) {
      writeReg(byteEncodings[lastDigit] | B00000001);
    } else writeReg(byteEncodings[lastDigit]);
    // DONE: Move to the next digit
    displayDigit--;
    // DONE: Update 'currentNumber' by removing the last digit
    currentNumber /= 10;
    // DONE: Clear the display to prevent ghosting between digit activations
    writeReg(B00000000);  // Clear the register to avoid ghosting
  }
}