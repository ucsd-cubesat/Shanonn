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
 */

/*
 * Opens a UART session.
 * By default, all transfers are non-blocking (flag O_NDELAY)
 * This process will not lock the serial file (flag O_NOCTTY)
 * The file is opened as Read/Write (flag O_RDWR)
 *
 * Returns the file handle.
 */
int uart_init();

/*
 * Reads a line from the file to the character buffer,
 * up to len bytes long.
 */
void uart_readln( int file, char* buff, int len );

/*
 * Ends the UART session.
 */
void uart_close( int file );
