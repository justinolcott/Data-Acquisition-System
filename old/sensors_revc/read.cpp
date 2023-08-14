#include "UARTDevice.h"

#define PMTK_SET_NMEA_BAUD_4800 "$PMTK251,4800*14\r\n"
#define PMTK_SET_NMEA_BAUD_9600 "$PMTK251,9600*17"
#define PMTK_SET_NMEA_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_NMEA_BAUD_115200 "$PMTK251,115200*2C\r\n"
/* ========== Update rate ========== */
#define PMTK_SET_NMEA_UPDATE_10_SEC  "$PMTK220,10000*2F" // Every 10 seconds
#define PMTK_SET_NMEA_UPDATE_5_SEC  "$PMTK220,5000*1B"  // Every 5 seconds
#define PMTK_SET_NMEA_UPDATE_1_10_SEC  "$PMTK220,100*1F\r\n" // Every tenth second



/* ========== System ========== */
#define PMTK_CMD_HOT_START "$PMTK101*32" // Reboot
#define PMTK_CMD_STANDBY_MODE "$PMTK161,0*28" // Standby


/* ========== Output format ========== */
/* Enable Recommended Minimum Sentence and GPS Fix data ( RMC and GGA )*/
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
#define PMTK_SET_NMEA_OUTPUT_CUSTOM "$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C"

int main() {
	SerialDevice serial(0, 9600);
	serial.send(PMTK_SET_NMEA_BAUD_9600);
	usleep(1000000);
	serial.send(PMTK_SET_NMEA_UPDATE_1_10_SEC);
	serial.send("$PMTK300,100,0,0,0,0*2\r\n");
	serial.send(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	usleep(1000000);
	serial.flush();
	while (1) {
//		serial.receive(response, 1);
//		cout << response[0];
//		serial.flush();
		cout << serial.getLine() << serial.getLine() << endl;
	}

	//cout << "All of response: " << response << endl;

	return 0;
}
