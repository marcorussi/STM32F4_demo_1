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
 * This file outch.c represents the source file of the output channels component.
 *
 * Author : Marco Russi
 *
 * Evolution of the file:
 * 17/09/2015 - File created - Marco Russi
 *
*/


//TODO: Modify the timer tick define dependency
//TODO: decide if blinking period and DC should be dynamically configured or not. They are constant now.


/* ---------------- Inclusions ------------------------ */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "tmr.h"                /* component TMR header file */
#include "pwm.h"                /* component PWM header file */
#include "rtos.h"               /* component RTOS header file */
#include "led.h"              	/* component header file */




/* ----------------- Local defines ----------------- */

/* Blinking period counter value */
#define US_BLINK_PERIOD_COUNTER_VALUE   ((uint16_t)((uint32_t)(LED_BLINK_PERIOD_VALUE_MS * 1000) / RTOS_UL_TICK_PERIOD_US))

/* Blinking duty cycle counter value */
#define US_BLINK_DC_COUNTER_VALUE       ((uint16_t)((US_BLINK_PERIOD_COUNTER_VALUE * LED_BLINK_DC_VALUE_PERCENT) / 100))




/* ----------------- Local Macros ----------------- */

/* Macro to set ON status request */
#define SET_TURN_ON_REQ(i)             (u8StatusReqStateChs |= (1 << (i)))
/* Macro to set OFF status request */
#define SET_TURN_OFF_REQ(i)            (u8StatusReqStateChs &= ~(1 << (i)))
/* Macro to check turn ON request */
#define CHECK_TURN_ON_REQ(i)           ((u8StatusReqStateChs & (1 << (i))) != 0)

/* Macro to set blinking LED request */
#define SET_BLINKING_REQ(i)            (ui8BlinkingReqChs |= (1 << (i)))
/* Macro to clear blinking LED request */
#define CLEAR_BLINKING_REQ(i)          (ui8BlinkingReqChs &= ~(1 << (i)))
/* Macro to check blinking LED request */
#define CHECK_BLINKING_REQ(i)          ((ui8BlinkingReqChs & (1 << (i))) != 0)
/* Macro to check blinking enable */
#define CHECK_BLINKING_ENABLED()       (ui8BlinkingReqChs != 0)
/* Macro to set blinking status ON */
#define SET_BLINKING_STATUS_ON()       (blinking_status_flag = true)
/* Macro to set blinking status OFF */
#define SET_BLINKING_STATUS_OFF()      (blinking_status_flag = false)
/* Macro to check blinking status */
#define CHECK_BLINKING_STATUS_ON()     (blinking_status_flag == true)




/* ----------------- Local Variables ----------------------- */

/* Module info flags register */
static bool blinking_status_flag;

/* Channels blinking request bits order as ILL_ke_Channels enum: 1 required - 0 not required. 8 channels */
static uint8_t ui8BlinkingReqChs;

/* Channels state to apply bits order as ILL_ke_ChState enum: 1 ON - 0 OFF. 8 channels*/
static uint8_t u8StatusReqStateChs;

/* Blinking period timeout */
static uint16_t ui16BlinkPeriodCounter;

/* Blinking TON timeout */
static uint16_t ui16BlinkTONCounter;

/* Blinking counter */
static uint16_t ui16BlinkingCounter;

/* Permillage PWM to ill level association array */
static uint16_t ill_levels_pwm_values[LED_KE_ILL_LEVEL_CHECK] =
{
	50,
    100,
    200,
    300,
    400,
    500,
	600,
	700,
	800,
	900,
	950
};

/* Permillage PWM to apply for each channel. Set an average value as default */
static uint16_t channels_pwm_values[LED_KE_CH_CHECK] =
{
   500,
   500,
   500,
   500
};




/* ---------------- Local Functions Prototypes ------------------ */

static void set_channel_output(led_ke_channels);
static void reset_channel_output(led_ke_channels);




/* ----------------- Exported Functions ----------------------- */

/* Init LED module */
void led_init(void)
{
    /* init module info flags */
    SET_BLINKING_STATUS_OFF();

    /* init blinking counter */
    ui16BlinkingCounter = 0;

    /* init blinking period counter value */
    ui16BlinkPeriodCounter = US_BLINK_PERIOD_COUNTER_VALUE;
      
    /* init blinking TON counter value */
    ui16BlinkTONCounter = US_BLINK_DC_COUNTER_VALUE;

    /* init PWM module and channels */
    pwm_init();
    pwm_set_frequency(1000);
    pwm_set_dc(PWM_CH1, 0);
    pwm_set_dc(PWM_CH2, 0);
    pwm_set_dc(PWM_CH3, 0);
    pwm_set_dc(PWM_CH4, 0);
    pwm_start();
}


/* Set LED channel status */
void led_set_channel_status(led_ke_channels ch_index, led_ke_ch_state required_state)
{
   /* check parameters */
   if ((ch_index < LED_KE_CH_CHECK)
   && (required_state < LED_KE_CH_STATE_CHECK)) {
      /* select status. In event of unknown status LED is turned off */
      switch(required_state)
      {
         /* turn ON */
         case LED_KE_CH_TURN_ON:
         {
            /* blinking is not required */
            CLEAR_BLINKING_REQ((uint8_t)ch_index);
            /* set to ON */
            SET_TURN_ON_REQ((uint8_t)ch_index);
            /* set channel immediately */
            set_channel_output(ch_index);
            break;
         }
         /* blinking */
         case LED_KE_CH_BLINKING:
         {
            /* blinking is required */
            SET_BLINKING_REQ((uint8_t)ch_index);
            /* blinking status is set synchronously to other blinking LEDs */
            break;
         }
         /* toggle */
         case LED_KE_CH_TOGGLE:
         {
            /* toggle status */
            if (CHECK_TURN_ON_REQ((uint8_t)ch_index)) {
                SET_TURN_OFF_REQ((uint8_t)ch_index);
                /* reset channel immediately */
                reset_channel_output(ch_index);
            } else {
                SET_TURN_ON_REQ((uint8_t)ch_index);
                /* set channel immediately */
                set_channel_output(ch_index);
            }
            /* ATTENTION: evaluate if it is necessary to update the
             * required channel immediately or at the next task run */
            break;
         }
         /* turn OFF and default */
         case LED_KE_CH_TURN_OFF:
         {
            /* set to OFF */
            SET_TURN_OFF_REQ((uint8_t)ch_index);
            /* LED is turned off immediately if it is not blinking only.
               If LED is blinking the turn off status shall be updated
               with other blinking LEDs */
            if (CHECK_BLINKING_REQ((uint8_t)ch_index)) {
               /* clear blinking request */
               CLEAR_BLINKING_REQ((uint8_t)ch_index);
            } else {
                /* reset channel immediately */
                reset_channel_output(ch_index);
            }
            break;
         }
         default:
         {
            /* invalid status: do nothing */
         }
      }
   } else {
      /* invalid LED index: do nothing */
   }
}


/* Set illumination level - for ill channels only */
void led_set_illumination_level(led_ke_channels channel_id, led_ke_ill_level ill_level)
{
    /* check required level */
    if ((ill_level < LED_KE_ILL_LEVEL_CHECK)
    && (channel_id < LED_KE_CH_CHECK)) {
        /* set required PWM value for required channel */
    	channels_pwm_values[(uint8_t)channel_id] = ill_levels_pwm_values[(uint8_t)ill_level];
    } else {
        /* do nothing - discard request */
    }
}


/* Manage blinking */
void led_manage_blinking(void)
{
    /* if almost one LED is required to blink */
    if (CHECK_BLINKING_ENABLED()) {
        /* increment blinking counter */
        ui16BlinkingCounter++;

        /* check if TOFF is elapsed. The >= is to avoid uncorrect overflow conditions */
        if (ui16BlinkingCounter >= ui16BlinkPeriodCounter) {
            /* TOFF is elapsed: turn ON */
            SET_BLINKING_STATUS_ON();

            /* New period and TON values are constants value.
            Do not update at every period. */

            /* reset blinking counter */
            ui16BlinkingCounter = 0;
        } else if (ui16BlinkingCounter == ui16BlinkTONCounter) {
            /* TON is elapsed: turn OFF */
            SET_BLINKING_STATUS_OFF();
        } else {
            /* leave counter to reach TON or TOFF */
        }
    } else {
        /* reset blinking counter */
        ui16BlinkingCounter = 0;
    }
}


/* ILL periodic task */
void led_periodic_task(void)
{
	uint8_t ch_index;

	/* toggle blinking channels status if required and apply it */
	for (ch_index = LED_KE_FIRST_CH; ch_index <= LED_KE_LAST_CH; ch_index++) {
		/* if blink channel required */
		if (CHECK_BLINKING_REQ(ch_index)) {
			/* update channel status info and apply it */
			if (CHECK_BLINKING_STATUS_ON()) {
				SET_TURN_ON_REQ(ch_index);
				/* ATTENTION: this operation should be done after for loop
				 * in order to update all channels simultaneously */
				/* set channel output */
				set_channel_output(ch_index);
			} else {
				SET_TURN_OFF_REQ(ch_index);
				/* ATTENTION: this operation should be done after for loop
				 * in order to update all channels simultaneously */
				/* reset channel output */
				reset_channel_output(ch_index);
			}
		} else {
			/* update eventual illumination level */
			if (CHECK_TURN_ON_REQ(ch_index)) {
				/* set channel output */
				set_channel_output(ch_index);
			} else {
				/* do nothing */
			}
		}
	}
}




/* ---------------- Local Functions ------------------ */

/* set a channel output status according to its channel number */
static void set_channel_output( led_ke_channels channel_id )
{
    /* set channel status */
    if (channel_id < LED_KE_CH_CHECK) {
        /* update related PWM channel */
    	pwm_set_dc(channel_id, channels_pwm_values[(uint8_t)channel_id]);
    } else {
    	/* discard the request, do nothing */
    }
}


/* set a channel output status according to its channel number */
static void reset_channel_output( led_ke_channels channel_id )
{
    /* reset channel status */
    if (channel_id < LED_KE_CH_CHECK) {
        /* update related PWM channel */
    	pwm_set_dc(channel_id, 0);
    } else {
    	/* discard the request, do nothing */
    }
}




/* END OF FILE */
