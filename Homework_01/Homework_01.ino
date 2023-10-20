#define redInput   A0
#define greenInput A1
#define blueInput  A2

#define redLED   11
#define greenLED 10
#define blueLED  9

#define redCC   1
#define greenCC 0.2
#define blueCC  0.3

#define AnalogReadResolution  1023
#define AnalogWriteResolution 255

const unsigned short sampleRate = 10;



void setup() {
  pinMode(redInput,   INPUT);
  pinMode(greenInput, INPUT);
  pinMode(blueInput,  INPUT);

  pinMode(redLED,   OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED,  OUTPUT);

  // Serial.begin(9600);
}

void loop() {
  unsigned short redValue   = 0;
  unsigned short greenValue = 0;
  unsigned short blueValue  = 0;

  for(int i=0; i<sampleRate; i++) {
    redValue   += analogRead(redInput);
    greenValue += analogRead(greenInput);
    blueValue  += analogRead(blueInput);
  }

  redValue   /= sampleRate;
  greenValue /= sampleRate;
  blueValue  /= sampleRate;

  // Serial.print(redValue);
  // Serial.print(" ");
  // Serial.print(greenValue);
  // Serial.print(" ");
  // Serial.println(blueValue);

  redValue =   map(redValue,   0, AnalogReadResolution, 0, AnalogWriteResolution) * redCC;
  greenValue = map(greenValue, 0, AnalogReadResolution, 0, AnalogWriteResolution) * greenCC;
  blueValue =  map(blueValue,  0, AnalogReadResolution, 0, AnalogWriteResolution) * blueCC;

  analogWrite(redLED, redValue);
  analogWrite(greenLED, greenValue);
  analogWrite(blueLED, blueValue);
}
