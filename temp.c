/*
 * temp.c
 *
 *  Created on: Feb 18, 2019
 *      Author: TritonCubed
 *
 *  Path of the sensor (temp 1)
 *  "/sys/bus/w1/devices/28-03119779ba5b/w1_slave"
 *
 *  The 1-Wire protocol uses the following pins:
 *
 *  RPI
 *  -------------
 *  Pin  7 (DATA)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include "temp.h"

/*
 * Gets the temperature in Celcius.
 */
float get_temperature( temp_sensor sensor ) {

  char buff[100];
  int tempFile;
  char *start;
  float value;

  char* path;

  switch( sensor ) {
    case TEMP_SENSOR_1:
      path = TEMP_1_PATH;
      break;
    case TEMP_SENSOR_2:
      path = TEMP_2_PATH;
      break;
    default:
      path = NULL;
  }

  // Open the file in the path.
  if( ( tempFile = open( path, O_RDONLY ) ) < 0 ) {
    printf( "open error\n" );
    exit( EXIT_FAILURE );
  }

  // Read the file
  if( read( tempFile, buff, 100 ) < 0 ) {
    printf( "read error\n" );
    exit( EXIT_FAILURE );
  }

  close( tempFile );

  // Returns the first index of 't'.
  start = strchr( buff, 't' );

  // Read the string following "t=". ERRONEUS
  sscanf( start, "t=%s", start );

  // atof: changes string to float.
  value = atof( start ) / 1000;

  return value;
}
