/*
 * mqtest.c
 *
 *  Created on: Feb 9, 2019
 *      Author: TritonCubed
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "rs232.h"

#define BUF_SIZE 128

/*
 * Helper function to delay execution by the given duration in seconds.
 */
void time_delay( double duration ) {
  clock_t timer = 0;
  clock_t start = clock();
  while( ( timer / CLOCKS_PER_SEC ) <= duration ) {
    timer = clock() - start;
  }
}

int main( int argc, char **argv ) {
  
  unsigned char res;
  unsigned char str_recv[BUF_SIZE];
  int cport_n = RS232_GetPortnr( "ttyACM0" );
  int bdrate = 9600;
  char mode[] = {'8', 'N', '1', 0};

  if( RS232_OpenComport( cport_n, bdrate, mode ) ) {
    fprintf( stdout, "RS232 failed to open\n" );
    exit( EXIT_FAILURE );
  }

  //begin  testing cycle
  while( 1 ) {
          time_delay( 0.25 );
	  int n = RS232_PollComport( cport_n, str_recv, BUF_SIZE );
	  str_recv[n] = 0;
	  fprintf( stdout, "Got %d bytes: %s\n", n, str_recv );
  }

  RS232_CloseComport( cport_n );

  return EXIT_SUCCESS;
}

