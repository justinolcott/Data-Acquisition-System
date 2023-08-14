#include "UARTDevice.h"
#include "Sensors.h"
#include "NMEA.h"
#include <string>

#include <thread>
#include <chrono>

#include <mutex>

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
#include <iostream>
class GPS {
private:
	std::mutex mx;
	SerialDevice* serial;
	std::string data;
public:
	GPS() {
		//SerialDevice serial0(0, 9600);
       		//serial0.send(PMTK_SET_NMEA_BAUD_38400);
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//serial0.closeUART();
		//std::this_thread::sleep_for(std::chrono::milliseconds(10000));
		serial = new SerialDevice(0, 9600);
		serial->send(PMTK_SET_NMEA_BAUD_38400);
		delete serial;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		serial = new SerialDevice(0, 38400);
        	serial->send(PMTK_SET_NMEA_UPDATE_1_10_SEC);
        	serial->send("$PMTK300,100,0,0,0,0*2\r\n");
        	serial->send(PMTK_SET_NMEA_OUTPUT_RMCGGA);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		serial->flush();
	}

	~GPS() {
		delete serial;
	}


	std::string get() {
		std::lock_guard<std::mutex> lk(mx);
		return data;
	}

	void set(std::string input) {
		std::lock_guard<std::mutex> lk(mx);
		data = input;
	}

	void run() {
		while (1) {
                        serial->flush();
                        std::string line1 = serial->getLine();
                        std::string line2 = serial->getLine();
                        std::cout << custom(line1, line2) << std::endl;
                }
	}

	void start() {
		while (1) {
			serial->flush();
			std::string line1 = serial->getLine();
			std::string line2 = serial->getLine();
			set(custom(line1, line2));
		}
	}

};

