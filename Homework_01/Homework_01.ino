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



void setup() {
  pinMode(redInput,   INPUT);
  pinMode(greenInput, INPUT);
  pinMode(blueInput,  INPUT);

  pinMode(redLED,   OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED,  OUTPUT);

  Serial.begin(9600);
}

void loop() {
  unsigned short redValue   = analogRead(redInput);
  delay(10);
  unsigned short greenValue = analogRead(greenInput);
  delay(10);
  unsigned short blueValue  = analogRead(blueInput);
  delay(10);

  if(redValue   < 32) redValue   = 0;
  if(greenValue < 32) greenValue = 0;
  if(blueValue  < 32) blueValue  = 0;

  // Serial.print(map(redValue,   0, AnalogReadResolution, 0, AnalogWriteResolution));
  // Serial.print(" ");
  // Serial.print(map(greenValue, 0, AnalogReadResolution, 0, AnalogWriteResolution));
  // Serial.print(" ");
  // Serial.println(map(blueValue,  0, AnalogReadResolution, 0, AnalogWriteResolution));

  redValue   = map(redValue,   0, AnalogReadResolution, 0, AnalogWriteResolution) * redCC;
  greenValue = map(greenValue, 0, AnalogReadResolution, 0, AnalogWriteResolution) * greenCC;
  blueValue  = map(blueValue,  0, AnalogReadResolution, 0, AnalogWriteResolution) * blueCC;



  analogWrite(redLED,   redValue);
  analogWrite(greenLED, greenValue);
  analogWrite(blueLED,  blueValue);
}
