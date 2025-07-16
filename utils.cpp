#include "utils_int.h"
#include "utils.h"
#include "config.h"

static TstUtils_pin stUtils_ButtonPin = {1, 1, 0};
static TstUtils_pin stUtils_ModePin = {1, 1, 0};

static pvEdgeCallback xUtils_RisingCallback = NULL;
static pvEdgeCallback xUtils_FallingCallback = NULL;
static pvEdgeCallback xUtils_CbFallingMode = NULL;

void vUtils_SetButtonCallback(TeUtils_Edge FeEdge, pvEdgeCallback xCallback)
{
    if (FeEdge == eUtils_Falling)
    { xUtils_FallingCallback = xCallback; }
    else
    { xUtils_RisingCallback = xCallback; }
}

void vUtils_SetModeCallback(pvEdgeCallback xCallback)
{
    xUtils_CbFallingMode = xCallback;
}

void vUtils_ButtonManager(void)
{
    uint8_t u8ReadButton = digitalRead(PIN_BUTTON);
    uint8_t u8ReadMode = digitalRead(PIN_MODE);

    if (u8ReadButton != stUtils_ButtonPin.u8PrevState)
    {
        stUtils_ButtonPin.u32LastEvent = millis();
    }
    if(u8ReadMode != stUtils_ModePin.u8PrevState)
    {
        stUtils_ModePin.u32LastEvent = millis();
    }

    if (((millis() - stUtils_ButtonPin.u32LastEvent) > TIME_DEBOUNCE) && (u8ReadButton != stUtils_ButtonPin.u8CurrState))
    {
        stUtils_ButtonPin.u8CurrState = u8ReadButton;
        if (!u8ReadButton && (xUtils_FallingCallback != NULL))
        { xUtils_FallingCallback(); }
        else if (u8ReadButton && (xUtils_RisingCallback != NULL))
        { xUtils_RisingCallback(); }
    }

    if (((millis() - stUtils_ModePin.u32LastEvent) > TIME_DEBOUNCE) && (u8ReadMode != stUtils_ModePin.u8CurrState))
    {
        stUtils_ModePin.u8CurrState = u8ReadMode;
        if (!u8ReadMode && xUtils_CbFallingMode != NULL)
        { xUtils_CbFallingMode(); }
    }

    stUtils_ButtonPin.u8PrevState = u8ReadButton;
    stUtils_ModePin.u8PrevState = u8ReadMode;
}

TeUtils_BtnState eUtils_GetButtonState(void)
{
    return (TeUtils_BtnState)(stUtils_ButtonPin.u8CurrState);
}