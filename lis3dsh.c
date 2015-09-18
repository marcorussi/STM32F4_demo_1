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

#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "lis3dsh.h"




/* ---------------- Local Defines ----------------- */

/* Number of axis */
#define NUM_OF_AXIS						3

/* LIS3DSH registers addresses */
#define ADD_REG_WHO_AM_I				0x0F
#define ADD_REG_CTRL_4					0x20
#define ADD_REG_OUT_X_L					0x28
#define ADD_REG_OUT_X_H					0x29
#define ADD_REG_OUT_Y_L					0x2A
#define ADD_REG_OUT_Y_H					0x2B
#define ADD_REG_OUT_Z_L					0x2C
#define ADD_REG_OUT_Z_H					0x2D

/* WHO AM I register default value */
#define UC_WHO_AM_I_DEFAULT_VALUE		0x3F

/* ADD_REG_CTRL_4 register configuration value: X,Y,Z axis enabled and 400Hz of output data rate */
#define UC_ADD_REG_CTRL_4_CFG_VALUE		0x77

/* Sensitivity for 2G range [mg/digit] */
#define SENS_2G_RANGE_MG_PER_DIGIT		((float)0.06)




/* ---------------- Local Macros ----------------- */

/* set read single command. Attention: command must be 0x3F at most */
#define SET_READ_SINGLE_CMD(x)			(x | 0x80)
/* set read multiple command. Attention: command must be 0x3F at most */
#define SET_READ_MULTI_CMD(x)			(x | 0xC0)
/* set write single command. Attention: command must be 0x3F at most */
#define SET_WRITE_SINGLE_CMD(x)			(x & (~(0xC0)))
/* set write multiple command. Attention: command must be 0x3F at most */
#define SET_WRITE_MULTI_CMD(x)			(x & (~(0x80))	\
										x |= 0x40)




/* ----------- Local variables declaration ------------- */

/* Array to store axis register address */
static const uint8_t axis_reg_addr_array[NUM_OF_AXIS][2] = {
	{ADD_REG_OUT_X_L, ADD_REG_OUT_X_H},
	{ADD_REG_OUT_Y_L, ADD_REG_OUT_Y_H},
	{ADD_REG_OUT_Z_L, ADD_REG_OUT_Z_H}
};




/* ----------- Local functions prototypes ------------- */

static void write_reg(uint8_t, uint8_t);
static uint8_t read_reg(uint8_t);
static void	spi_setup(void);
static void gpio_setup(void);
static inline int16_t two_compl_to_int16(uint16_t);




/* ------------- Exported functions implementation --------------- */

/* Function to initialise the LIS3DSH */
void lis3dsh_init(void)
{
	uint8_t reg_value;

	/* initialise GPIO */
	gpio_setup();

	/* initialise SPI1 */
	spi_setup();

	/* get WHO AM I value */
	reg_value = read_reg(ADD_REG_WHO_AM_I);

	/* if WHO AM I value is the expected one */
	if (reg_value == UC_WHO_AM_I_DEFAULT_VALUE) {
		/* set output data rate to 400 Hz and enable X,Y,Z axis */
		write_reg(ADD_REG_CTRL_4, UC_ADD_REG_CTRL_4_CFG_VALUE);
		/* verify written value */
		reg_value = read_reg(ADD_REG_CTRL_4);
		/* if written value is different */
		if (reg_value != UC_ADD_REG_CTRL_4_CFG_VALUE) {
			/* ERROR: stay here... */
			while (1);
		}
	} else {
		/* ERROR: stay here... */
		while (1);
	}
}


/* Function to read an axis value */
int16_t lis3dsh_readAxis(uint8_t req_axis)
{
	int16_t int_value_mg = 0;

	if (req_axis < NUM_OF_AXIS) {
		/* get 16-bit value */
		int_value_mg = ((read_reg(axis_reg_addr_array[req_axis][1]) << 8) |
						read_reg(axis_reg_addr_array[req_axis][0]));

		/* transform X value from two's complement to 16-bit int */
		int_value_mg = two_compl_to_int16(int_value_mg);

		/* convert X absolute value to mg value */
		int_value_mg = int_value_mg * SENS_2G_RANGE_MG_PER_DIGIT;
	} else {
		/* invalid axis: do nothing */
	}

	return int_value_mg;
}




/* ------------ Local functions implementation -------------- */

/* Function to write a register to LIS3DSH through SPI  */
static void write_reg(uint8_t reg, uint8_t data)
{
	/* set CS low */
	gpio_clear(GPIOE, GPIO3);
	/* discard returned value */
	spi_xfer(SPI1, SET_WRITE_SINGLE_CMD(reg));
	spi_xfer(SPI1, data);
	/* set CS high */
	gpio_set(GPIOE, GPIO3);
}


/* Function to read a register from LIS3DSH through SPI */
static uint8_t read_reg(uint8_t reg)
{
	uint8_t reg_value;
	/* set CS low */
	gpio_clear(GPIOE, GPIO3);
	reg_value = spi_xfer(SPI1, SET_READ_SINGLE_CMD(reg));
	reg_value = spi_xfer(SPI1, 0xFF);
	/* set CS high */
	gpio_set(GPIOE, GPIO3);

	return reg_value;
}


/* Function to setup the SPI1 */
static void spi_setup(void)
{
	/* Enable SPI1 clock. */
	rcc_periph_clock_enable(RCC_SPI1);

	/* reset SPI1 */
	spi_reset(SPI1);
	/* init SPI1 master */
	spi_init_master(SPI1,
					SPI_CR1_BAUDRATE_FPCLK_DIV_64,
					SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1,
					SPI_CR1_DFF_8BIT,
					SPI_CR1_MSBFIRST);
	/* enable SPI1 first */
	spi_enable(SPI1);
}


/* Function to setup the used GPIO */
static void gpio_setup(void)
{
	/* LIS3DSH pins map:
	PA5 - SPI1_SCK
	PA6 - SPI1_MISO
	PA7 - SPI1_MOSI
	PE3 - CS_SPI
	*/
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
	/* set SPI pins as CLK, MOSI, MISO */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	/* Push Pull, Speed 100 MHz */
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO5 | GPIO6 | GPIO7);
	/* Alternate Function: SPI1 */
	gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

	/* Enable GPIOE clock. */
	rcc_periph_clock_enable(RCC_GPIOE);
	/* set CS as OUTPUT */
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	/* Push Pull, Speed 100 MHz */
	gpio_set_output_options(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO3);
	/* set CS high */
	gpio_set(GPIOE, GPIO3);
}


/* Transform a two's complement value to 16-bit int value */
static inline int16_t two_compl_to_int16(uint16_t two_compl_value)
{
	int16_t int16_value = 0;

	/* conversion */
	if (two_compl_value > 32768) {
		int16_value = -(((~two_compl_value) & 0xFFFF) + 1);
	} else {
		int16_value = two_compl_value;
	}

	return int16_value;
}




/* End of file */

