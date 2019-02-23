/*
 * mq131.h
 *
 *  Created on: Feb 9, 2019
 *      Author: TritonCubed
 *
 *  For the MQ-131 Ozone sensor, we are using the following
 *  pins:
 *
 *  Pin 2 - 5V
 *  Pin 6 - Ground
 *  Pin 11 - Digital
 */

/*
 * Readies the mq131.
 */
void mq131_init();

/*
 * Closes the m131 port.
 */
void mq131_close();

/*
 * Read the sensor output.
 */
unsigned char mq131_read();
