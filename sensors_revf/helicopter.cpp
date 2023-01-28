#include "ADS1115.h"
#include "GPS.h"
#include "MPU6050.h"
#include "I2CBus.h"
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include "BaseSensorVector.h"
#include "FileManip.h"

int main() {

//	std::this_thread::sleep_for(std::chrono::milliseconds(30000));
	I2CBus* bus1 = new I2CBus(1);
	I2CBus* bus3 = new I2CBus(3);
	I2CBus* bus4 = new I2CBus(4);
	I2CBus* bus5 = new I2CBus(5);

//	GPS gps;
	ADS1115 ads(bus1, 0x48);
//	ads.addChannel(SIXTEEN, EIGHT_HUNDRED_SIXTY, A0_A1, "strain 1");
	ads.addChannel(SIXTEEN, SIXTY_FOUR, A2_A3, "strain 2");
//	MPU6050 acc(bus5, 0x68);

//	std::cout << "Starting... " << std::endl;
//	std::cout.flush();
	BaseSensorVector sensors;
//	sensors.add(&gps);
//	sensors.add(&acc);
	sensors.add(&ads);
	sensors.start();

//	std::fstream file;
//	file.open(getTime(), std::fstream::out);
	std::ostream& os = std::cout;
//	std::thread t1 (&GPS::start, &gps);
//	std::thread t2 (&ADS1115::start, &ads);
//	std::thread t3 (&MPU6050::start, &acc);

	sensors.run(os, 1000000);

	return 0;
}
