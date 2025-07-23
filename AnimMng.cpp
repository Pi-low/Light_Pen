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
#include "AnimMngInt.h"

/*********************************************************************************
* Types & definitions
*********************************************************************************/

/*********************************************************************************
* Internal functions prototypes
*********************************************************************************/

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
    vAnim_RunAlternate,
#if (DEVICE_MODE != DEVICE_SIMPLE)
    vAnim_RunGradient,
    vAnim_RunBicolor,
    vAnim_RunEdge
#endif
};


/*********************************************************************************
* External functions
*********************************************************************************/

/*********************************************************************************
 * @brief Initialize animation engine
 * 
 ********************************************************************************/
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
    stAnim_MasterConfig.u8MainColorIndex = 1;
    stAnim_MasterConfig.u8SecColorIndex = 4;
    stAnim_MasterConfig.u8BlinkRateIndex = 0;
    stAnim_MasterConfig.u8FadeRateIndex = 0;
    stAnim_MasterConfig.u8EdegeSize = 1;
    stAnim_MasterConfig.eMode = eAnim_RunSolid;
    stAnim_MasterConfig.u8SubMenu = 0;
}

/*********************************************************************************
 * @brief Run animation engine
 * 
 * @param Fptr 
 ********************************************************************************/
void vAnim_CoreMng(CRGB* Fptr)
{
    switch(eAnim_CurrentState)
    {
        case eAnim_StateRun:
        tpvAnimations[stAnim_MasterConfig.eMode](Fptr);
        break;

        case eAnim_StateSelect:
#if (DEVICE_MODE == DEVICE_SIMPLE)
        vAnim_MenuBlink(Fptr, CRGB::Blue, stAnim_MasterConfig.eMode + 1);
#else
        vAnim_MenuStripDisplay(Fptr, CRGB::Red, stAnim_MasterConfig.eMode + 1);
#endif
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
#if (DEVICE_MODE == DEVICE_SIMPLE)
            if(stAnim_MasterConfig.u8SubMenu == 0)
            { vAnim_MenuBlink(Fptr, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex], 1); }
            else
            {  vAnim_MenuBlink(Fptr, ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex], 2); }
#else
            vAnim_ConfigAlternate(Fptr);
#endif
            break;
#if (DEVICE_MODE != DEVICE_SIMPLE)
            case eAnim_RunGradient:
            vAnim_ConfigGradient(Fptr);
            break;

            case eAnim_RunBicolor:
            vAnim_ConfigBicolor(Fptr);
            break;

            case eAnim_RunEdge:
            vAnim_ConfigEdge(Fptr);
            break;
#endif
            default:
            vAnim_MenuBlink(Fptr, CRGB::Red, 1);
            delay(1000);
            break;
        }
        break;

        default:
        break;
    }
}

/*********************************************************************************
* Internal functions
*********************************************************************************/

/*********************************************************************************
 * @brief single color fill
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_RunSolid(CRGB* FpLeds)
{
    static uint32_t u32timeout = 0;
    if (millis() > u32timeout)
    {
        u32timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        { fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]); }
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Single color blink
 * 
 * @param FpLeds 
 ********************************************************************************/
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
        { fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]); }
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Single color fade in/out
 * 
 * @param FpLeds 
 ********************************************************************************/
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
            { su8FadeIndex = 255; }
            else
            { su8FadeIndex += ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]; }
        }
        else
        {
            if ((int)(su8FadeIndex - ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]) < 0)
            { su8FadeIndex = 0; }
            else
            { su8FadeIndex -= ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]; }
        }
        nscale8(FpLeds, NB_PIXELS, su8FadeIndex);
        FastLED.show();
    }
}

#if (DEVICE_MODE == DEVICE_SIMPLE)
/*********************************************************************************
 * @brief Alternating colors
 * 
 * @param FpLeds 
 ********************************************************************************/
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
            { fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]); }
            else
            { fill_solid(FpLeds, NB_PIXELS, ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]); }
        }
        FastLED.show();
    }
}
#else // led strip
/*********************************************************************************
 * @brief Alternating colors
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_RunAlternate(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            for (uint8_t u8Index = 0; u8Index < NB_PIXELS; u8Index++)
            {
                if ((u8Index % 2) == 0)
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]; }
                else
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]; }
            }
        }
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Gradient with main and secondary color
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_RunGradient(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            CHSV Main = rgb2hsv_approximate(ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]);
            CHSV Secondary = rgb2hsv_approximate(ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]);
            fill_gradient(FpLeds, NB_PIXELS, Main, Secondary);
        }
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Edge fill with secondary color
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_RunEdge(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            for (uint8_t u8Index = 0; u8Index < NB_PIXELS; u8Index++)
            {
                if (u8Index < stAnim_MasterConfig.u8EdegeSize || u8Index >= (NB_PIXELS - stAnim_MasterConfig.u8EdegeSize))
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]; }
                else
                {  FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]; }
            }
        }
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Bicolor fill, split half with main and sec color
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_RunBicolor(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        if (eUtils_GetButtonState(eUtils_Button) == eUtils_Active)
        {
            for (uint8_t u8Index = 0; u8Index < NB_PIXELS; u8Index++)
            {
                if (u8Index < (NB_PIXELS / 2))
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]; }
                else
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]; }
            }
        }
        FastLED.show();
    }
}
#endif

/*********************************************************************************
 * @brief Configuring blink rates
 * 
 * @param FpLeds 
 ********************************************************************************/
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

/*********************************************************************************
 * @brief Configuring fading rates
 * 
 * @param FpLeds 
 ********************************************************************************/
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
            { su8FadeIndex += ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]; }
        }
        else
        {
            if ((int)(su8FadeIndex - ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]) < 0)
            {
                su8FadeIndex = 0;
                su8FlipFlop ^= 1;
            }
            else
            { su8FadeIndex -= ctu16FadeRates[stAnim_MasterConfig.u8FadeRateIndex]; }
        }
        
        nscale8(FpLeds, NB_PIXELS, su8FadeIndex);
        FastLED.show();
    }
}

#if (DEVICE_MODE != DEVICE_SIMPLE)
/*********************************************************************************
 * @brief Configure gradient fill
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_ConfigGradient(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint32_t su32TimeoutBlink = 0;
    static uint8_t su8FlipFlop = 0;

    if (millis() > su32TimeoutBlink)
    {
        su32TimeoutBlink = millis() + 300;
        su8FlipFlop ^= 1;
    }
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        CHSV Main = rgb2hsv_approximate(ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]);
        CHSV Secondary = rgb2hsv_approximate(ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]);
        if (su8FlipFlop)
        {
            if (!stAnim_MasterConfig.u8SubMenu)
            { Main = CHSV(0, 0, 0); }
            else
            { Secondary = CHSV(0, 0, 0); }
        }
        
        fill_gradient(FpLeds, NB_PIXELS, Main, Secondary);
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Configure dual color
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_ConfigBicolor(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint32_t su32TimeoutBlink = 0;
    static uint8_t su8FlipFlop = 0;

    if (millis() > su32TimeoutBlink)
    {
        su32TimeoutBlink = millis() + 300;
        su8FlipFlop ^= 1;
    }
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        
        for (uint8_t u8Index = 0; u8Index < NB_PIXELS; u8Index++)
        {
            if (u8Index < (NB_PIXELS / 2))
            {
                if (!stAnim_MasterConfig.u8SubMenu)
                { FpLeds[u8Index] = (su8FlipFlop) ? ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex] : CRGB::Black; }
                else
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]; }
                
            }
            else
            {
                if (stAnim_MasterConfig.u8SubMenu)
                { FpLeds[u8Index] = (su8FlipFlop) ? ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex] : CRGB::Black; }
                else
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]; }
            }
        }
    
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Configure color alternate
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_ConfigAlternate(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    static uint32_t su32TimeoutBlink = 0;
    static uint8_t su8FlipFlop = 0;

    if (millis() > su32TimeoutBlink)
    {
        su32TimeoutBlink = millis() + 300;
        su8FlipFlop ^= 1;
    }
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        
        for (uint8_t u8Index = 0; u8Index < 4; u8Index++)
        {
            if (u8Index < 2)
            {
                if (!stAnim_MasterConfig.u8SubMenu)
                { FpLeds[u8Index] = (su8FlipFlop) ? ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex] : CRGB::Black; }
                else
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8MainColorIndex]; }
                
            }
            else
            {
                if (stAnim_MasterConfig.u8SubMenu)
                { FpLeds[u8Index] = (su8FlipFlop) ? ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex] : CRGB::Black; }
                else
                { FpLeds[u8Index] = ctrgb_Palette[stAnim_MasterConfig.u8SecColorIndex]; }
            }
        }
        FastLED.show();
    }
}

/*********************************************************************************
 * @brief Configure color edges
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_ConfigEdge(CRGB* FpLeds)
{
    static uint32_t su32Timeout = 0;
    if (millis() > su32Timeout)
    {
        su32Timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        for (uint8_t u8Index = 0; u8Index < NB_PIXELS; u8Index++)
        {
            if (u8Index < stAnim_MasterConfig.u8EdegeSize || u8Index >= (NB_PIXELS - stAnim_MasterConfig.u8EdegeSize))
            {
                FpLeds[u8Index] = CRGB::White;
            }
        }
        FastLED.show();
    }
}
#endif ///DEVICE_STRIP

/*********************************************************************************
 * @brief Runtime option click fall
 * 
 ********************************************************************************/
void vAnim_CbClickFall(void)
{
    u32LastFallingEvent = millis();
}

/*********************************************************************************
 * @brief Long click management
 * 
 ********************************************************************************/
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

/*********************************************************************************
 * @brief On release click button
 * 
 ********************************************************************************/
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

#if (DEVICE_MODE == DEVICE_SIMPLE)
                case eAnim_RunAlter:
                stAnim_MasterConfig.u8BlinkRateIndex++;
                stAnim_MasterConfig.u8BlinkRateIndex %= 3;
                break;
#else
                case eAnim_RunEdge:
                stAnim_MasterConfig.u8EdegeSize++;
                stAnim_MasterConfig.u8EdegeSize %= 11;
                if ((stAnim_MasterConfig.u8EdegeSize > 10) || (!stAnim_MasterConfig.u8EdegeSize))
                { stAnim_MasterConfig.u8EdegeSize = 1; }
                break;
#endif
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
#if (DEVICE_MODE != DEVICE_SIMPLE)
                case eAnim_RunGradient:
                case eAnim_RunBicolor:
#endif
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

#if (DEVICE_MODE != DEVICE_SIMPLE)
                case eAnim_RunEdge:
                stAnim_MasterConfig.u8EdegeSize++;
                stAnim_MasterConfig.u8EdegeSize %= 11;
                if ((stAnim_MasterConfig.u8EdegeSize > 10) || (!stAnim_MasterConfig.u8EdegeSize))
                { stAnim_MasterConfig.u8EdegeSize = 1; }
                break;
#endif

                default:
                break;
            }
            break;

            default:
            break;
        }
    }
}

/*********************************************************************************
 * @brief Click tigger button
 * 
 ********************************************************************************/
void vAnim_CbClickSubMenu(void)
{   // callback attached to BUTTON pin, manage submenu nav
    if (eAnim_CurrentState == eAnim_StateSelect)
    {
        eAnim_CurrentState = eAnim_eStateSubParam;
    }
    else if ((eAnim_CurrentState == eAnim_eStateSubParam)
#if (DEVICE_MODE == DEVICE_SIMPLE)
    && (stAnim_MasterConfig.eMode == eAnim_RunAlter)
#else
    && ((stAnim_MasterConfig.eMode >= eAnim_RunAlter) && (stAnim_MasterConfig.eMode < eAnim_RunEdge))
#endif
    )
    {
        stAnim_MasterConfig.u8SubMenu++;
        stAnim_MasterConfig.u8SubMenu %= 2;
    }
}

/*********************************************************************************
 * @brief 
 * 
 * @param FpLeds 
 ********************************************************************************/
void vAnim_OnEntrySelect(CRGB* FpLeds)
{
    fill_solid(FpLeds, NB_PIXELS, CRGB::Blue);
    FastLED.show();
    delay(500);
    FastLED.clear();
    FastLED.show();
}

/*********************************************************************************
 * 
 ********************************************************************************/
void vAnim_OnExitSelect(CRGB* FpLeds)
{
    fill_solid(FpLeds, NB_PIXELS, CRGB::Green);
    FastLED.show();
    delay(500);
    FastLED.clear();
    FastLED.show();
}

#if (DEVICE_MODE != DEVICE_SIMPLE)
/*********************************************************************************
 * @brief Display number sector, 1 red each 4 pixels
 * 
 * @param FpLeds 
 * @param FSetColor 
 * @param Fu8Index 
 ********************************************************************************/
void vAnim_MenuStripDisplay(CRGB* FpLeds, CRGB FSetColor, uint8_t Fu8Index)
{
    static uint32_t su32timeout = 0;
    if (millis() > su32timeout)
    {
        su32timeout = millis() + REFRESH_TIMEOUT;
        FastLED.clear();
        for (uint8_t i = 0; i < Fu8Index; i++)
        {
            FpLeds[i*4] = FSetColor;
        }
        FastLED.show();
    }
}
#endif

/*********************************************************************************
 * @brief Blink every seconds with repeat
 * 
 * @param FpLeds 
 * @param FSetColor 
 * @param Fu8Repeat 
 ********************************************************************************/
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
