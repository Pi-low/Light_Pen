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
#define TIME_CLICK_DURATION_MIN     10
#define TIME_CLICK_DURATION_MAX     200

typedef struct {
    CRGB MainColor;
    CRGB SecColor;
    uint8_t u8ColorIndex;
    uint16_t u16BlinkPerMs;     //< blink period
    uint16_t u16AlterPerMs;     //< alternate period
    uint16_t u16FadeRate;
    TeAnim_RunMode eMode;
} TstAnim_Configuration;

/*********************************************************************************
* Internal functions prototypes
*********************************************************************************/
static void vAnim_CbClickFall(void);
static void vAnim_CbClickRise(void);
static void vAnim_CbLongClick(void);
static void vAnim_OnEntrySelect(CRGB* FpLeds);
static void vAnim_OnExitSelect(CRGB* FpLeds);
static void vAnim_RunSolid(CRGB* FpLeds);
static void vAnim_RunBlink(CRGB* FpLeds);
static void vAnim_RunFade(CRGB* FpLeds);
static void vAnim_RunAlternate(CRGB* FpLeds);
static void vAnim_RunDual(CRGB* FpLeds);

/*********************************************************************************
* Global variables
*********************************************************************************/
extern CRGB MainLedStip[];
static uint32_t u32LastFallingEvent = 0;
static TeAnim_State eAnim_CurrentState = eAnim_StateRun;
static TstAnim_Configuration stAnim_MasterConfig;

static const uint16_t ctu16BlinkRates[3] = {50, 100, 250};
static const uint16_t ctu16FadeRates[3] = {500, 1000, 2000};
static CRGB ctrgb_Palette[17] = {CRGB::White};

static void (*tpvAnimations[5])(CRGB*) = { //animation function pointer array
    vAnim_RunSolid,
    vAnim_RunBlink,
    vAnim_RunFade,
    vAnim_RunAlternate,
    vAnim_RunDual
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
    for (uint8_t u8Index = 0; u8Index < 16; u8Index++)
    {
        ctrgb_Palette[u8Index + 1] = CHSV(u8Index * 16, 255, 255);
    }
    stAnim_MasterConfig.MainColor = ctrgb_Palette[0];
    stAnim_MasterConfig.SecColor = ctrgb_Palette[1];
    stAnim_MasterConfig.u16BlinkPerMs = 100;
    stAnim_MasterConfig.u16AlterPerMs = 100;
    stAnim_MasterConfig.u16FadeRate = 1000;
    stAnim_MasterConfig.eMode = eAnim_RunSolid;
    stAnim_MasterConfig.u8ColorIndex = 0;
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
            fill_solid(FpLeds, NB_PIXELS, stAnim_MasterConfig.MainColor);
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
        su32RefreshTimeOut = millis() + stAnim_MasterConfig.u16BlinkPerMs;
        su8FlipFlop ^= 1;
        FastLED.clear();
        if (su8FlipFlop && (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)) 
        {
            fill_solid(FpLeds, NB_PIXELS, stAnim_MasterConfig.MainColor);
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
    
}

/**
 * @brief Alternating colors
 * 
 * @param FpLeds 
 */
void vAnim_RunAlternate(CRGB* FpLeds)
{
    
}

/**
 * @brief Two color display
 * 
 * @param FpLeds 
 */
void vAnim_RunDual(CRGB* FpLeds)
{
    
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
{
    if (eAnim_CurrentState == eAnim_StateRun)
    {
        eAnim_CurrentState = eAnim_StateSelect;
        vAnim_OnEntrySelect(MainLedStip);
    }
    else if (eAnim_CurrentState == eAnim_StateSelect)
    {
        eAnim_CurrentState = eAnim_StateRun;
        vAnim_OnExitSelect(MainLedStip);
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
            if (stAnim_MasterConfig.eMode < eAnim_RunAlter)
            {
                stAnim_MasterConfig.u8ColorIndex = (stAnim_MasterConfig.u8ColorIndex + 1) % ANIM_COLOR_NB;
                stAnim_MasterConfig.MainColor = ctrgb_Palette[stAnim_MasterConfig.u8ColorIndex];
            }
            break;

            case eAnim_StateSelect:
            stAnim_MasterConfig.eMode = (stAnim_MasterConfig.eMode + 1) % 2;
            break;
        }
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