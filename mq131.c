/*
 * mq131.c
 *
 *  Created on: Feb 9, 2019
 *      Author: TritonCubed
 */

#include "mq131.h"
#include "bcm2835.h"

void mq131_init() {
  //configure GPIO pin 11 to gpio input functionality
  bcm2835_gpio_fsel( RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_INPT );
}

unsigned char mq131_read() {
  return bcm2835_gpio_lev( RPI_V2_GPIO_P1_11 );
}

void mq131_close() {
	//doesn't do anything, but contributes to the logical flow of the program
}
