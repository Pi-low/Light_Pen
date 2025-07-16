#ifndef _UTILS_H_
#define _UTILS_H_
#include <FastLED.h>
#include "config.h"
#include <stdint.h>

typedef enum {
    eUtils_Falling = 0,
    eUtils_Rising
} TeUtils_Edge;

typedef enum {
    eUtils_Active = 0,
    eUtils_Idle
} TeUtils_BtnState;

typedef void (*pvEdgeCallback)(void);

void vUtils_SetButtonCallback(TeUtils_Edge FeEdge, pvEdgeCallback xCallback);
void vUtils_SetModeCallback(pvEdgeCallback xCallback);
void vUtils_ButtonManager(void);
TeUtils_BtnState eUtils_GetButtonState(void);


#endif //_UTILS_H_
