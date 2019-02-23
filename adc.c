/*
 * adc.c
 *
 *  Created on: Feb 21, 2019
 *      Author: TritonCubed
 *
 *  ADS1115 Adafruit breakout lib ported from
 *  the Arduino version. Can use several optimizations,
 *  but why bother?
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

#include "adc.h"
#include "helper.h"
#include <bcm2835.h>
#include <stdio.h>

/*
 * writes to the ADC register
 */
void writeRegister( uint8_t i2cAddress, uint8_t reg, uint16_t value ) {
  char buff[3];

  buff[0] = (uint8_t)reg;
  buff[1] = (uint8_t)(value >> 8) & 0xFF;
  buff[2] = (uint8_t)(value & 0xFF);

  bcm2835_i2c_setSlaveAddress( i2cAddress );
  bcm2835_i2c_write( buff, 3 );
}

/*
 * reads from the ADC register
 */
uint16_t readRegister( uint8_t i2cAddress, uint8_t reg ) {
  char buff[2];

  buff[0] = reg;

  bcm2835_i2c_setSlaveAddress( i2cAddress );
  bcm2835_i2c_write_read_rs( buff, 1, buff, 2 );

  return buff[0] << 8 | buff[1];
}

/*
 * Initializes the ADC. Make sure I2C is running.
 */
void ADC_init( ADC* adc, adsGain_t gain ) {
  adc->m_i2cAddress = ADS1015_ADDRESS;
  adc->m_conversionDelay = ADS1115_CONVERSIONDELAY;
  adc->m_bitShift = 0;
  adc->m_gain = gain; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

/*
 * Gets a single-ended ADC reading from the specified channel
 */
uint16_t readADC_SingleEnded( ADC* adc, uint8_t channel ) {
  if( channel > 3 ) {
    return 0;
  }

  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |  // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD |  // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS |  // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= adc->m_gain;

  // Set single-ended input channel
  switch( channel ){
    case ( 0 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case ( 1 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case ( 2 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case ( 3 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config );

  // Wait for the conversion to complete
  bcm2835_delay( adc->m_conversionDelay );

  // Read the conversion results
  // Shift 12-bit results right 4 bits for the ADS1015
  return readRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONVERT ) >> adc->m_bitShift;
}

/*
 * Reads the conversion results, measuring the voltage
 * difference between the P (AIN0) and N (AIN1) input.  Generates
 *  a signed value since the difference can be either
 * positive or negative.
 */
int16_t readADC_Differential_0_1( ADC* adc ) {
  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |  // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD |  // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS |  // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= adc->m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1;          // AIN0 = P, AIN1 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config );

  // Wait for the conversion to complete
  bcm2835_delay( adc->m_conversionDelay );

  // Read the conversion results
  uint16_t res = readRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONVERT ) >> adc->m_bitShift;
  if( adc->m_bitShift == 0 ) {
    return (int16_t) res;
  }
  else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if( res > 0x07FF ) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t) res;
  }
}

/*
 * Reads the conversion results, measuring the voltage
 * difference between the P (AIN2) and N (AIN3) input.  Generates
 * a signed value since the difference can be either
 * positive or negative.
 */
int16_t readADC_Differential_2_3( ADC* adc ) {
  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |  // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD |  // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS |  // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= adc->m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3;          // AIN2 = P, AIN3 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config );

  // Wait for the conversion to complete
  bcm2835_delay( adc->m_conversionDelay );

  // Read the conversion results
  uint16_t res = readRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONVERT ) >> adc->m_bitShift;
  if( adc->m_bitShift == 0 ) {
    return (int16_t) res;
  }
  else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if( res > 0x07FF ) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t) res;
  }
}

/*
 * Sets up the comparator to operate in basic mode, causing the
 * ALERT/RDY pin to assert (go from high to low) when the ADC
 * value exceeds the specified threshold.
 *
 * This will also set the ADC in continuous conversion mode.
 */
void startComparator_SingleEnded( ADC* adc, uint8_t channel, int16_t threshold ) {
  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_1CONV |  // Comparator enabled and asserts on 1 match
                    ADS1015_REG_CONFIG_CLAT_LATCH |  // Latching mode
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD |  // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS |  // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_CONTIN |  // Continuous conversion mode
                    ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  // Set PGA/voltage range
  config |= adc->m_gain;

  // Set single-ended input channel
  switch( channel ){
    case ( 0 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case ( 1 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case ( 2 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case ( 3 ):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set the high threshold register
  // Shift 12-bit results left 4 bits for the ADS1015
  writeRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_HITHRESH,
                 threshold << adc->m_bitShift );

  // Write config register to the ADC
  writeRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config );
}

/*
 * In order to clear the comparator, we need to read the
 * conversion results.  This function reads the last conversion
 * results without changing the config value.
 */
int16_t getLastConversionResults( ADC* adc ) {
  // Wait for the conversion to complete
  bcm2835_delay( adc->m_conversionDelay );

  // Read the conversion results
  uint16_t res = readRegister( adc->m_i2cAddress, ADS1015_REG_POINTER_CONVERT ) >> adc->m_bitShift;
  if( adc->m_bitShift == 0 ) {
    return (int16_t) res;
  }
  else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if( res > 0x07FF ) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t) res;
  }
}

/*
 * Sets the gain and input voltage range
 */
void setGain( ADC* adc, adsGain_t gain ) {
  adc->m_gain = gain;
}

/*
 * Gets the gain and input voltage range
 */
adsGain_t getGain( ADC* adc ) {
  return adc->m_gain;
}
