#include <FastLED.h>

/* Code for RaveBox v1.0, written by Milind Sharma. */

// TODO: Add pin for LBO indicator

// BASE MACRO DEFINITIONS
#define LED_PIN 9
#define L_BUTTON_PIN 2
#define R_BUTTON_PIN 3
#define LED_DATA_PIN 9
#define L_KNOB_PIN A4
#define R_KNOB_PIN A5
#define NUM_MODES 5        // Remember to increment this if you add more modes!! 
#define NUM_PATTERNS 5

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
int timeInterval;

int8_t mode = 0;
uint8_t pattern =0;
unsigned long stateTime = 0;

int lButtonState;
int lButtonLastState = HIGH;
int rButtonState;
int rButtonLastState = HIGH; 
unsigned long lButtonLastDebounceTime = 0;
unsigned long rButtonLastDebounceTime = 0;
unsigned long lButtonLastClickTime = 0;
unsigned long rButtonLastClickTime = 0;
unsigned long debounceDelay = 50;
uint8_t modeFrameCounter = 0;    // Used to generate different frames in modes
uint8_t counter = 0;
bool lDoubleClick = false;
bool rDoubleClick = false;

// Timing related things
unsigned long knobReadDelay = 5;
unsigned long lastKnobReadTime = 0;

// Function definitions
// void pOutsideIn(int interval, int color, uint8_t counter);    // p prefix means that it's a pattern

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

void readinputs(){
  if ((millis() - lastKnobReadTime) >= knobReadDelay) {
    LKnobValue = analogRead(L_KNOB_PIN);
    RKnobValue = analogRead(R_KNOB_PIN);     // You can use this for whatever.
    timeInterval = map(LKnobValue, 0, 1024, 1, 1000);
  }
  //Serial.print("knob left: ");
  //Serial.print(LKnobValue);
  //Serial.print(" R knob: ");
  //Serial.print(RKnobValue);
  //Serial.print(" mode: ");
  //Serial.println(mode);
  //Serial.print(" pattern: ");
  //Serial.println(pattern);
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
        if (millis()-lButtonLastClickTime<500 && lDoubleClick){
          pattern--;
          lDoubleClick=false;
          mode=0;

          if (pattern < 0) pattern = NUM_PATTERNS - 1;
        }else{
          mode--;
        }

        if (!lDoubleClick){
          lButtonLastClickTime=millis();
          lDoubleClick=true;
        }
         
       
        if (mode < 0) mode = NUM_MODES - 1;    // Wraps to the highest side if goes below 0; 
      }
    }
  }
  if ((millis() - rButtonLastDebounceTime) >= debounceDelay) {
    if (rButtonReading != rButtonState) {
      rButtonState = rButtonReading;
      if (rButtonState == LOW) {
        if (millis()-rButtonLastClickTime<500 && rDoubleClick){
          mode=0;
          pattern++;
          rDoubleClick=false;
          pattern = pattern %NUM_PATTERNS;
        }else{
          mode++;
        }
        
        if (!rDoubleClick){
          rButtonLastClickTime=millis();
          rDoubleClick=true;
        }
        mode = mode % NUM_MODES;    // This wraps mode, so after highest mode it returns to 0;
      }
    }
  }

  if (millis()-lButtonLastClickTime>500){
    lDoubleClick=false;
  }
  if (millis()-rButtonLastClickTime>500){
    rDoubleClick=false;
  }

  lButtonLastState = lButtonReading;
  rButtonLastState = rButtonReading;
}



void loop() {
  // non blocking delays are used frequently for timing
  readinputs();
  switch (pattern) {
    case 0:
      //Serial.println("pattern 1 go brrr!");
      concentricRingPattern(LKnobValue, RKnobValue);
      break;
    case 1:
      LinesPattern(LKnobValue, RKnobValue);
      break;
    case 2:
      SpiralPattern(LKnobValue, RKnobValue);
      break;
  } 
}

void SpiralPattern(int interval,int hueValue) {
  if (mode==0){
    if(millis()-stateTime > interval){
      stateTime= millis();
      counter++;
      //FastLED.clear();
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawSpiral(counter, color);
      FastLED.show();
    }
    if (counter> 64){
      counter=0;
      FastLED.clear();
    }
  }if (mode==1){
    if(millis()-stateTime > interval){
      stateTime= millis();
      counter++;
      FastLED.clear();
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawSpiral(counter, color);
      FastLED.show();
    }
    if (counter> 64){
      counter=0;
      FastLED.clear();
    }
  }
}

void drawSpiral(int count, CRGB color) {
  if(count < 7){
    leds[XY(7-count, 0)]=color;
  }else if (count<14){
    leds[XY(0, count-7)]=color;
  }else if (count<21){
    leds[XY(count-14, kMatrixHeight-1)]=color;
  }else if (count<28){
    leds[XY(kMatrixWidth-1, 28-count)]=color;
  }
  
  else if(count < 34){
    leds[XY(34-count, 1)]=color;
  }else if(count < 39){
    leds[XY(1, count-32)]=color;
  }else if(count < 44){
    leds[XY(count-37, kMatrixHeight-2)]=color;
  }else if (count<48){
    leds[XY(kMatrixWidth-2, 49-count)]=color;
  }

  else if(count < 52){
    leds[XY(53-count, 2)]=color;
  }else if(count < 55){
    leds[XY(2, count-49)]=color;
  }else if(count < 58){
    leds[XY(count-52, kMatrixHeight-3)]=color;
  }else if (count<60){
    leds[XY(kMatrixWidth-3, 62-count)]=color;
  }

  else if(count < 62){
    leds[XY(64-count, 3)]=color;
  }else if(count < 64){
    leds[XY(count-59, 4)]=color;
  }
}


void LinesPattern(int interval,int hueValue) {
  if (mode==0){
    if(millis()-stateTime > interval){
      stateTime= millis();
      FastLED.clear();
      //drawRing(counter, CRGB::Black);
      counter++;
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawColumn(counter, color);
      FastLED.show();
    }
    if (counter>kMatrixWidth-1){
      counter=0;
    }
  }else if(mode==1){
    if(millis()-stateTime > interval){
      stateTime= millis();
      FastLED.clear();
      //drawRing(counter, CRGB::Black);
      counter++;
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawColumn(counter, color);
      drawColumn(7-counter, 255-color);
      FastLED.show();
    }
    if (counter>kMatrixWidth-1){
      counter=0;
    }
  }else if(mode==2){
    if(millis()-stateTime > interval){
      stateTime= millis();
      FastLED.clear();
      //drawRing(counter, CRGB::Black);
      counter++;
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawColumn(counter, color);
      drawRow(counter, color);
      drawColumn(7-counter, 255-color);
      FastLED.show();
    }
    if (counter>kMatrixWidth-1){
      counter=0;
    }
  }else if(mode==3){
    if(millis()-stateTime > interval){
      stateTime= millis();
      FastLED.clear();
      //drawRing(counter, CRGB::Black);
      counter++;
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawColumn(counter, color);
      drawRow(counter, 255-color);
      drawRow(7-counter, 255-color);
      drawColumn(7-counter, color);
      FastLED.show();
    }
    if (counter>kMatrixWidth-1){
      counter=0;
    }
  }
}


void drawColumn(int col, CRGB color) {
  for (int y = 0; y < kMatrixHeight; ++y) {
    leds[XY(col, y)] = color;
  }
}

void drawRow(int col, CRGB color) {
  for (int y = 0; y < kMatrixWidth; ++y) {
    leds[XY(y, col)] = color;
  }
}


void concentricRingPattern(int interval,int hueValue) {
  if (mode==0){
    if(millis()-stateTime > interval){
      stateTime= millis();
      drawRing(counter, CRGB::Black);
      counter++;
    }else{
      int hue = map(RKnobValue, 0, 1024, 0, 256 );
      CRGB color = CHSV(hue, 255, 255);
      drawRing(counter, color);
      FastLED.show();
    }
    if (counter>min(kMatrixWidth, kMatrixHeight) / 2){
      counter=0;
    }
  }else if (mode==1){
    static bool expanding = true;  // Flag to control contraction or expansion
  
    if (millis() - stateTime > interval) {
      stateTime = millis();
  
      // Turn off LEDs in the current ring
      drawRing(counter, CRGB::Black);
  
      if (expanding) {
        counter++;
        if (counter >= min(kMatrixWidth, kMatrixHeight) / 2) {
          // If fully expanded, switch to contraction
          expanding = false;
        }
      } else {
        counter--;
        if (counter <= 0) {
          // If fully contracted, switch to expansion
          expanding = true;
        }
      }
    } else {
      int hue = map(hueValue, 0, 1024, 0, 256);
      CRGB color = CHSV(hue, 255, 255);
  
      // Turn on LEDs in the current ring
      drawRing(counter, color);
      FastLED.show();
    }
  }
}


void drawRing(int ring, CRGB color) {
  int startX = ring;
  int startY = ring;
  int endX = kMatrixWidth - ring - 1;
  int endY = kMatrixHeight - ring - 1;

  // Draw top and bottom rows
  for (int x = startX; x <= endX; ++x) {
    leds[XY(x, startY)] = color;
    leds[XY(x, endY)] = color;
  }

  // Draw left and right columns
  for (int y = startY + 1; y < endY; ++y) {
    leds[XY(startX, y)] = color;
    leds[XY(endX, y)] = color;
  }
}

//int XY(int x, int y) {
  //return y * kMatrixWidth + x;
//}

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * kMatrixWidth) + x;
    } else {
      i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
      } else {
        i = kMatrixHeight * (kMatrixWidth - x) - (y+1);
      }
    }
  }
  
  return i;
}

