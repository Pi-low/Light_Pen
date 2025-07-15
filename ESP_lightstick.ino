#include <FastLED.h>
#include "config.h"
#include "utils.h"

CRBG leds[NB_PIXELS];

void setup()
{
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);
  FastLED.addLeds<WS2812, PIN_DATA, GRB>(leds, NB_PIXELS).setCorrection(TypicalLEDStrip);
}

void loop() {
    vUtils_ButtonManager();
}
