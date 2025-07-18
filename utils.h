/**
 * @brief Tools and misc
 * @file utils.h
 * @version 0.1
 * @date 2025-07-17
 * @author Nello (nello.chom@protonmail.com)
 */

#ifndef _UTILS_H_
#define _UTILS_H_

/*********************************************************************************
* Includes
*********************************************************************************/
#include "config.h"

/*********************************************************************************
* Types & definitions
*********************************************************************************/
typedef enum {
    eUtils_Button,
    eUtils_Select
} TeUtils_BtnType;

typedef enum {
    eUtils_Falling = 0,
    eUtils_Rising,
    eUtils_Long
} TeUtils_Edge;

typedef enum {
    eUtils_Active = 0,
    eUtils_Idle
} TeUtils_BtnState;

typedef void (*pvEdgeCallback)(void);

/*********************************************************************************
* External functions
*********************************************************************************/
void vUtils_SetButtonCallback(TeUtils_Edge FeEdge, pvEdgeCallback xCallback);
void vUtils_SetModeCallback(TeUtils_Edge FeEdge, pvEdgeCallback xCallback);
void vUtils_ButtonManager(void);
TeUtils_BtnState eUtils_GetButtonState(TeUtils_BtnType FeType);


#endif //_UTILS_H_
