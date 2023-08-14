#pragma once
#ifndef GPS_H
#define GPS_H

/*
	Class for GPS sensor specifically MTK3339
		sold by Adafruit; however, this would most likely
		work with all NMEA / PMTK devices

		Uses UART for communication, so it does not need
		a bus passed into it

		Has mutex which prevents a data race when ran
		over a thread
*/

#include <thread>
#include <chrono>
#include <string>

#include "UARTDevice.h"
#include "BaseSensor.h"
#include "NMEA.h"

/* ========== Baud rate ========== */
#define PMTK_SET_NMEA_BAUD_4800 "$PMTK251,4800*14"
#define PMTK_SET_NMEA_BAUD_9600 "$PMTK251,9600*17"
#define PMTK_SET_NMEA_BAUD_38400 "$PMTK251,38400*27\r\n"
#define PMTK_SET_NMEA_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_NMEA_BAUD_115200 "$PMTK251,115200*1F"

/* ========== Update rate ========== */
#define PMTK_SET_NMEA_UPDATE_10_SEC  "$PMTK220,10000*2F" // Every 10 seconds
#define PMTK_SET_NMEA_UPDATE_5_SEC  "$PMTK220,5000*1B"  // Every 5 seconds
#define PMTK_SET_NMEA_UPDATE_1_10_SEC  "$PMTK220,100*2F\r\n" // Every tenth second

/* ========== System ========== */
#define PMTK_CMD_HOT_START "$PMTK101*32" // Reboot
#define PMTK_CMD_STANDBY_MODE "$PMTK161,0*28" // Standby

/* ========== Output format ========== */
/* Enable Recommended Minimum Sentence and GPS Fix data ( RMC and GGA )*/
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
#define PMTK_SET_NMEA_OUTPUT_CUSTOM "$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C"

class GPS : public BaseSensor {
private:
	UARTDevice* uart;
public:
	GPS(int bus = 0) {
		setHeader("Time(UTC),Lattitude,Longitude,Altitude(m),Speed(kn),Bearing(degrees),");
		//Sets Baud to 38400 when GPS is started
		uart = new UARTDevice(bus, 9600);
		uart->send(PMTK_SET_NMEA_BAUD_38400);
		delete uart;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		//Now baud is 38400 which is enough for 1/10 second updates
		uart = new UARTDevice(bus, 38400);
        uart->send(PMTK_SET_NMEA_UPDATE_1_10_SEC);
        uart->send("$PMTK300,100,0,0,0,0*2\r\n");
        uart->send(PMTK_SET_NMEA_OUTPUT_RMCGGA);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		uart->flush();
	}

	~GPS() {
		delete uart;
	}

	void start() {
		while (1) {
			uart->flush();
			std::string line1 = uart->getLine();
			std::string line2 = uart->getLine();
			set(custom(line1, line2));
		}
	}
};
#endif