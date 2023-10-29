const int floors = 3;
const int buttonPins[floors] = { 2, 4, 6 };
const int LEDPins[floors] = { 3, 5, 7 };
const int buzzerPin = 10;
const int stateLEDPin = 11;

unsigned int currentFloor = 0;
unsigned int targetFloor = 0;
bool elevatorMoving = false;

byte reading[floors];
byte lastReading[floors];
byte buttonState[floors];

unsigned long lastDebounceTime[floors];
unsigned long debounceDelay = 50;
unsigned long lastFloorTime = 0;
const int floorDelay = 1000;



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
          elevatorMoving = true;
          lastFloorTime = millis();
        }
      }
    }
    lastReading[i] = reading[i];
  }


  if (elevatorMoving) {
    if (currentFloor == targetFloor) {
      elevatorMoving = false;
      tone(buzzerPin, 440, 200);
      delay(200);
      tone(buzzerPin, 392, 400);
    }
    
    // move elevator
    else if(millis() - lastFloorTime > floorDelay) {
      lastFloorTime = millis();
      if (currentFloor > targetFloor) currentFloor--;
      else currentFloor++;
    }

    if (millis() % 500 < 250) digitalWrite(stateLEDPin, HIGH);
    else digitalWrite(stateLEDPin, LOW);

  }

  else {
    digitalWrite(stateLEDPin, LOW);
  }

  for(int i=0; i<floors; i++) {
    if(i != currentFloor) digitalWrite(LEDPins[i], LOW);
    else digitalWrite(LEDPins[i], HIGH);
  }

  // buttons pressed deboog
  // for (int i = 0; i < floors; i++) {
  //   Serial.print(buttonState[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();

  Serial.print(currentFloor);
  Serial.println(targetFloor);
}