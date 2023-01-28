#include "GPS.h"
#include <thread>
#include <chrono>
#include <iostream>

void run(GPS &gps) {
	std::cout << gps.get() << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main() {
	GPS gps;
	std::thread t1 (&GPS::start, &gps);
	while (1) {
		std::cout << gps.get() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	t1.join();
	return 0;
}
