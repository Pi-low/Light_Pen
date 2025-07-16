#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>
#include <FastLED.h>
#include "config.h"
#include "utils.h"

extern CRGB leds[];

typedef enum {
    eMenu_Solid = 0,
    eMenu_Fade,
    eMenu_Blink,
    eMenu_Dual,
    eMenu_Dualternate,
} TeMenu_Modes;

static TeMenu_Modes eMenu_CurrentMode = eMenu_Solid;

void vMenu_TellCurrentMode(void)
{
    uint8_t u8Count;
    
}



#endif //_MENU_H_