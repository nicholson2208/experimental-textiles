/*
@author Matt Nicholson, Fall 2023
read a bunch of ADCs, use that to light up an
onboard LED

I could use interrupts, but I will poll instead!

*/

#include <Adafruit_NeoPixel.h>

int NUM_SENSORS = 4;

// select the input pin for the potentiometer
int sensorPins[] = {A0, A1, A2, A3}; 

// I know, I know -- why even define a var
// if you are just going to define by hand anyway
int sensorValues[] = {0, 0, 0, 0};

// because of the construction, we might end up
// with the range of each being limited!
int sensorBaselines[] = {1024, 1024, 1024, 1024};

// the range of each sensor
int sensorRange[] = {1024, 1024, 1024, 1024};

// needed for the on board neopixel!
int Power = 11;
int PIN  = 12;

// variable for the color
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// store the value of each sensor in 0-256
float sensorsToRGB[] = {0.0, 0.0, 0.0, 0.0};

// It hurts my eyes to stare at, make this dimmer
int inverseBrightness = 4;

// some preset colors in case I want that
int myColors[8][3] = {{0, 0, 0}, {0, 0, 255}, {0, 255, 0}, {0, 255, 255}, {255, 0, 0}, {255, 0, 255}, {255, 255, 0}, {255, 255, 255}};

// actually initialize the pixel!
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // So that I can read the values via serial!
  Serial.begin(9600);

  pixels.begin();
  pinMode(Power, OUTPUT);
  digitalWrite(Power, HIGH);

  // this board also has an onboard LED, use that as an output!
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Starting!");

  calibrateSensors();

  Serial.println("Ready to read!!");  
}

void loop() {
  readAllSensors();
  // 3.3 V will be 1024
  // 0 V will be 0

  pixels.clear();
  
  // A0 and A2 are horizontal!

  redValue = (sensorsToRGB[0] + sensorsToRGB[2]) / 2.0;
  if(redValue < 0) {
    redValue = 0;
  } else if (redValue > 1024) {
    redValue = 1024;
  }

  // A1 is left -- Green!
  greenValue = (sensorsToRGB[1]);

  if(greenValue < 0) {
    greenValue = 0;
  } else if (greenValue > 1024) {
    greenValue = 1024;
  }

  // A3 is right -- blue
  blueValue = (sensorsToRGB[3]);

  if(blueValue < 0) {
    blueValue = 0;
  } else if (blueValue > 1024) {
    blueValue = 1024;
  }

  pixels.setPixelColor(0, pixels.Color(redValue/inverseBrightness, greenValue/inverseBrightness, blueValue/inverseBrightness));
  delay(400);
  pixels.show();

  // blink the onboard LED so I can see that it is working
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500); 
  digitalWrite(LED_BUILTIN, LOW);
  
}

void calibrateSensors() {
  Serial.println("Calibrating Sensors!");

  // the number of things to read to average each thing
  // 20 times guarantees that the number is still under the overflow limit
  int numReads = 20;

  for (int i = 0; i < NUM_SENSORS; i++){
    // calibrate each sensor

    int thisSensorSum = 0;

    for (int j = 0; j < numReads; j++){
      thisSensorSum += analogRead(sensorPins[i]);

      // don't go too fast for my wee computer brain
      delay(75);
    }

    int thisBaseline = thisSensorSum / numReads;

    sensorBaselines[i] = thisBaseline > 1024 ? 1024 : thisBaseline;
    sensorRange[i] = 1024 - sensorBaselines[i];

    delay(75);
  } 
  // blink the onboard LED a bunch!
  for (int i = 0; i < 5; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50); 
    digitalWrite(LED_BUILTIN, LOW);

  } 
}

void readAllSensors() {
  Serial.println("Readings Sensors!");

  for (int i = 0; i < NUM_SENSORS; i++){
    sensorValues[i] = analogRead(sensorPins[i]);

    delay(25);

    // scale these readings to 0-255 from the baseline to 1024
    // float thisSensor = (((float) (sensorValues[0] - sensorBaselines[0])) / ((float) sensorRange[0])) * 256.0; 
    sensorsToRGB[i] = (((float) (sensorValues[i] - sensorBaselines[i])) / ((float) sensorRange[i])) * 256.0; 

    Serial.print(i);
    Serial.print(":\t\t");
    Serial.print(sensorValues[i]);
    Serial.print("\t");
    Serial.println(sensorsToRGB[i]);
  } 

  // blink an in a dot dash pattern
  for (int i = 0; i < 2; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50); 
    digitalWrite(LED_BUILTIN, LOW);
    delay(50); 
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100); 
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  } 
}
