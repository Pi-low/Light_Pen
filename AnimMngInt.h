/**
 * @brief Animation manager internal header
 * @file AnimMngInt.h
 * @version 0.1
 * @date 2025-07-23
 * @author your name (you@domain.com)
 */

/*********************************************************************************
* Includes
*********************************************************************************/
#include "Config.h"

/*********************************************************************************
* Types & definitions
*********************************************************************************/
#define REFRESH_TIMEOUT (1000/REFRESH_RATE_HZ)
#define TIME_CLICK_DURATION_MIN     0
#define TIME_CLICK_DURATION_MAX     500
#define TIME_MENU_BLINK_LOOP        1000
#define TIME_FADE_CONFIG_LOOP       2000
#define ANIM_COLOR_NB 17

typedef enum {
    eAnim_StateRun = 0,
    eAnim_StateSelect,
    eAnim_eStateSubParam
} TeAnim_State;

typedef enum {
    eAnim_RunSolid = 0,
    eAnim_RunFade,
    eAnim_RunBlink,
    eAnim_RunAlter,
#if (DEVICE_MODE != DEVICE_SIMPLE)
    eAnim_RunGradient,
    eAnim_RunBicolor,
    eAnim_RunEdge,
#endif
    eAnim_NbRun
} TeAnim_RunMode;

typedef struct {
    uint8_t u8MainColorIndex;
    uint8_t u8SecColorIndex;
    uint8_t u8BlinkRateIndex;     //< refers to index ctu16BlinkRates
    uint8_t u8FadeRateIndex;      //< refers to index ctu16FadeRates
    uint8_t u8EdegeSize;
    TeAnim_RunMode eMode;
    uint8_t u8SubMenu;
} TstAnim_Configuration;

/*********************************************************************************
* Functions prototypes
*********************************************************************************/
// Callbacks
void vAnim_CbClickFall(void);
void vAnim_CbClickRise(void);
void vAnim_CbLongClick(void);
void vAnim_CbClickFall(void);

// Animations
void vAnim_RunSolid(CRGB* FpLeds);
void vAnim_RunBlink(CRGB* FpLeds);
void vAnim_RunFade(CRGB* FpLeds);
void vAnim_RunAlternate(CRGB* FpLeds);
#if (DEVICE_MODE != DEVICE_SIMPLE)
void vAnim_RunGradient(CRGB* FpLeds);
void vAnim_RunEdge(CRGB* FpLeds);    // edges are filled with secondary solor
void vAnim_RunBicolor(CRGB* FpLeds); // split half with main and secondary color
#endif

// Configurations
void vAnim_ConfigBlink(CRGB* FpLeds);
void vAnim_ConfigFade(CRGB* FpLeds);
#if (DEVICE_MODE != DEVICE_SIMPLE)
void vAnim_ConfigGradient(CRGB* FpLeds);
void vAnim_ConfigBicolor(CRGB* FpLeds);
void vAnim_ConfigEdge(CRGB* FpLeds);
void vAnim_ConfigAlternate(CRGB* FpLeds);
#endif

// Menu utils
void vAnim_OnEntrySelect(CRGB* FpLeds);
void vAnim_OnExitSelect(CRGB* FpLeds);
void vAnim_CbClickSubMenu(void);
void vAnim_MenuBlink(CRGB* FpLeds, CRGB FSetColor, uint8_t Fu8Repeat);
#if (DEVICE_MODE != DEVICE_SIMPLE)
void vAnim_MenuStripDisplay(CRGB* FpLeds, CRGB FSetColor, uint8_t Fu8Index);
#endif
