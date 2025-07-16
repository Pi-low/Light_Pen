#ifndef _UTILS_INT_H_
#define _UTILS_INT_H_
#include <stdint.h>

typedef struct {
    uint8_t u8CurrState;
    uint8_t u8PrevState;
    uint32_t u32LastEvent;
} TstUtils_pin;

#endif //_UTILS_INT_H_