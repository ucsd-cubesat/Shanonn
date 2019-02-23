#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "helper.h"
#include "adc.h"
#include "i2c.h"

//as volts
#define STEP_SIZE 0.000125003815f

int main( int argc, char **argv ) {

  // BCM Lib initialization
  if( !bcm2835_init() ) {
    fprintf( stderr, "bcm2835_init failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }

  i2c_init();

  ADC adc;

  ADC_init( &adc, GAIN_ONE );

  float adc0, adc1, adc2, adc3;

  while( 1 ) {

    adc0 = readADC_SingleEnded( &adc, 0 ) * STEP_SIZE;
    adc1 = readADC_SingleEnded( &adc, 1 ) * STEP_SIZE;
    adc2 = readADC_SingleEnded( &adc, 2 ) * STEP_SIZE;
    //adc3 = readADC_SingleEnded( &adc, 3 );

    printf( "AIN0: %f V\n", adc0 );
    printf( "AIN1: %f V\n", adc1 );
    printf( "AIN2: %f V\n\n", adc2 );
    //printf( "AIN3: %d\n", adc3 );

    time_delay( 1 );
  }

  i2c_close();
  bcm2835_close();
  return EXIT_SUCCESS;
}
