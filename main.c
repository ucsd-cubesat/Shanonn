/*
 * main.c
 *
 *  Created on: Feb 3, 2019
 *      Author: TritonCubed
 */

#include "spi.h"
#include "i2c.h"
#include "lora.h"
#include "gps.h"
#include "temp.h"
#include "adc.h"
#include "rs232.h"
#include "pressure.h"
#include <bcm2835.h>
#include "helper.h"
#include <stdlib.h>
#include <stdio.h>

#define PAYLOAD_SIZE 250
#define OZONE_PIN 0
#define BATTERY_PIN 2
#define SOLAR_PIN 1

#define BAUD 9600
#define MODE "8N1"
#define SERIAL_FILE "ttyACM0"


//payload
char payload[PAYLOAD_SIZE];

MS5803 pressureSensor;
ADC adc;
loc_t location;
int SERIAL_PORT;

//as volts
#define ADC_STEP_SIZE 0.000125f

/**
 * Helper function to construct payload.
 */
void buildPayload( ) {

  double pressure = 0;
  float temp1 = 0, temp2 = 0;
  float ozoneReading = 0, batteryReading = 0, solarReading = 0;
  payload[0] = 0;

  //from the GPS
  char timestamp[50];
  char gpsData[50];

  //from the pressure sensor
  char pressureData[50];

  //from the temperature sensors
  char tempData[50];

  //from the gieger meter
  //char geigerData[50];

  //from the ADC
  char ozoneData[50];
  char batteryData[50];
  char solarData[50];

  //read data
  getLocation( &location );

  pressure = ms5803_getPressure( &pressureSensor, ADC_256 );

  temp1 = get_temperature( TEMP_SENSOR_1 );
  temp2 = get_temperature( TEMP_SENSOR_2 );

  ozoneReading = readADC_SingleEnded( &adc, OZONE_PIN ) * ADC_STEP_SIZE;
  batteryReading = readADC_SingleEnded( &adc, BATTERY_PIN ) * ADC_STEP_SIZE;
  solarReading = readADC_SingleEnded( &adc, SOLAR_PIN ) * ADC_STEP_SIZE;

  //timestamp
  snprintf( timestamp, 50, "-- %02d:%02d:%02d UTC --",
                           location.hour,
                           location.minute,
                           location.second );

  //gps
  snprintf( gpsData, 50, "Lat: %lf\nLong: %lf\nAlt: %lf",
                         location.latitude,
                         location.longitude,
                         location.altitude );

  //pressure sensor
  snprintf( pressureData, 50, "Pressure: %3.2f mbar",
                              pressure );
  //temperature sensors
  snprintf( tempData, 50, "Temps: %2.2f C\n       %2.2f C",
                          temp1,
                          temp2 );

  //ozone sensor
  snprintf( ozoneData, 50, "Ozone: %1.4f V",
                           ozoneReading );

  //solar panels
  snprintf( solarData, 50, "Solar: %1.4f V",
                           solarReading );

  //solar panels
  snprintf( batteryData, 50, "Battery: %1.4f V",
                             batteryReading );

  //geiger meter
  int n = RS232_PollComport( SERIAL_PORT, geigerData, 50 );
  geigerData[n] = 0;

  //compile
  snprintf( payload, PAYLOAD_SIZE, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
                                   timestamp,
                                   gpsData,
                                   pressureData,
                                   tempData,
                                   ozoneData,
                                   solarData,
                                   batteryData,
                                   geigerData );
}

int main( int argc, char **argv ){


  // BCM Lib initialization
  if( !bcm2835_init() ) {
    printf( "bcm2835_init failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }

  i2c_init();  //setup i2c

  gps_init(); //setup gps
  ms5803_init( &pressureSensor, ADDRESS_HIGH ); //setup pressure sensor
  ADC_init( &adc, GAIN_ONE ); //setup ADC

  SERIAL_PORT = RS232_GetPortnr( SERIAL_FILE );
  if( RS232_OpenComport( SERIAL_PORT, BAUD, MODE ) ) {
    printf( "RS232 failed to open\n" );
    exit( EXIT_FAILURE );
  }

  spi_init();  //setup spi
  lora_init( PAYLOAD_SIZE ); //boot sequence to LoRa standby

  printf( "Initialize complete\n" );

  //begin beaconing cycle
  while( 1 ) {

    //make the payload
    buildPayload( payload, PAYLOAD_SIZE );

    //set SPI access to fifo to the Tx base reg
    write_reg( REG_FIFO_ADDR_PTR, FIFO_TX_BASE_ADDR );

    //load payload into FIFO
    for( int k = 0; k < ( PAYLOAD_SIZE - 1 ); k++ ){
      write_reg( REG_FIFO, payload[k] );
    }

    //commence Tx
    write_reg( REG_OP_MODE, LORA_TX );

    //delay before confirm
    bcm2835_delay( 500 );

    //confirm Tx
    if( read_reg( REG_IRQ_FLAGS ) == FLAG_TX_DONE
    		&& read_reg(REG_OP_MODE) == LORA_STANDBY ) {
      write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
      printf( "%s", payload );
    }
    else if( read_reg( REG_OP_MODE ) != LORA_STANDBY ){
      //reinit lora module
      lora_init( PAYLOAD_SIZE );
    }
  }

  //close all resources
  gps_close();
  lora_close();
  i2c_close();
  spi_close();
  bcm2835_close();
  RS232_CloseComport( SERIAL_PORT );

  return EXIT_SUCCESS;
}
