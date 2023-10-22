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

#define readThreshold 32



void setup() {
  pinMode(redInput,   INPUT);
  pinMode(greenInput, INPUT);
  pinMode(blueInput,  INPUT);

  pinMode(redLED,   OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED,  OUTPUT);
}

void loop() {
  // read values from input pins
  unsigned short redValue   = analogRead(redInput);
  delay(10);
  unsigned short greenValue = analogRead(greenInput);
  delay(10);
  unsigned short blueValue  = analogRead(blueInput);
  delay(10);

  // discard noise in the lower range
  if(redValue   < readThreshold) redValue   = 0;
  if(greenValue < readThreshold) greenValue = 0;
  if(blueValue  < readThreshold) blueValue  = 0;

  // map values and add color correction
  redValue   = map(redValue,   0, AnalogReadResolution, 0, AnalogWriteResolution) * redCC;
  greenValue = map(greenValue, 0, AnalogReadResolution, 0, AnalogWriteResolution) * greenCC;
  blueValue  = map(blueValue,  0, AnalogReadResolution, 0, AnalogWriteResolution) * blueCC;

  // write values to output pins 
  analogWrite(redLED,   redValue);
  analogWrite(greenLED, greenValue);
  analogWrite(blueLED,  blueValue);
}
