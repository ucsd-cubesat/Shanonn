/*
 * loratest.c
 *
 *  Created on: Feb 17, 2019
 *      Author: TritonCubed
 */

#include "spi.h"
#include "lora.h"
#include "helper.h"
#include <stdlib.h>
#include <stdio.h>

//payload
char payload[25];

/**
 * Helper function to construct payload.
 */
void buildPayload( ) {
  //stub
  //memset( payload, 0, sizeof( payload ) );
  strcpy( payload, "Hello, world!" );
}

int main( int argc, char **argv ){


  // BCM Lib initialization
  if( !bcm2835_init() ) {
    fprintf( stderr, "bcm2835_init failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }

  spi_init();  //setup spi
  lora_init( sizeof( payload ) ); //boot sequence to LoRa standby

  //begin beaconing cycle
  while( 1 ) {

    //make the payload
    buildPayload( payload, sizeof( payload ) );

    //set SPI access to fifo to the Tx base reg
    write_reg( REG_FIFO_ADDR_PTR, FIFO_TX_BASE_ADDR );

    //load payload into FIFO
    for( int k = 0; k < ( sizeof( payload ) - 1 ); k++ ){
      write_reg( REG_FIFO, payload[k] );
    }

    //commence Tx
    write_reg( REG_OP_MODE, LORA_TX );

    //half second delay before confirm
    time_delay( 0.5 );

    //confirm Tx
    if( read_reg( REG_IRQ_FLAGS ) == FLAG_TX_DONE
        && read_reg(REG_OP_MODE) == LORA_STANDBY ) {
      write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
      fprintf( stdout, "Transmitted payload.");
    }
    else if( read_reg( REG_OP_MODE ) != LORA_STANDBY ){
      //reinit lora module
      lora_init( sizeof( payload ) );
    }

    //4.5 second delay for a full 5 second delay
    time_delay( 4.5 );
  }

  lora_close();
  spi_close();
  bcm2835_close();

  return EXIT_SUCCESS;
}
