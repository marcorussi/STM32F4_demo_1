/*
 * The MIT License (MIT)
 *
 * Copyright (c) [2015] [Marco Russi]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*
 * This file outch.h represents the header file of the output channels component.
 *
 * Author : Marco Russi
 *
 * Evolution of the file:
 * 17/09/2015 - File created - Marco Russi
 *
*/

#ifndef _LED_INCLUDED_         /* switch to read the header file once */
#define _LED_INCLUDED_         /* one time */




/* ------------ Inclusions ----------------- */

#include <stdint.h>




/* ------------ Exported defines ----------------- */

/* Blinking period in ms */
#define LED_BLINK_PERIOD_VALUE_MS     	((uint16_t)800)	/* 800 ms */

/* Blinking DC in % */
#define LED_BLINK_DC_VALUE_PERCENT		((uint16_t)80)	/* 80 % */




/* ------------ Exported typedefs ----------------- */

/* Channels enum */
/* ATTENTION: this enum should be aligned with PWM channels enum and PORT pin number enum */
typedef enum
{
    LED_KE_FIRST_CH
   ,LED_KE_CHANNEL_1 = LED_KE_FIRST_CH
   ,LED_KE_CHANNEL_2
   ,LED_KE_CHANNEL_3
   ,LED_KE_CHANNEL_4
   ,LED_KE_LAST_CH = LED_KE_CHANNEL_4
   ,LED_KE_CH_CHECK
} led_ke_channels;

/* Channels states enum */
typedef enum
{
    LED_KE_FIRST_CH_STATE
   ,LED_KE_CH_TURN_OFF = LED_KE_FIRST_CH_STATE
   ,LED_KE_CH_BLINKING
   ,LED_KE_CH_TOGGLE
   ,LED_KE_CH_TURN_ON
   ,LED_KE_LAST_CH_STATE = LED_KE_CH_TURN_ON
   ,LED_KE_CH_STATE_CHECK
} led_ke_ch_state;

/* Illumination levels enum */
typedef enum
{
	LED_KE_MIN_ILL_LEVEL,
	LED_KE_ILL_LEVEL_1 = LED_KE_MIN_ILL_LEVEL,
	LED_KE_ILL_LEVEL_2,
	LED_KE_ILL_LEVEL_3,
	LED_KE_ILL_LEVEL_4,
	LED_KE_ILL_LEVEL_5,
	LED_KE_ILL_LEVEL_6,
	LED_KE_MID_ILL_LEVEL = LED_KE_ILL_LEVEL_6,
	LED_KE_ILL_LEVEL_7,
	LED_KE_ILL_LEVEL_8,
	LED_KE_ILL_LEVEL_9,
	LED_KE_ILL_LEVEL_10,
	LED_KE_ILL_LEVEL_11,
	LED_KE_MAX_ILL_LEVEL = LED_KE_ILL_LEVEL_11,
	LED_KE_ILL_LEVEL_CHECK
} led_ke_ill_level;




/* ---------------- Exported Functions Prototypes --------------- */

extern void led_init(void);
extern void led_set_channel_status(led_ke_channels, led_ke_ch_state);
extern void led_set_illumination_level(led_ke_channels, led_ke_ill_level);
extern void led_manage_blinking(void);
extern void led_periodic_task(void);




#endif

/* END OF FILE */
