/*
 * pressure.h
 *
 *  Created on: Feb 17, 2019
 *      Author: TritonCubed
 *
 *  Pressure sensor lib!
 *
 *  The breakout uses the following pins:
 *
 *  RPI             Chip
 *  --------------------
 *  Pin  1 (3v3)     3.3v
 *  Pin  6 (GND)     GND
 *  Pin  5 (SCL)     SCL
 *  Pin  3 (SDA)     SDA
 */

#include <stdint.h>

// Define units for conversions.
typedef enum {
  CELSIUS,
  FAHRENHEIT
} temperature_units;

// Define measurement type.
typedef enum {
  PRESSURE = 0x00,
  TEMPERATURE = 0x10
} measurement;

// Define constants for Conversion precision
typedef enum {
  ADC_256 = 0x00,
  ADC_512 = 0x02,
  ADC_1024 = 0x04,
  ADC_2048 = 0x06,
  ADC_4096 = 0x08
} precision;

// Define address choices for the device (I2C mode)
typedef enum {
  ADDRESS_HIGH = 0x76,
  ADDRESS_LOW = 0x77
} ms5803_addr;

//Commands
#define CMD_RESET 0x1E // reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command

#define CMD_PROM 0xA0 // Coefficient location

//saves the data for the sensor
typedef struct {
    int32_t _temperature_actual;
    int32_t _pressure_actual;
  int32_t _baseline_pressure;
    ms5803_addr _address;
    uint16_t coefficient[8];
} MS5803;

/*
 * Resets the sensor.
 */
void ms5803_reset( MS5803 *sensor );

/*
 * Initializes the sensor
 */
void ms5803_init( MS5803 *sensor, ms5803_addr address );

/*
 * Reads the sensor temperature
 */
float ms5803_getTemperature( MS5803 *sensor, temperature_units units, precision _precision );

/*
 * Reads the sensor pressure in Pa
 */
float ms5803_getPressure( MS5803 *sensor, precision _precision );

/*
 * Performs measurement function
 */
void ms5803_getMeasurements( MS5803 *sensor, precision _precision );

/*
 * Sends a command across the wire.
 */
void ms5803_sendCommand( MS5803 *sensor, uint8_t command );

/*
 * Retrieve ADC result
 */
uint32_t ms5803_getADCconversion( MS5803 *sensor, measurement _measurement,
                                  precision _precision );
