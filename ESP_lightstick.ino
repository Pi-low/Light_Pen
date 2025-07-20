/**
 * @brief Main
 * @file ESP_lightstick.ino
 * @version 0.1
 * @date 2025-07-17
 * @author Nello (nello.chom@protonmail.com)
 */

/*********************************************************************************
* Includes
*********************************************************************************/
#include <FastLED.h>
#include "config.h"
#include "utils.h"
#include "AnimMng.h"

CRGB MainLedStip[NB_PIXELS];
const char* ssid = MY_WIFI_SSID;
const char* password = MY_WIFI_PWD;
uint32_t u32Timeout = 0;

void setup()
{
    WiFi.begin(ssid, password);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_MODE, INPUT_PULLUP);
    FastLED.addLeds<WS2812, PIN_DATA, GRB>(MainLedStip, NB_PIXELS).setCorrection(TypicalLEDStrip);
#if (DEVICE_MODE == DEVICE_SIMPLE)
    FastLED.setBrightness(255);
#else
    FastLED.setBrightness(180);
#endif
    vAnim_Init();
    FastLED.clear();
    FastLED.show();
}

void loop() {
    vUtils_ButtonManager();
    vAnim_CoreMng(MainLedStip);
}
