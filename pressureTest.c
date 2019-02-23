#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "helper.h"
#include "pressure.h"
#include "i2c.h"

// Thanks to Mike Grusin for letting me borrow the functions below from
// the BMP180 example code.
 double sealevel(double P, double A) {
  // Given a pressure P (mbar) taken at a specific altitude (meters),
  // return the equivalent pressure (mbar) at sea level.
  // This produces pressure readings that can be used for weather measurements.

  return(P/pow(1-(A/44330.0),5.255));
}

double altitude(double P, double P0) {
  // Given a pressure measurement P (mbar) and the pressure at a baseline P0 (mbar),
  // return altitude (meters) above baseline.

  return(44330.0*(1-pow(P/P0,1/5.255)));
}

int main( int argc, char **argv ) {

  // BCM Lib initialization
  if( !bcm2835_init() ) {
    fprintf( stderr, "bcm2835_init failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }

  i2c_init();

  MS5803 sensor;

  ms5803_init( &sensor, ADDRESS_HIGH );

  //Create variables to store results
  float temperature_c, temperature_f;
  double pressure_abs, pressure_relative, altitude_delta, pressure_baseline;

  // Create Variable to store altitude in (m) for calculations;
  double base_altitude = 81.0;  // Altitude of meeting room (m)

  pressure_baseline = ms5803_getPressure( &sensor, ADC_4096 );

  while( 1 ) {

    // Read pressure from the sensor in mbar.
    temperature_c = ms5803_getTemperature( &sensor, CELSIUS, ADC_512 );
    temperature_f = ms5803_getTemperature( &sensor, FAHRENHEIT, ADC_512 );
    pressure_abs = ms5803_getPressure( &sensor, ADC_4096 );
    // Let's do something interesting with our data.

    // Convert abs pressure with the help of altitude into relative pressure
    // This is used in Weather stations.
    pressure_relative = sealevel( pressure_abs, base_altitude );
    altitude_delta = altitude( pressure_abs , pressure_baseline );

    printf( "Temperature (C) = %f\n", temperature_c );

    printf( "Temperature (F) = %f\n", temperature_f );

    printf( "Pressure abs (mbar) = %f\n", pressure_abs );

    printf( "Pressure relative (mbar) = %f\n", pressure_relative );

    printf( "Altitude change (m) = %f\n", altitude_delta );

    time_delay( 1 );
  }

  i2c_close();
  bcm2835_close();
  return EXIT_SUCCESS;
}
