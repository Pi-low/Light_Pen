/**
 * @brief Manage animation
 * @file AnimMng.h
 * @version 0.1
 * @date 2025-07-17
 * @author Nello (nello.chom@protonmail.com)
 */

#ifndef _ANIM_MNG_H_
#define _ANIM_MNG_H_

/*********************************************************************************
* Includes
*********************************************************************************/
#include "config.h"
#include "utils.h"

/*********************************************************************************
* Types & definitions
*********************************************************************************/
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
    eAnim_NbRun
} TeAnim_RunMode;

/*********************************************************************************
* External functions
*********************************************************************************/
void vAnim_Init(void);
void vAnim_CoreMng(struct CRGB* Fptr);

#endif //_ANIM_MNG_H_
