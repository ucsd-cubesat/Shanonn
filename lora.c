/*
 * lora.c
 *
 *  Created on: Feb 3, 2019
 *      Author: TritonCubed
 *
 *  The breakout uses the following pins:
 *
 *  RPI             Chip
 *  --------------------
 *  Pin 14 (GND)      0V
 *  Pin 24            CS
 *  Pin 16            RT
 *  Pin 17 (3V3)     3V3
 *  Pin 23 (SCLK)     CK
 *  Pin 21 (MISO)     SO
 *  Pin 19 (MOSI)     SI
 */

#include "lora.h"
#include "bcm2835.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Boots the lora module.
 * Requires SPI to be initialized.
 */
void lora_init( char bufferSize ) {

	//get current mode
	uint8_t bootmode = read_reg( REG_OP_MODE );

	//Needs to be in FSK
	if( !(bootmode & NON_FSK) ) {
		write_reg( REG_OP_MODE, FSK_SLEEP );
	}

	//proper boot routine
	write_reg( REG_OP_MODE, LORA_SLEEP );
	write_reg( REG_OP_MODE, LORA_STANDBY );

	//check that the module is in standby mode
	bootmode = read_reg( REG_OP_MODE );
	if( bootmode != LORA_STANDBY ) {
		printf( "Could not initialize LORA.\n" );
		exit( EXIT_FAILURE );
	}
	printf( "LORA on standby.\n" );
	//if all's good, set the payload size
	write_reg( REG_PAYLOAD_LEN, bufferSize - 1);
}

/*
 * Cleans the lora module.
 */
void lora_close() {
	write_reg( REG_OP_MODE, FSK_SLEEP );
}

/*
 * Reads from the specified register.
 * Returns the value in the register.
 */
uint8_t read_reg( uint8_t reg ){
  char tbuf[] = { reg , 0x00 };
  char rbuf[] = { 0x00, 0x00 };
  bcm2835_spi_transfernb( tbuf, rbuf, sizeof( tbuf ) );
  //fprintf( stdout, "Read value 0x%02X from register 0x%02X.\n", rbuf[1], reg );
  return rbuf[1];
}

/*
 * Writes to the specified register.
 * Returns the previous value in the register.
 */
uint8_t write_reg( uint8_t reg, char data ) {
  char tbuf[] = { reg | 0x80, data}; //flag reg MSB high to indicate write
  char rbuf[] = { 0x00, 0x00 };
  bcm2835_spi_transfernb( tbuf, rbuf, sizeof( tbuf ) );

  if( reg == REG_FIFO ){
    //printf( "Wrote '%c' to FIFO.\n", data );
  }
  else {
    //printf( "Wrote value 0x%02X to register 0x%02X.\n", data, reg );
  }
  return rbuf[1];
}
