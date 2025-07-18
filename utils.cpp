/**
 * @brief Tolls and misc
 * @file utils.cpp
 * @version 0.1
 * @date 2025-07-17
 * @author Nello (nello.chom@protonmail.com)
 */

/*********************************************************************************
* Includes
*********************************************************************************/
#include "utils.h"

/*********************************************************************************
* Types & definitions
*********************************************************************************/
typedef struct {
    uint8_t u8CurrState;
    uint8_t u8PrevState;
    uint32_t u32LastEvent;
} TstUtils_pin;

/*********************************************************************************
* Global variables
*********************************************************************************/
static TstUtils_pin stUtils_ButtonPin = {1, 1, 0};
static TstUtils_pin stUtils_ModePin = {1, 1, 0};

static pvEdgeCallback xUtils_RisingCallback = NULL;
static pvEdgeCallback xUtils_FallingCallback = NULL;
static pvEdgeCallback xUtils_CbFallingMode = NULL;
static pvEdgeCallback xUtils_CbRisingMode = NULL;
static pvEdgeCallback xUtils_CbLongPushMode = NULL;

/*********************************************************************************
* Internal functions
*********************************************************************************/

/*********************************************************************************
* External functions
*********************************************************************************/

/**
 * @brief Setup main button callback function
 * 
 * @param FeEdge 
 * @param xCallback 
 */
void vUtils_SetButtonCallback(TeUtils_Edge FeEdge, pvEdgeCallback xCallback)
{
    if (FeEdge == eUtils_Falling)
    { xUtils_FallingCallback = xCallback; }
    else
    { xUtils_RisingCallback = xCallback; }
}

/**
 * @brief Setup mode button callback function
 * 
 * @param FeEdge 
 * @param xCallback 
 */
void vUtils_SetModeCallback(TeUtils_Edge FeEdge, pvEdgeCallback xCallback)
{
    switch(FeEdge)
    {
        case eUtils_Falling:
        xUtils_CbFallingMode = xCallback;
        break;

        case eUtils_Rising:
        xUtils_CbRisingMode = xCallback;
        break;

        case eUtils_Long:
        xUtils_CbLongPushMode = xCallback;
        break;
    }
}

/**
 * @brief Manage buttons interface
 *
 */
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

    // Button
    if (((millis() - stUtils_ButtonPin.u32LastEvent) > TIME_DEBOUNCE) && (u8ReadButton != stUtils_ButtonPin.u8CurrState))
    {
        stUtils_ButtonPin.u8CurrState = u8ReadButton;
        if (!u8ReadButton && (xUtils_FallingCallback != NULL))
        { xUtils_FallingCallback(); }
        else if (u8ReadButton && (xUtils_RisingCallback != NULL))
        { xUtils_RisingCallback(); }
    }

    // Mode selector
    if (((millis() - stUtils_ModePin.u32LastEvent) > TIME_DEBOUNCE) && (u8ReadMode != stUtils_ModePin.u8CurrState))
    {
        stUtils_ModePin.u8CurrState = u8ReadMode;
        if (!u8ReadMode && xUtils_CbFallingMode != NULL)
        { xUtils_CbFallingMode(); }
        if (u8ReadMode && xUtils_CbRisingMode != NULL)
        { xUtils_CbRisingMode(); }
    }
    else if (((millis() - stUtils_ModePin.u32LastEvent) > TIME_LONG_PUSH) && (u8ReadMode == stUtils_ModePin.u8CurrState) && (!u8ReadMode))
    {
        if (xUtils_CbLongPushMode != NULL)
        { xUtils_CbLongPushMode(); }
        stUtils_ModePin.u32LastEvent = millis(); // will loop at [TIME_LONG_PUSH] rate
    }

    stUtils_ButtonPin.u8PrevState = u8ReadButton;
    stUtils_ModePin.u8PrevState = u8ReadMode;
}

/**
 * @brief Get button state
 * 
 * @param FeType 
 * @return TeUtils_BtnState 
 */
TeUtils_BtnState eUtils_GetButtonState(TeUtils_BtnType FeType)
{
    return (FeType == eUtils_Button) ? (TeUtils_BtnState)stUtils_ButtonPin.u8CurrState : (TeUtils_BtnState)stUtils_ModePin.u8CurrState;
}
