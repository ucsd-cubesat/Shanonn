/*
 * gps.c
 *
 *  Created on: Feb 16, 2019
 *      Author: TritionCubed
 *
 *  The GPS module for the payload.
 *  Gives access to the UPUTRONICS breakout for MAX-M8C GPS
 *
 *  Getting a serial interface to work on RPi v3 was very complicated.
 *  To get a working Serial port on /dev/ttyS0, use these commands:
 *
 *  $ sudo -s
 *  $ echo "enable_uart=1" >> /boot/config.txt
 *  $ systemctl stop serial-getty@ttyS0.service
 *  $ systemctl disable serial-getty@ttyS0.service
 *
 *  You can see the output using
 *  $ screen /dev/ttyS0
 *
 *  The breakout uses the following pins:
 *
 *  RPI             Chip
 *  --------------------
 *  Pin  1 (3v3)     VCC
 *  Pin  6 (GND)     GND
 *  Pin  5 (SCL)     SCL
 *  Pin  3 (SDA)     SDA
 */

#include "gps.h"
#include "nmea.h"
#include <bcm2835.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Stub
 */
void gps_init() {
}

/*
 * Stub
 */
void gps_close() {
}

/*
 * Saves location data to the given
 * location struct.
 */
void getLocation( loc_t *coord ) {

  char buff[100]; //buffer for processing
  uint8_t status = _EMPTY; //status indicator
  gpgga_t gpgga; //GPGGA data
  gprmc_t gprmc; //GPRMC data
  int i;

  while( status != _COMPLETED ) {

	//reads a line of gps data
  bcm2835_i2c_setSlaveAddress( GPS_ADDRESS );
  //bcm2835_delay( 10 );

  buff[0] = 0;

  while( buff[0] != '$' ) {
    bcm2835_i2c_read( buff, 1 );
  }

  i = 1;
  while( i < 100 ) {
    bcm2835_i2c_read( &(buff[i]), 1 );
    if( buff[i] == 0xFF ) {
      continue;
    }
    else if( buff[i] == '\n' ) {
      buff[i] = 0;
      break;
    }
    else {
      i++;
    }
  }
  buff[99] = 0;

  //keep parsing location/velocity data until we have both from the same frame
	switch( nmea_get_message_type( buff ) ) {
	  case NMEA_GPGGA:
	    nmea_parse_gpgga( buff, &gpgga );

	    gps_convert_deg_to_dec( &(gpgga.latitude), gpgga.lat, &(gpgga.longitude), gpgga.lon );

	    coord->latitude = gpgga.latitude;
	    coord->longitude = gpgga.longitude;
	    coord->altitude = gpgga.altitude;

	    status |= NMEA_GPGGA;
	    break;
	  case NMEA_GPRMC:
	  	nmea_parse_gprmc( buff, &gprmc );

		  coord->speed = gprmc.speed;
		  coord->course = gprmc.course;
		  coord->hour = gprmc.hour;
		  coord->minute = gprmc.minute;
		  coord->second = gprmc.second;

		  status |= NMEA_GPRMC;
		  break;
    }
  }
}

/*
 * Convert lat e lon to decimals (from deg)
 */
void gps_convert_deg_to_dec( double *latitude, char ns,  double *longitude, char we ) {
  double lat = ( ns == 'N' ) ? *latitude : -1 * (*latitude);
  double lon = ( we == 'E' ) ? *longitude : -1 * (*longitude);

  *latitude = gps_deg_dec( lat );
  *longitude = gps_deg_dec( lon );
}

double gps_deg_dec( double deg_point ) {
  double ddeg;
  double sec = modf( deg_point, &ddeg ) * 60;
  int deg = (int)( ddeg / 100 );
  int min = (int)( deg_point - ( deg * 100 ) );

  double absdlat = round( deg * 1000000. );
  double absmlat = round( min * 1000000. );
  double absslat = round( sec * 1000000. );

  return round( absdlat + ( absmlat / 60 ) + ( absslat / 3600) ) / 1000000;
}
