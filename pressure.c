/*
 * pressure.c
 *
 *  Created on: Feb 17, 2019
 *      Author: TritonCubed
 *
 *  Pressure sensor lib! Adapted/Ported from
 *  The official library for arduino.
 *
 *  The breakout uses the following pins:
 *
 *  RPI             Chip
 *  --------------------
 *  Pin  1 (3v3)     VCC
 *  Pin  6 (GND)     GND
 *  Pin  3 (SDA)     SDA
 *  Pin  5 (SCL)     SCL
 */

#include "pressure.h"
#include "helper.h"
#include <bcm2835.h>
#include <math.h>
#include <stdio.h>

/*
 * Resets the sensor.
 */
void ms5803_reset( MS5803 *sensor ) {
  ms5803_sendCommand( sensor, CMD_RESET );
}

/*
 * Initializes the sensor
 */
void ms5803_init( MS5803 *sensor, ms5803_addr address ) {
  uint8_t i;
  uint8_t highByte, lowByte;

  sensor->_address = address;
  ms5803_reset( sensor );

  for( i = 0; i <= 7; i++ ) {
    ms5803_sendCommand( sensor, CMD_PROM + ( i * 2 ) );
    bcm2835_i2c_read( &highByte, 1 );
    bcm2835_i2c_read( &lowByte, 1 );
    sensor->coefficient[i] = ( highByte << 8 ) | lowByte;
  }
  
}

/*
 * Reads the sensor temperature
 */
float ms5803_getTemperature( MS5803 *sensor, temperature_units units, precision _precision ) {
  ms5803_getMeasurements( sensor, _precision );
  float temperature_reported;
  // If Fahrenheit is selected return the temperature converted to F
  if( units == FAHRENHEIT ) {
    temperature_reported = sensor->_temperature_actual / 100;
    temperature_reported = ( ( ( temperature_reported ) * 9 ) / 5 ) + 32;
    return temperature_reported;
  }

  // If Celsius is selected return the temperature converted to C
  else {
    temperature_reported = sensor->_temperature_actual / 100;
    return temperature_reported;
  }
}

/*
 * Reads the sensor pressure in Pa
 */
float ms5803_getPressure( MS5803 *sensor, precision _precision ) {
  ms5803_getMeasurements( sensor, _precision );
  float pressure_reported;
  pressure_reported = sensor->_pressure_actual;
  pressure_reported = pressure_reported / 10;
  return pressure_reported;
}

/*
 * Performs measurement function
 */
void ms5803_getMeasurements( MS5803 *sensor, precision _precision ) {
  //Retrieve ADC result
  int32_t temperature_raw = ms5803_getADCconversion( sensor, TEMPERATURE, _precision );
  int32_t pressure_raw = ms5803_getADCconversion( sensor, PRESSURE, _precision );

  //Create Variables for calculations
  int32_t temp_calc;
  int32_t pressure_calc;

  int32_t dT;

  //Now that we have a raw temperature, let's compute our actual.
  dT = temperature_raw - ( (int32_t)(sensor->coefficient[5]) << 8 );
  temp_calc = ( ( (int64_t)dT * (sensor->coefficient[6]) ) >> 23 ) + 2000;

  //Now we have our first order Temperature, let's calculate the second order.
  int64_t T2, OFF2, SENS2, OFF, SENS;  //working variables

  if( temp_calc < 2000 )
  // If temp_calc is below 20.0C
      {
    T2 = 3 * ( ( (int64_t)dT * dT ) >> 33 );
    OFF2 = 3 * ( ( temp_calc - 2000 ) * ( temp_calc - 2000 ) ) / 2;
    SENS2 = 5 * ( ( temp_calc - 2000 ) * ( temp_calc - 2000 ) ) / 8;

    if( temp_calc < -1500 )
    // If temp_calc is below -15.0C
        {
      OFF2 = OFF2 + 7 * ( ( temp_calc + 1500 ) * ( temp_calc + 1500 ) );
      SENS2 = SENS2 + 4 * ( ( temp_calc + 1500 ) * ( temp_calc + 1500 ) );
    }
  }
  else
  // If temp_calc is above 20.0C
  {
    T2 = 7 * ( (uint64_t) dT * dT ) / pow( 2, 37 );
    OFF2 = ( ( temp_calc - 2000 ) * ( temp_calc - 2000 ) ) / 16;
    SENS2 = 0;
  }

  // Now bring it all together to apply offsets

  OFF = ( ( int64_t )( sensor->coefficient[2] ) << 16 )
      + ( ( ( sensor->coefficient[4] * (int64_t)dT ) ) >> 7 );
  SENS = ( ( int64_t )( sensor->coefficient[1] ) << 15 )
      + ( ( ( sensor->coefficient[3] * (int64_t)dT ) ) >> 8 );

  temp_calc = temp_calc - T2;
  OFF = OFF - OFF2;
  SENS = SENS - SENS2;

  // Now lets calculate the pressure

  pressure_calc = ( ( ( SENS * pressure_raw ) / 2097152 ) - OFF ) / 32768;

  sensor->_temperature_actual = temp_calc;
  sensor->_pressure_actual = pressure_calc;  // 10;// pressure_calc;

}
/*
 * Sends a command across the wire.
 */
void ms5803_sendCommand( MS5803 *sensor, uint8_t command ) {
  bcm2835_i2c_setSlaveAddress( sensor->_address );
  bcm2835_i2c_write( &command, 1 );
}

/*
 * Retrieve ADC result
 */
uint32_t ms5803_getADCconversion( MS5803 *sensor, measurement _measurement,
                                  precision _precision ) {
  uint32_t result;
  uint8_t highByte = 0, midByte = 0, lowByte = 0;

  ms5803_sendCommand( sensor, CMD_ADC_CONV + _measurement + _precision );
  bcm2835_delay( 1 );
  switch( _precision ){
    case ADC_256:
      bcm2835_delay( 1 );
      break;
    case ADC_512:
      bcm2835_delay( 3 );
      break;
    case ADC_1024:
      bcm2835_delay( 4 );
      break;
    case ADC_2048:
      bcm2835_delay( 6 );
      break;
    case ADC_4096:
      bcm2835_delay( 10 );
      break;
  }

  ms5803_sendCommand( sensor, CMD_ADC_READ );
  bcm2835_i2c_read( (char*) &highByte, 1 );
  bcm2835_i2c_read( (char*) &midByte, 1 );
  bcm2835_i2c_read( (char*) &lowByte, 1 );

  result = ((uint32_t)highByte << 16) + ((uint32_t)midByte << 8) + lowByte;

  return result;

}
