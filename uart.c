/*
 * uart.c
 *
 *  Created on: Feb 16, 2019
 *      Author: TritonCubed
 *
 *  A UART implementation. rs232 doesn't wanna
 *  work with the GPS, so this is a work around.
 *  Everything is hard coded Baud 9600, 8N1
 *  This can be changed if desired. Look in termios.h
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
 *  The following pins...
 *   RPI
 *  ------------
 *  Pin  1 (3v3)
 *  Pin  6 (GND)
 *  Pin  8 (TXD)
 *  Pin 10 (RXD)
 *
 *  ...map to the file /dev/ttyS0
 *  This serial interface is BAUD 9600, 8 bit, one stop bit, no parity.
 *
 *  https://raspberrypi.stackexchange.com/questions/45570/how-do-i-make-serial-work-on-the-raspberry-pi3-pi3b-pizerow/45571#45571
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <inttypes.h>
#include <string.h>

#include "uart.h"
#include "helper.h"

/*
 * Opens a UART session.
 * By default, all transfers are non-blocking (flag O_NDELAY)
 * This process will not lock the serial file (flag O_NOCTTY)
 * The file is opened as Read-Write (flag O_RDWR)
 *
 * Returns the file handle.
 */
int uart_init() {

  int file;
  struct termios options;

  file = open( "/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY );

  if( file == -1 ) {
    printf( "Could not open UART\n" );
    exit( EXIT_FAILURE );
  }

  tcgetattr( file, &options );
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush( file, TCIFLUSH );
  tcsetattr( file, TCSANOW, &options );
  return file;
}

/*
 * Reads a line from the file to the character buffer,
 * up to len bytes long.
 */
void uart_readln( int file, char* buff, int len ) {
  char c;
  char *b = buff;
  int rx_length = -1;
  while( 1 ) {
    rx_length = read( file, (void*)(&c), 1 );
    if( rx_length <= 0 ) {
      time_delay( 0.5 );
    }
    else {
      if( c == '\n' ) {
        *b++ = '\0';
        break;
      }
      *b++ = c;
    }
  }
}

/*
 * Ends the UART session.
 */
void uart_close( int file ) {
  close( file );
}
