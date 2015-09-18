/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Marco Russi
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
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


/* ---------------- Inclusions ----------------- */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <libopencm3/stm32/gpio.h>

#include "app.h"
/* RTOS module */
#include "rtos.h"
/* LIS3DSH accelerometer driver */
#include "lis3dsh.h"
/* LED module */
#include "led.h"




/* --------------- Local macros declaration ------------- */

#define LED_TH_MG					1000	/* 1000mg */




/* --------------- Local macros declaration ------------- */

/* Macros for turning LEDs ON */
#define LED_GREEN_ON()					(led_set_channel_status(LED_KE_CHANNEL_1, LED_KE_CH_TURN_ON))
#define LED_ORANGE_ON()					(led_set_channel_status(LED_KE_CHANNEL_2, LED_KE_CH_TURN_ON))
#define LED_RED_ON()					(led_set_channel_status(LED_KE_CHANNEL_3, LED_KE_CH_TURN_ON))
#define LED_BLUE_ON()					(led_set_channel_status(LED_KE_CHANNEL_4, LED_KE_CH_TURN_ON))

/* Macros for turning LEDs OFF */
#define LED_GREEN_OFF()					(led_set_channel_status(LED_KE_CHANNEL_1, LED_KE_CH_TURN_OFF))
#define LED_ORANGE_OFF()				(led_set_channel_status(LED_KE_CHANNEL_2, LED_KE_CH_TURN_OFF))
#define LED_RED_OFF()					(led_set_channel_status(LED_KE_CHANNEL_3, LED_KE_CH_TURN_OFF))
#define LED_BLUE_OFF()					(led_set_channel_status(LED_KE_CHANNEL_4, LED_KE_CH_TURN_OFF))




/* --------------- Exported functions ------------- */

/* Application init */
void app_init(void)
{
	/* init LED channels illumination levels and state */
	led_set_illumination_level(LED_KE_CHANNEL_1, LED_KE_MAX_ILL_LEVEL);
	led_set_illumination_level(LED_KE_CHANNEL_2, LED_KE_MAX_ILL_LEVEL);
	led_set_illumination_level(LED_KE_CHANNEL_3, LED_KE_MAX_ILL_LEVEL);
	led_set_illumination_level(LED_KE_CHANNEL_4, LED_KE_MAX_ILL_LEVEL);
	led_set_channel_status(LED_KE_CHANNEL_1, LED_KE_CH_TURN_OFF);
	led_set_channel_status(LED_KE_CHANNEL_2, LED_KE_CH_TURN_OFF);
	led_set_channel_status(LED_KE_CHANNEL_3, LED_KE_CH_TURN_OFF);
	led_set_channel_status(LED_KE_CHANNEL_4, LED_KE_CH_TURN_OFF);
}


/* Application main function */
void app_main_demo(void)
{
	int16_t int_value_x_mg, int_value_y_mg, int_value_z_mg;

	/* get X, Y, Z values */
	int_value_x_mg = lis3dsh_readAxis(LIS3DSH_AXIS_X);
	int_value_y_mg = lis3dsh_readAxis(LIS3DSH_AXIS_Y);
	int_value_z_mg = lis3dsh_readAxis(LIS3DSH_AXIS_Z);

	/* set X related LEDs according to specified threshold */
	if (int_value_x_mg >= LED_TH_MG) {
		LED_BLUE_OFF();
		LED_ORANGE_OFF();
		LED_GREEN_OFF();
		LED_RED_ON();
	} else if (int_value_x_mg <= -LED_TH_MG) {
		LED_BLUE_OFF();
		LED_ORANGE_OFF();
		LED_RED_OFF();
		LED_GREEN_ON();
	}

	/* set Y related LEDs according to specified threshold */
	if (int_value_y_mg >= LED_TH_MG) {
		LED_BLUE_OFF();
		LED_RED_OFF();
		LED_GREEN_OFF();
		LED_ORANGE_ON();
	} else if (int_value_y_mg <= -LED_TH_MG) {
		LED_RED_OFF();
		LED_GREEN_OFF();
		LED_ORANGE_OFF();
		LED_BLUE_ON();
	}

	/* set Z related LEDs according to specified threshold */
	if (int_value_z_mg >= LED_TH_MG) {
		LED_BLUE_ON();
		LED_ORANGE_ON();
		LED_RED_ON();
		LED_GREEN_ON();
	} else if (int_value_z_mg <= -LED_TH_MG) {
		LED_BLUE_OFF();
		LED_ORANGE_OFF();
		LED_RED_OFF();
		LED_GREEN_OFF();
	}
}




/* End of file */



