/*
 * i2c.h
 *
 *  Created on: Feb 18, 2019
 *      Author: TritonCubed
 *
 *  The protocol uses the following pins:
 *
 *  RPI
 *  ------------
 *  Pin  5 (SCL)
 *  Pin  3 (SDA)
 */

/*
 * Configures I2C with the proper bitrate
 */
void i2c_init();

/*
 * Closes the I2C session.
 */
void i2c_close();
