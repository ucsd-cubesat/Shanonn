#include <stdio.h>
#include <stdlib.h>
#include "temp.h"
#include "helper.h"
#include <time.h>

void main() {
  float temperature;
  time_t t0 = time(0);
  time_t t1;
  int runtime_diff;

  while( 1 ) {

    temperature = get_temperature( TEMP_SENSOR_1 );
    t1 = time(0);
    runtime_diff = difftime(t1, t0);
    printf( "Sensor 1: %f deg C, time: %d\n", temperature, runtime_diff );

    temperature = get_temperature( TEMP_SENSOR_2 );
    t1 = time(0);
    runtime_diff = difftime(t1, t0);
    printf( "Sensor 2: %f deg C, time: %d\n\n", temperature, runtime_diff );

    time_delay( 1 );
  }
}
