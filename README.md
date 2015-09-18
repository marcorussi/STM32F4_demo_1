# STM32F4_demo_1
A demo application with LIS3DSH accelerometer driver, LED driver on top of a PWM driver and a minimal RTOS (scheduler).

For compiling the project it is necessary to download the libopencm3 library https://github.com/libopencm3/libopencm3. This project folder must be located in the same folder where libopencm3 is located. For compiling the project run:

    $ make

and for flashing the STM32F4Discovery board run:

    $ make flash

The default toolchain is the same of libopencm3, an arm-none-eabi/arm-elf toolchain.

