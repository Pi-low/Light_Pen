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

typedef struct {
    CRGB MainColor;
    CRGB SecColor;
    uint8_t u8ColorIndex;
    uint16_t u16BlinkPerMs;
    uint16_t u16AlterPerMs;
    uint16_t u16FadeRate;
    TeAnim_RunMode eMode;
} TstAnim_Configuration;

/*********************************************************************************
* Internal functions prototypes
*********************************************************************************/
static void vAnim_CbClickRunParam(void);
static void vAnim_RunSolid(CRGB* FpLeds);
static void vAnim_RunBlink(CRGB* FpLeds);
static void vAnim_RunFade(CRGB* FpLeds);
static void vAnim_RunAlternate(CRGB* FpLeds);
static void vAnim_RunDual(CRGB* FpLeds);

/*********************************************************************************
* Global variables
*********************************************************************************/
static TeAnim_State eAnim_CurrentState = eAnim_StateRun;
static TstAnim_Configuration stAnim_MasterConfig;

static const uint16_t ctu16BlinkRates[3] = {50, 100, 250};
static const uint16_t ctu16FadeRates[3] = {500, 1000, 2000};
static CRGB ctrgb_Palette[17] = {CRGB::White};

static void (*tpvAnimations[5])(CRGB*) = {
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
    vUtils_SetModeCallback(eUtils_Rising, vAnim_CbClickRunParam);
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
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            fill_solid(FpLeds, NB_PIXELS, stAnim_MasterConfig.MainColor);
        }
        else
        {
            FastLED.clear();
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
 * @brief Runtime option click callback
 * 
 */
void vAnim_CbClickRunParam(void)
{
    if (stAnim_MasterConfig.eMode < eAnim_RunAlter)
    {
        stAnim_MasterConfig.u8ColorIndex = (stAnim_MasterConfig.u8ColorIndex + 1) % ANIM_COLOR_NB;
        stAnim_MasterConfig.MainColor = ctrgb_Palette[stAnim_MasterConfig.u8ColorIndex];
    }
}