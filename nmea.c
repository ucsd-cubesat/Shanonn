/*
 * nmea.c
 *
 *  Created on: Feb 16, 2019
 *      Author: TritionCubed
 *
 * An NMEA parsing lib, found on github.
 * http://aprs.gids.nl/nmea/#rmc for the formats
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nmea.h"

/*
 * Parses a GPGGA sentence (GNPPA on the GPS) from the buffer
 * to the struct that represents it.
 */
void nmea_parse_gpgga( char *nmea, gpgga_t *loc ) {
  char *p = nmea;

  p = strchr( p, ',' ) + 1; //skip time

  //parse latitude
  p = strchr( p, ',' ) + 1;
  loc->latitude = atof( p );

  //parse direciton
  p = strchr( p, ',' ) + 1;
  switch( p[0] ) {
    case 'N':
      loc->lat = 'N';
      break;
    case 'S':
      loc->lat = 'S';
      break;
    case ',':
      loc->lat = '\0';
      break;
  }

  //parse longitude
  p = strchr( p, ',' ) + 1;
  loc->longitude = atof( p );

  //parse direction
  p = strchr( p, ',' ) + 1;
  switch( p[0] ) {
    case 'W':
      loc->lon = 'W';
      break;
    case 'E':
      loc->lon = 'E';
      break;
    case ',':
      loc->lon = '\0';
      break;
  }

  //prase signal quality
  p = strchr( p, ',' ) + 1;
  loc->quality = (uint8_t) atoi( p );

  //parse satellite list
  p = strchr( p, ',' ) + 1;
  loc->satellites = (uint8_t) atoi( p );

  //skip accuracy
  p = strchr( p, ',' ) + 1;

  //parse altitude
  p = strchr( p, ',' ) + 1;
  loc->altitude = atof( p );

  //implicit skip rest
}

/*
 * Parses a GPRMC sentence (GNRMC on the GPS) from the buffer
 * to the struct that represents it.
 */
void nmea_parse_gprmc( char *nmea, gprmc_t *loc ) {
  char *p = nmea;

  //parse time (UTC)
  p = strchr( p, ',' ) + 1;
  char time[3] = { 0, 0, 0 };
  char *res;

  //hour
  time[0] = p[0];
  time[1] = p[1];
  loc->hour = strtol( time, &res, 10 );

  //minute
  time[0] = p[2];
  time[1] = p[3];
  loc->minute = strtol( time, &res, 10 );

  //second
  time[0] = p[4];
  time[1] = p[5];
  loc->second = strtol( time, &res, 10 );

  p = strchr( p, ',' ) + 1; //skip status

  //parse latitude
  p = strchr( p, ',' ) + 1;
  loc->latitude = atof( p );

  //parse direction
  p = strchr( p, ',' ) + 1;
  switch( p[0] ) {
    case 'N':
      loc->lat = 'N';
      break;
    case 'S':
      loc->lat = 'S';
      break;
    case ',':
      loc->lat = '\0';
      break;
  }

  //parse longitude
  p = strchr( p, ',' ) + 1;
  loc->longitude = atof( p );

  //parse direction
  p = strchr( p, ',' ) + 1;
  switch( p[0] ) {
    case 'W':
      loc->lon = 'W';
      break;
    case 'E':
      loc->lon = 'E';
      break;
    case ',':
      loc->lon = '\0';
      break;
  }

  //parse speed
  p = strchr( p, ',' ) + 1;
  loc->speed = atof( p );

  //parse course
  p = strchr( p, ',' ) + 1;
  loc->course = atof( p );
}

/**
 * Get the message type (GPGGA, GPRMC, etc..)
 *
 * This function filters out also wrong packages ( bad sentence, invalid checksum)
 *
 * Returns an error if one occured, or the type.
 */
uint8_t nmea_get_message_type( const char *message ) {
  uint8_t checksum = 0;

  //check we're at the start of a sentence
  if( *message != '$' ) {
    return NMEA_UNKNOWN;
  }

  //check for validity
  if( ( checksum = nmea_valid_checksum( message ) ) != _EMPTY ) {
    return checksum;
  }

  if( strstr( message, NMEA_GPGGA_STR ) != NULL ) {
    return NMEA_GPGGA;
  }

  if( strstr( message, NMEA_GPRMC_STR ) != NULL ) {
    return NMEA_GPRMC;
  }
  return NMEA_UNKNOWN;
}

/*
 * Calculates the checksum of the NMEA sentence
 * Assume we are at the beginning of a valid string
 *
 * Returns _EMPTY on success, errors otherwise.
 */
uint8_t nmea_valid_checksum( const char *message ) {

  //find the checksum indicator
  char *offset = strchr( message, '*' );

  //assert it must be there
  if( offset == 0 ) {
    return NMEA_CHECKSUM_ERR;
  }

  //find the checksum value, after a * character
  uint8_t checksum = (uint8_t) strtol( offset + 1, 0, 16 );

  //at this point, we're somewhere before the null terminator
  //calculate checksum based on the sentence

  uint8_t sum = 0;
  message++; //skip over $

  //while we haven't reached the null terminator or the end
  //of a sentence, keep iterating
  while( *message != 0 && *message != '*' ) {
	//checksum does not use the delimiters ','
	if( *message != ',' ) {
	  sum ^= *message;
	}
    message++;
  }

  if( sum != checksum ) {
    return NMEA_CHECKSUM_ERR;
  }

  return _EMPTY;
}
