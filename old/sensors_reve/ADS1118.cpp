#include "SPIBus.h"
#include "ADS1118.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
	SPIBus* bus = new SPIBus;
	ADS1118 ads(bus);
	ads.addChannel(TWO_THIRDS, EIGHT_HUNDRED_SIXTY, A0_A1, "milliVolts(mV)");
	std::thread t1 (&ADS1118::start, &ads);

	while (1) {
		std::cout << ads.get() << std::endl;
		std::this_thread::sleep_for(std::chrono::microseconds(100000));
	}

	return 0;
}
