/**
 * @brief Manage animation
 * @file AnimMng.c
 * @version 0.1
 * @date 2025-07-17
 * @author Nello (nello.chom@protonmail.com)
 */

/*********************************************************************************
* Includes
*********************************************************************************/
#include <FastLED.h>
#include "AnimMng.h"

/*********************************************************************************
* Types & definitions
*********************************************************************************/
#define REFRESH_TIMEOUT (1000/REFRESH_RATE_HZ)
#define TIME_CLICK_DURATION_MIN     0
#define TIME_CLICK_DURATION_MAX     500
#define TIME_MENU_BLINK_LOOP        1000
#define TIME_FADE_CONFIG_LOOP       2000

typedef struct {
    uint8_t u8MainColorIndex;
    uint8_t u8SecColorIndex;
    uint8_t u8BlinkRateIndex;     //< refers to index ctu16BlinkRates
    uint8_t u8FadeRateIndex;      //< refers to index ctu16FadeRates
    TeAnim_RunMode eMode;
    uint8_t u8SubMenu;
} TstAnim_Configuration;

/*********************************************************************************
* Internal functions prototypes
*********************************************************************************/
static void vAnim_CbClickFall(void);
static void vAnim_CbClickRise(void);
static void vAnim_CbLongClick(void);
static void vAnim_OnEntrySelect(CRGB* FpLeds);
static void vAnim_OnExitSelect(CRGB* FpLeds);
static void vAnim_CbClickSubMenu(void);
static void vAnim_MenuBlink(CRGB* FpLeds, CRGB FSetColor, uint8_t Fu8Repeat);
static void vAnim_RunSolid(CRGB* FpLeds);
static void vAnim_RunBlink(CRGB* FpLeds);
static void vAnim_RunFade(CRGB* FpLeds);
static void vAnim_RunAlternate(CRGB* FpLeds);
static void vAnim_ConfigBlink(CRGB* FpLeds);
static void vAnim_ConfigFade(CRGB* FpLeds);

/*********************************************************************************
* Global variables
*********************************************************************************/
extern CRGB MainLedStip[];
static uint32_t u32LastFallingEvent = 0;
static TeAnim_State eAnim_CurrentState = eAnim_StateRun;
static TstAnim_Configuration stAnim_MasterConfig;

static const uint16_t ctu16BlinkRates[3] = {50, 100, 250};
static const uint16_t ctu16FadeRates[3] = {32, 16, 8};
static CRGB ctrgb_Palette[17] = {CRGB::White};

static void (*tpvAnimations[eAnim_NbRun])(CRGB*) = { //animation function pointer array
    vAnim_RunSolid,
    vAnim_RunFade,
    vAnim_RunBlink,
    vAnim_RunAlternate
};


/*********************************************************************************
* External functions
*********************************************************************************/

/**
 * @brief Initialize animation engine
 * 
 */
void vAnim_Init(void)
{
    vUtils_SetModeCallback(eUtils_Falling, vAnim_CbClickFall);
    vUtils_SetModeCallback(eUtils_Rising, vAnim_CbClickRise);
    vUtils_SetModeCallback(eUtils_Long, vAnim_CbLongClick);
    vUtils_SetButtonCallback(eUtils_Falling, vAnim_CbClickSubMenu);
    for (uint8_t u8Index = 0; u8Index < 16; u8Index++)
    {
        ctrgb_Palette[u8Index + 1] = CHSV(u8Index * 16, 255, 255);
    }
    stAnim_MasterConfig.u8MainColorIndex = 0;
    stAnim_MasterConfig.u8SecColorIndex = 1;
    stAnim_MasterConfig.u8BlinkRateIndex = 0;
    stAnim_MasterConfig.u8FadeRateIndex = 0;
    stAnim_MasterConfig.eMode = eAnim_RunSolid;
    stAnim_MasterConfig.u8SubMenu = 0;
}

/**
 * @brief Run animation engine
 * 
 * @param Fptr 
 */
void vAnim_CoreMng(CRGB* Fptr)
{
    switch(eAnim_CurrentState)
    {
        case eAnim_StateRun:
        tpvAnimations[stAnim_MasterConfig.eMode](Fptr);
        break;

        case eAnim_StateSelect:
        vAnim_MenuBlink(Fptr, CRGB::Blue, stAnim_MasterConfig.eMode + 1);
        break;

        case eAnim_eStateSubParam:
        switch(stAnim_MasterConfig.eMode)
        {
            case eAnim_RunBlink:
            vAnim_ConfigBlink(Fptr);
            break;

            case eAnim_RunFade:
            vAnim_ConfigFade(Fptr);
            break;

            case eAnim_RunAlter:
            if(stAnim_MasterConfig.u8SubMenu == 0)
            {
                vAnim_MenuBlink(Fptr, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex], 1);
            }
            else
            {
                vAnim_MenuBlink(Fptr, ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex], 2);
            }
            break;

            default:
            fill_solid(Fptr, NB_PIXELS, CRGB::White);
            FastLED.show();
            delay(1000);
            break;
        }
        break;

        default:
        delay(REFRESH_TIMEOUT);
        break;
    }
}

/**
 * @brief single color fill
 * 
 * @param FpLeds 
 */
void vAnim_RunSolid(CRGB* FpLeds)
{
    static uint32_t u32timeout = 0;
    if (millis() > u32timeout)
    {
        u32timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]);
        }
        FastLED.show();
    }
}

/**
 * @brief Single color blink
 * 
 * @param FpLeds 
 */
void vAnim_RunBlink(CRGB* FpLeds)
{
    static uint8_t su8FlipFlop = 0;
    static uint32_t su32RefreshTimeOut = 0;
    if (millis() > su32RefreshTimeOut)
    {
        su32RefreshTimeOut = millis() + ctu16BlinkRates[stAnim_MasterConfig.u8BlinkRateIndex];
        su8FlipFlop ^= 1;
        FastLED.clear();
        if (su8FlipFlop && (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)) 
        {
            fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]);
        }
        FastLED.show();
    }
}

/**
 * @brief Single color fade in/out
 * 
 * @param FpLeds 
 */
void vAnim_RunFade(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint8_t su8FadeIndex = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]);
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            if ((int)(su8FadeIndex + ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]) > 255)
            {
                su8FadeIndex = 255;
            }
            else
            {
                su8FadeIndex += ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex];
            }
        }
        else
        {
            if ((int)(su8FadeIndex - ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]) < 0)
            {
                su8FadeIndex = 0;
            }
            else
            {
                su8FadeIndex -= ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex];
            }
        }
        nscale8(FpLeds, NB_PIXELS, su8FadeIndex);
        FastLED.show();
    }
}

/**
 * @brief Alternating colors
 * 
 * @param FpLeds 
 */
void vAnim_RunAlternate(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint8_t su8FlipFlop = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + ctu16BlinkRates[stAnim_MasterConfig.u8BlinkRateIndex];
        su8FlipFlop ^= 1;
        FastLED.clear();

        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            if (su8FlipFlop)
            {
                fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]);
            }
            else
            {
                fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]);
            }
        }
            
        FastLED.show();
    }
}

void vAnim_ConfigBlink(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint8_t su8FlipFlop = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + ctu16BlinkRates[stAnim_MasterConfig.u8BlinkRateIndex];
        su8FlipFlop ^= 1;
        FastLED.clear();
        if(su8FlipFlop)
        {
            fill_solid(FpLeds, NB_PIXELS, CRGB::White);
        }
        FastLED.show();
    }
}

void vAnim_ConfigFade(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint8_t su8FadeIndex = 0;
    static uint8_t su8FlipFlop = 0;

    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        fill_solid(FpLeds, NB_PIXELS, CRGB::White);
        if (su8FlipFlop)
        {
            if ((int)(su8FadeIndex + ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]) > 255)
            {
                su8FadeIndex = 255;
                su8FlipFlop ^= 1;
            }
            else
            {
                su8FadeIndex += ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex];
            }
        }
        else
        {
            if ((int)(su8FadeIndex - ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]) < 0)
            {
                su8FadeIndex = 0;
                su8FlipFlop ^= 1;
            }
            else
            {
                su8FadeIndex -= ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex];
            }
        }
        
        nscale8(FpLeds, NB_PIXELS, su8FadeIndex);
        FastLED.show();
    }
}

/**
 * @brief Runtime option click fall
 * 
 */
void vAnim_CbClickFall(void)
{
    u32LastFallingEvent = millis();
}

/**
 * @brief Long click management
 * 
 */
void vAnim_CbLongClick(void)
{   // used to exit menu & submenu
    switch(eAnim_CurrentState)
    {
        case eAnim_StateRun:
        eAnim_CurrentState = eAnim_StateSelect;
        vAnim_OnEntrySelect(MainLedStip);
        break;

        case eAnim_StateSelect:
        eAnim_CurrentState = eAnim_StateRun;
        vAnim_OnExitSelect(MainLedStip);
        break;

        case eAnim_eStateSubParam:
        eAnim_CurrentState = eAnim_StateSelect;
        vAnim_OnExitSelect(MainLedStip);
        break;

    }
}

/**
 * @brief On release click button
 * 
 */
void vAnim_CbClickRise(void) 
{
    uint32_t u32Now = millis();
    if (((u32Now - u32LastFallingEvent) >= TIME_CLICK_DURATION_MIN) && ((u32Now - u32LastFallingEvent) < TIME_CLICK_DURATION_MAX))
    {   // short click detected
        switch(eAnim_CurrentState)
        {
            case eAnim_StateRun:
            switch (stAnim_MasterConfig.eMode)
            {
                case eAnim_RunSolid:
                case eAnim_RunBlink:
                case eAnim_RunFade:
                stAnim_MasterConfig.u8MainColorIndex++;
                stAnim_MasterConfig.u8MainColorIndex %= ANIM_COLOR_NB;
                break;

                case eAnim_RunAlter:
                stAnim_MasterConfig.u8BlinkRateIndex++;
                stAnim_MasterConfig.u8BlinkRateIndex %= 3;
                break;
            }
            break;

            case eAnim_StateSelect:
            stAnim_MasterConfig.eMode = (TeAnim_RunMode)((stAnim_MasterConfig.eMode + 1) % eAnim_NbRun);
            break;

            case eAnim_eStateSubParam:
            switch(stAnim_MasterConfig.eMode)
            {
                case eAnim_RunSolid:
                break;

                case eAnim_RunBlink:
                stAnim_MasterConfig.u8BlinkRateIndex++;
                stAnim_MasterConfig.u8BlinkRateIndex %= 3;
                break;
                
                case eAnim_RunFade:
                {
                    stAnim_MasterConfig.u8FadeRateIndex++;
                    stAnim_MasterConfig.u8FadeRateIndex %= 3;
                }
                break;

                case eAnim_RunAlter:
                if (stAnim_MasterConfig.u8SubMenu == 0)
                {
                    stAnim_MasterConfig.u8MainColorIndex++;
                    stAnim_MasterConfig.u8MainColorIndex %= ANIM_COLOR_NB;
                }
                else
                {
                    stAnim_MasterConfig.u8SecColorIndex++;
                    stAnim_MasterConfig.u8SecColorIndex %= ANIM_COLOR_NB;
                }
                break;

                default:
                break;
            }
            break;

            default:
            break;
        }
    }
}

void vAnim_CbClickSubMenu(void)
{   // callback attached to BUTTON pin, manage submenu nav
    if (eAnim_CurrentState == eAnim_StateSelect)
    {
        eAnim_CurrentState = eAnim_eStateSubParam;
    }
    else if ((eAnim_CurrentState == eAnim_eStateSubParam) && (stAnim_MasterConfig.eMode == eAnim_RunAlter))
    {
        stAnim_MasterConfig.u8SubMenu++;
        stAnim_MasterConfig.u8SubMenu %= 2;
    }
}

void vAnim_OnEntrySelect(CRGB* FpLeds)
{
    fill_solid(FpLeds, NB_PIXELS, CRGB::White);
    FastLED.show();
    delay(500);
    FastLED.clear();
    FastLED.show();
}

void vAnim_OnExitSelect(CRGB* FpLeds)
{
    fill_solid(FpLeds, NB_PIXELS, CRGB::Green);
    FastLED.show();
    delay(500);
    FastLED.clear();
    FastLED.show();
}

void vAnim_MenuBlink(CRGB* FpLeds, CRGB FSetColor, uint8_t Fu8Repeat)
{
    static uint32_t su32timeout = 0;
    if (millis() > su32timeout)
    {
        su32timeout = TIME_MENU_BLINK_LOOP + millis();
        for (uint8_t i = 0; i < Fu8Repeat; i++)
        {
            fill_solid(FpLeds, NB_PIXELS, FSetColor);
            FastLED.show();
            delay(50);
            FastLED.clear();
            FastLED.show();
            delay(150);
        }
    }
}
