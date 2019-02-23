/*
 * gps.c
 *
 *  Created on: Feb 16, 2019
 *      Author: TritionCubed
 *
 *  The GPS module for the payload.
 *  Gives access to the UPUTRONICS breakout for MAX-M8C GPS
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

//the I2C address
#define GPS_ADDRESS 0x42

typedef struct {
  double latitude;
  double longitude;
  double altitude;
  double speed;
  double course;
  int hour;
  int minute;
  int second;
} loc_t;

/*
 * Stub
 */
void gps_init();

/*
 * Stub
 */
void gps_close();

/*
 * Saves location data to the given
 * location struct.
 */
void getLocation( loc_t *coord );

/*
 * Convert lat e lon to decimals (from deg)
 */
void gps_convert_deg_to_dec( double *latitude, char ns,  double *longitude, char we );

double gps_deg_dec( double deg_point );
