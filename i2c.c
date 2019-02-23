/*
 * i2c.c
 *
 *  Created on: Feb 18, 2019
 *      Author: TritonCubed
 *
 *  The protocol uses the following pins:
 *
 *  RPI
 *  ------------
 *  Pin  5 (SCL)
 *  Pin  3 (SDA)
 */

#include <bcm2835.h>
#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Configures I2C with the proper bitrate
 */
void i2c_init() {
  if( !bcm2835_i2c_begin() ) {
    printf( "bcm2835_i2c_start failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }
  bcm2835_i2c_set_baudrate( BCM2835_I2C_CLOCK_DIVIDER_2500 );
}

/*
 * Closes the I2C session.
 */
void i2c_close() {
  bcm2835_i2c_end();
}
