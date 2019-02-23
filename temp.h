/*
 * temp.h
 *
 *  Created on: Feb 18, 2019
 *      Author: TritonCubed
 *
 *  Path of the sensors, based on serial numbers, so they are static.
 *  "/sys/bus/w1/devices/28-03119779ba5b/w1_slave"
 *  "/sys/bus/w1/devices/28-0311977963e7/w1_slave"
 *
 *  The 1-Wire protocol uses the following pins:
 *
 *  RPI
 *  -------------
 *  Pin  7 (DATA)
 */

typedef enum {
  TEMP_SENSOR_1,
  TEMP_SENSOR_2
} temp_sensor;

#define TEMP_2_PATH "/sys/bus/w1/devices/28-03119779ba5b/w1_slave"
#define TEMP_1_PATH "/sys/bus/w1/devices/28-0311977963e7/w1_slave"

/*
 * Gets the temperature in Celcius.
 */
float get_temperature();
