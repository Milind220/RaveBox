/* Code for RaveBox v1.0, written by Milind Sharma. */

// TODO: Add pin for LBO indicator

// BASE DEFINITIONS
#include <FastLED.h>

// BASE MACRO DEFINITIONS
#define LED_PIN 9
#define L_BUTTON_PIN 2
#define R_BUTTON_PIN 3
#define LED_DATA_PIN 9
#define L_KNOB_PIN A4
#define R_KNOB_PIN A5
#define NUM_MODES 5        // Remember to increment this if you add more modes!! 

// FAST LED BASE STUFF
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS 32

// height and width parameters
const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;

// parameters for different pixel layouts
const bool kMatrixSerpentineLayout = true;
const bool kMatrixVertical = true;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds(leds_plus_safety_pixel + 1);

// Base Globals
int RKnobValue;
int LKnobValue;

int8_t mode = 0;
int lButtonState;
int lButtonLastState = HIGH;
int rButtonState;
int rButtonLastState = HIGH; 
unsigned long lButtonLastDebounceTime = 0;
unsigned long rButtonLastDebounceTime = 0;
unsigned long debounceDelay = 50;
uint8_t modeFrameCounter = 0;    // Used to generate different frames in modes

// Timing related things
unsigned long knobReadDelay = 5;
unsigned long lastKnobReadTime = 0;

// Function definitions
void pOutsideIn(int speed, int color, uint8_t counter);    // p prefix means that it's a pattern

void setup() {
  Serial.begin(9600);

  pinMode(L_BUTTON_PIN, INPUT);
  pinMode(R_BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );

  // Get initial readings of knobs so first iteration of patterns has values to work with
  LKnobValue = analogRead(L_KNOB_PIN);
  RKnobValue = analogRead(R_KNOB_PIN);
}

void loop() {
  // non blocking delays are used frequently for timing
  if ((millis() - lastKnobReadTime) >= knobReadDelay) {
    LKnobValue = analogRead(L_KNOB_PIN);
    RKnobValue = analogRead(R_KNOB_PIN);
  }
  // read buttons
  int lButtonReading = digitalRead(L_BUTTON_PIN);   
  int rButtonReading = digitalRead(R_BUTTON_PIN);
  // debounce buttons
  if (lButtonReading != lButtonLastState) lButtonLastDebounceTime = millis();
  if (rButtonReading != rButtonLastState) rButtonLastDebounceTime = millis();
  if ((millis() - lButtonLastDebounceTime) >= debounceDelay) {
    if (lButtonReading != lButtonState) {
      lButtonState = lButtonReading;
      if (lButtonState == LOW) {
        mode--; 
        if (mode < 0) mode = NUM_MODES - 1;    // Wraps to the highest side if goes below 0; 
      }
    }
  }
  if ((millis() - rButtonLastDebounceTime) >= debounceDelay) {
    if (rButtonReading != rButtonState) {
      rButtonState = rButtonReading;
      if (rButtonState == LOW) {
        mode ++;
        mode = mode % NUM_MODES;    // This wraps mode, so after highest mode it returns to 0;
      }
    }
  }

  switch (mode) {
    case 0:
      pOutsideIn( , RKnobValue, counter);
      break;
  } 
  // Part of the button debounce logic
  lButtonLastState = lButtonReading;
  rButtonLastState = rButtonReading;
}

// pattern functions 
void pOutsideIn(int speed, int color, uint8_t counter) {
  FastLED.clear();
  hue = map(color, 0, 1024, 0, 256 );
  CRGB color = CHSV( hue, 200, 200);
  if (counter == 0) {
    leds[XY(0,0)] = color;
    leds[XY(0,1)] = color;
    leds[XY(0,2)] = color;
    leds[XY(0,3)] = color;
    leds[XY(0,4)] = color;
    leds[XY(0,5)] = color;
    leds[XY(0,6)] = color;
    leds[XY(0,7)] = color;
    leds[XY(1,7)] = color;
    leds[XY(2,7)] = color;
    leds[XY(3,7)] = color;
    leds[XY(4,7)] = color;
    leds[XY(5,7)] = color;
    leds[XY(6,7)] = color;
    leds[XY(1,0)] = color;
    leds[XY(2,0)] = color;
    leds[XY(3,0)] = color;
    leds[XY(4,0)] = color;
    leds[XY(5,0)] = color;
    leds[XY(6,0)] = color;
    leds[XY(7,0)] = color;
    leds[XY(7,1)] = color;
    leds[XY(7,2)] = color;
    leds[XY(7,3)] = color;
    leds[XY(7,4)] = color;
    leds[XY(7,5)] = color;
    leds[XY(7,6)] = color;
    leds[XY(7,7)] = color;
  }
  else if (counter == 1) {
    leds[XY(1,1)] = color;
    leds[XY(1,2)] = color;
    leds[XY(1,3)] = color;
    leds[XY(1,4)] = color;
    leds[XY(1,5)] = color;
    leds[XY(1,6)] = color;
    leds[XY(2,1)] = color;
    leds[XY(2,6)] = color;
    leds[XY(3,1)] = color;
    leds[XY(3,6)] = color;
    leds[XY(4,1)] = color;
    leds[XY(4,6)] = color;
    leds[XY(5,1)] = color;
    leds[XY(5,6)] = color;
    leds[XY(6,1)] = color;
    leds[XY(6,2)] = color;
    leds[XY(6,3)] = color;
    leds[XY(6,4)] = color;
    leds[XY(6,5)] = color;
    leds[XY(6,6)] = color;
  }
  else if (counter == 2) {
    leds[XY(2,2)] = color;
    leds[XY(2,3)] = color;
    leds[XY(2,4)] = color;
    leds[XY(2,5)] = color;

    leds[XY(3,2)] = color;
    leds[XY(3,5)] = color;
    leds[XY(4,2)] = color;
    leds[XY(4,5)] = color;
    
    leds[XY(5,2)] = color;
    leds[XY(5,3)] = color;
    leds[XY(5,4)] = color;
    leds[XY(5,5)] = color;
  }
  else {
    leds[XY(3,3)] = color;
    leds[XY(3,4)] = color;
    leds[XY(4,3)] = color;
    leds[XY(4,4)] = color;
  }
  FastLed.show();    // TODO: Maybe move this and FastLED.clear() out of the function and into the main loop for less repetition.
}

// helper functions

