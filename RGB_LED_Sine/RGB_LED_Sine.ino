// Cycle RGB LED's using a sinusoidal fade technique
// Switch input to change mode (white / colour cycle)

// Precalculated 360 sinusoid values for PWM signals to LED's.
// This gives a smooth transition between colours
const byte leds[360] = {
  0,   0,   0,   0,   0,   1,   1,   2, 
  2,   3,   4,   5,   6,   7,   8,   9, 
 11,  12,  13,  15,  17,  18,  20,  22, 
 24,  26,  28,  30,  32,  35,  37,  39, 
 42,  44,  47,  49,  52,  55,  58,  60, 
 63,  66,  69,  72,  75,  78,  81,  85, 
 88,  91,  94,  97, 101, 104, 107, 111, 
114, 117, 121, 124, 127, 131, 134, 137, 
141, 144, 147, 150, 154, 157, 160, 163, 
167, 170, 173, 176, 179, 182, 185, 188, 
191, 194, 197, 200, 202, 205, 208, 210, 
213, 215, 217, 220, 222, 224, 226, 229, 
231, 232, 234, 236, 238, 239, 241, 242, 
244, 245, 246, 248, 249, 250, 251, 251, 
252, 253, 253, 254, 254, 255, 255, 255, 
255, 255, 255, 255, 254, 254, 253, 253, 
252, 251, 251, 250, 249, 248, 246, 245, 
244, 242, 241, 239, 238, 236, 234, 232, 
231, 229, 226, 224, 222, 220, 217, 215, 
213, 210, 208, 205, 202, 200, 197, 194, 
191, 188, 185, 182, 179, 176, 173, 170, 
167, 163, 160, 157, 154, 150, 147, 144, 
141, 137, 134, 131, 127, 124, 121, 117, 
114, 111, 107, 104, 101,  97,  94,  91, 
 88,  85,  81,  78,  75,  72,  69,  66, 
 63,  60,  58,  55,  52,  49,  47,  44, 
 42,  39,  37,  35,  32,  30,  28,  26, 
 24,  22,  20,  18,  17,  15,  13,  12, 
 11,   9,   8,   7,   6,   5,   4,   3, 
  2,   2,   1,   1,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0, 
  0,   0,   0,   0,   0,   0,   0,   0};

// Pins for LED control
const byte redPin = 3;
const byte greenPin = 5;
const byte bluePin = 6;

// Pin for switch input
const byte buttonPin = 2;
volatile byte buttonState = LOW;  // Tell compiler volatile as interrupt driven

// Fade loop delays
unsigned int slowFadeDelay = 250;  // time to spend displaying each colour (in slow fade mode) (ms)
unsigned int fastFadeDelay = 50;  // time to spend displaying each colour (in fast fade mode) (ms)
unsigned int fadeDelay = slowFadeDelay; //start in slow fade mode
unsigned long fadeDelayLastTime = 0;  // Variable to hold last time fadeDelay was passed (ms)
unsigned long fadeDelayCurrent;  // Variable to hold current time within fade loop (ms)

// Variables for switch debounce
unsigned long debounceTimer = 0;  // Current debounce timer value (ms)
unsigned int debounceWindow = 100;  // Debounce window time (ms)
unsigned int longPressWindow = 1000; // Long press window time (ms)
unsigned long buttonPressTime = 0; // Current button press time (ms)

// Position in RGB fade
int k = 0;



// State for RGB mode
boolean rgb = true;

int colourMode = 0; //Colour mode 0 = disabled 1 = Red, 2 = Green, 3 = Blue

//_______________________________
void setup()
{
  Serial.begin(9600);
  // initialize the LED pins as an outputs:
  pinMode(redPin, OUTPUT);  //Red LED
  pinMode(greenPin, OUTPUT);  // Green LED
  pinMode(bluePin, OUTPUT);  // Blue LED

  // initialize the pushbutton pin as an input, with internal pullup enabled:
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Attach an interrupt to the Interrupt Service Routine vector
  attachInterrupt(digitalPinToInterrupt(buttonPin), pin_ISR, CHANGE);
}

//_______________________________
void loop()
{
  if (rgb == true && colourMode == 0) { //Are we in RGB colour cycle mode?
    fadeDelayCurrent = millis() - fadeDelayLastTime;  // How long in current fade loop?
    if (fadeDelayCurrent <= fadeDelay) {  // If less than fadeDelay, write current RGB values to LED's

      // Write LED values
      analogWrite(redPin, leds[(k + 120) % 360]);   // Set 120deg relative phase for red LED
      analogWrite(greenPin, leds[k % 360]);         // No phase shift for green
      analogWrite(bluePin, leds[(k + 240) % 360]);  // Set 240deg relative phase for blue LED
    }

    // If not at end of fade cycle
    else if (k < 360) {
      ++k;  // Increment fade index
      fadeDelayLastTime = millis();  // Reset fade delay last event to current time
    }
    
    // If at end of fade cycle, go back to the beginning
    else if (k = 360) {
      k = 0;  // Reset fade index to zero
      fadeDelayLastTime = millis();  // Reset fade delay last event to current time
    }
  }
  
  else if (rgb != true) {
    // If not in RGB mode (rgb != true), all LED's on (White)
    analogWrite(redPin, 255);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 255);
  }
  
  else {
    if (colourMode == 1 && rgb == true) {
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
    }
    else if (colourMode == 2 && rgb == true) {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
    }
    else if (colourMode == 3 && rgb == true) {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 255);
    }
  }
  
}
// Switch input interrupt service routine
// Reads the state of a switch, with debouncing
void pin_ISR()
{
  buttonState = digitalRead(buttonPin);  // Read button state
  if (buttonState != HIGH) {  // Is button pressed?
    debounceTimer = millis();  // Yes - reset debounce timer
  }
  
  buttonPressTime = millis() - debounceTimer; //How long button pressed?
  
  if (buttonPressTime > debounceWindow && buttonPressTime < longPressWindow) {  // Longer than debounce time, shorter than long press?
    rgb = !rgb;  // Toggle RGB / White mode
  }

  if (buttonPressTime > longPressWindow && colourMode != 3) { // Long press on button?
    ++colourMode; // Move to next colour
  }
  else if (buttonPressTime > longPressWindow && colourMode == 3){
    colourMode = 0;
  }
}
