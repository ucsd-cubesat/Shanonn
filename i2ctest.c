#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

int main( int argc, char **argv ) {

  // BCM Lib initialization
  if( !bcm2835_init() ) {
    fprintf( stderr, "bcm2835_init failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }

  if( !bcm2835_i2c_begin() ) {
      fprintf( stderr, "bcm2835_i2c_begin failed.  Must run as root.\n" );
      exit( EXIT_FAILURE );
    }

  char buff;

  bcm2835_i2c_set_baudrate( 9600 );
  bcm2835_i2c_setSlaveAddress( 0x42 );

  while( 1 ) {
    bcm2835_i2c_read( &buff, 1 );
    printf( "%c", buff );
  }

  bcm2835_i2c_end();
  bcm2835_close();

  return EXIT_SUCCESS;
}
