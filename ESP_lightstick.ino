#include <stdint.h>
#include <FastLED.h>
#include "config.h"
#include "utils.h"

#define REFRESH_TIMEOUT (1000/REFRESH_RATE_HZ)

CRGB leds[NB_PIXELS];
uint8_t u8HueIndex = 0;
CRGB MainColor;
uint32_t u32Timeout = 0;

void vMain_ModeManagerCallback(void);
// void vMain_OnPress(void);
// void vMain_OnRelease(void);

void setup()
{
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_MODE, INPUT_PULLUP);
    FastLED.addLeds<WS2812, PIN_DATA, GRB>(leds, NB_PIXELS).setCorrection(TypicalLEDStrip);
    MainColor = CHSV(u8HueIndex, 255, 255);
    vUtils_SetModeCallback(vMain_ModeManagerCallback);
    // vUtils_SetButtonCallback(eUtils_Falling, vMain_OnPress);
    // vUtils_SetButtonCallback(eUtils_Rising, vMain_OnRelease);
    FastLED.clear();
}

void loop() {
    vUtils_ButtonManager();
    if (millis() > u32Timeout)
    {
        u32Timeout = millis() + REFRESH_TIMEOUT;
        if (eUtils_GetButtonState() == eUtils_Active)
        {
            fill_solid(leds, NB_PIXELS, MainColor);
        }
        else
        {
            fill_solid(leds, NB_PIXELS, CRGB::Black);
        }
        FastLED.show();
    }
}

void vMain_ModeManagerCallback(void)
{
    u8HueIndex+=16;
    MainColor = CHSV(u8HueIndex, 255, 255);
}

// void vMain_OnPress(void)
// {
//     fill_solid(leds, NB_PIXELS, MainColor);
//     FastLED.show();
// }

// void vMain_OnRelease(void)
// {
//     fill_solid(leds, NB_PIXELS, CRGB::Black);
//     FastLED.show();
// }
