/*
 * gpstest.c
 *
 *  Created on: Feb 16, 2019
 *      Author: TritonCubed
 */

#include <stdio.h>
#include <stdlib.h>
#include "gps.h"
#include "i2c.h"
#include <bcm2835.h>

int main() {

  printf( "Starting GPS Test...\n" );

  // BCM Lib initialization
    if( !bcm2835_init() ) {
      fprintf( stderr, "bcm2835_init failed.  Must run as root.\n" );
      exit( EXIT_FAILURE );
    }

    i2c_init();

  // Open the communication
  gps_init();

  loc_t data;

  while( 1 ) {
    getLocation( &data );

    // You can extends adding data.speed, data.altitude, data.course (the direction)
    //printf( "%lf %lf @ %d:%d:%d UTC\n", data.latitude, data.longitude, data.hour, data.minute, data.second );
    printf( "%lf %lf @ %d:%d:%d UTC\n", data.latitude, data.longitude, data.hour, data.minute, data.second );
  }
  gps_close();
  i2c_close();
  bcm2835_close();
  return EXIT_SUCCESS;
}
