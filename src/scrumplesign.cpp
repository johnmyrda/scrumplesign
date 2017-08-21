#include "FastLED.h"
#include "FastLED_RGBW.h"

#include "PatternController.h"
#include "patterns/ColorBars.h"
#include "patterns/Generic.h"
#include "patterns/Gradient.h"
#include "patterns/LarsonScanner.h"
#include "patterns/BeaconPulse.h"
#include "patterns/BeaconPulse2.h"
#include "patterns/BeaconScanner.h"
#include "patterns/Rainbow.h"
#include "patterns/RandomColors.h"
#include "patterns/Sos.h"
#include "patterns/Usa.h"
#include "patterns/Lightning.h"

#include <TimerOne.h>

#define NUM_LEDS 142
#define LED_PIN 2
#define BUTTON_PIN 3

CRGBW leds[NUM_LEDS];
CRGB  dummy_leds[NUM_LEDS];
CRGB  white = CRGB::White;
CRGB *ledsRGB = (CRGB *) &leds[0];
PatternController pattern_master = PatternController(dummy_leds, NUM_LEDS);

// These global values keep track of the "brightness" setting controlled by the
// onboard button.
int brightnesses[] = {7, 20, 35, 60, 80, 120, 255};
int num_brightnesses = sizeof(brightnesses) / sizeof(brightnesses[0]);
int brightness = num_brightnesses / 2;
int last_button_position = false;

// This function will be called periodically to update the button state and
// brightness levels.
void updateBrightness() {
  bool button_down = (bool)digitalRead(BUTTON_PIN);
  if (!button_down && last_button_position) {
    brightness = (brightness + 1) % num_brightnesses;
    FastLED.setBrightness(brightnesses[brightness]);
  }
  last_button_position = button_down;
}

void copy_dummy_leds(){
  for (uint8_t x = 0; x < NUM_LEDS; x++) {
    CRGB cur = dummy_leds[x];
    if(cur.r == cur.g && cur.g == cur.b){
      leds[x] = CRGBW(0,0,0,cur.r);
    } else {
      leds[x] = dummy_leds[x];
    }
  }
}

void setup() {
  // Set up the brightness button as an input
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Set up timer to run the brightness button checks periodically
  Timer1.initialize(100000);  // Period in Microseconds = 0.1 seconds
  Timer1.attachInterrupt(updateBrightness);


  pattern_master.add(new Rainbow(-4));
  Pattern * larson = new LarsonScanner();
  larson->set_fps(50);
  pattern_master.add(larson);
  pattern_master.add(new ColorBars(40));
  pattern_master.add(new Rainbow(1));


  Lightning lightning = Lightning(CRGB::White, 10);
  lightning.set_fps(35);
  pattern_master.add(&lightning);

  pattern_master.add(new BeaconScanner());
  BeaconPulse2 bp = BeaconPulse2(CRGB::White);
  bp.set_fps(35);
  pattern_master.add(&bp);

  pattern_master.set_fps(20);
  pattern_master.pattern_duration = 60000;

  // Initialize the LED strip
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS)).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightnesses[brightness]);
}

void loop() {
  pattern_master.show();
  copy_dummy_leds();
  FastLED.show();
}
