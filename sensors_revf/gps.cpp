#include "GPS.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "MPU6050.h"
#include "MyException.h"
#include "ADS1115.h"
#include "I2CBus.h"
int main() {
	GPS gps;
	std::thread t1 (&GPS::start, &gps);
	while (1) {
		try {
			std::cout << gps.get();
			std::cout << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		catch(MyException& e) {
			std::cout << "nothing to see here" << std::endl;
		}
	}
	t1.join();
	return 0;
}
