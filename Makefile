gpstest: i2c.c nmea.c gps.c gpstest.c
	gcc i2c.c nmea.c gps.c gpstest.c -lm -lbcm2835 -o gpstest
	sudo ./gpstest

pressureTest: i2c.c helper.c pressure.c pressureTest.c
	gcc i2c.c helper.c pressure.c pressureTest.c -lbcm2835 -lm -o pressureTest
	sudo ./pressureTest
	
adctest: i2c.c helper.c adc.c adctest.c
	gcc i2c.c helper.c adc.c adctest.c -lbcm2835 -o adctest
	sudo ./adctest

geigertest: rs232.c geigertest.c helper.c
	gcc rs232.c geigertest.c helper.c -o geigertest
	sudo ./geigertest
	
payload: adc.c temp.c pressure.c nmea.c gps.c helper.c main.c lora.c spi.c i2c.c rs232.c
	gcc adc.c temp.c pressure.c nmea.c gps.c helper.c main.c lora.c spi.c i2c.c rs232.c -lbcm2835 -lm -o payload
	sudo ./payload
	
temperaturetest: temperaturetest.c helper.c temp.c
	gcc temperaturetest.c helper.c temp.c -o temperaturetest
	sudo ./temperaturetest

clean:
	rm gpstest pressureTest geigertest i2ctest payload