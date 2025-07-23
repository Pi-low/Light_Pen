/**
 * @brief Global configuration
 * @file config.h
 * @version 0.1
 * @date 2025-07-17
 * @author Nello (nello.chom@protonmail.com)
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdlib.h>
#include <FastLED.h>

/****** /!\ DO NOT CHANGE /!\ ******/
#define DEVICE_SIMPLE       1       // single pixel
#define DEVICE_STRIP        2       // led strip configuration
/***********************************/

#define DEVICE_MODE         DEVICE_SIMPLE ///< configure led mode

// PINOUT CONFIGURATION
#define PIN_BUTTON          3       // trigger (active LOW)
#define PIN_MODE            2       // mode select (active LOW)
#define PIN_DATA            4       // WS2812B GRB 800KHz

// TIMINGS
#define TIME_DEBOUNCE       30      // debounce ms
#define TIME_DOUBLE_CLICK   400     // double click max time ms
#define TIME_LONG_PUSH      2000    // long push delay in ms
#define REFRESH_RATE_HZ     50      // animation rate

#if (DEVICE_MODE == DEVICE_SIMPLE)
#define NB_PIXELS           DEVICE_SIMPLE // onse single led
#else
#define NB_PIXELS           30      // led strip size
#endif

#endif //_CONFIG_H_
