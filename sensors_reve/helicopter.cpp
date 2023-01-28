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
#include "ClockSensor.h"

int main() {

	I2CBus* bus1 = new I2CBus(1);
	I2CBus* bus3 = new I2CBus(3);
	I2CBus* bus4 = new I2CBus(4);
	I2CBus* bus5 = new I2CBus(5);

	GPS gps;

	ADS1115 ads1(bus1, 0x48);
	ADS1115 ads2(bus1, 0x49);
	ADS1115 ads3(bus1, 0x4a);
	ADS1115 ads4(bus1, 0x4b);

	ADS1115 ads5(bus3, 0x48);
	ADS1115 ads6(bus3, 0x49);
	ADS1115 ads7(bus3, 0x4a);

	ADS1115 ads8(bus4, 0x48);
	ADS1115 ads9(bus4, 0x49);
	ADS1115 ads10(bus4, 0x4a);

	MPU6050 acc(bus5, 0x68);

	ads1.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 9");
	ads1.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 10");

	ads2.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 7");
	ads2.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 8");

	ads3.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 5");
	ads3.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 6");

	ads4.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 3");
	ads4.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 4");

	ads5.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 17");
	ads5.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 18");

	ads6.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 15");
	ads6.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 16");

	ads7.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 13");
	ads7.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 14");

	ads8.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 1");
	ads8.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 2");

	ads9.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A0_A1, "Channel 11");
	ads9.addChannel(SIXTEEN, ONE_HUNDRED_TWENTY_EIGHT, A2_A3, "Channel 12");

	ads10.addChannel(TWO_THIRDS, SIXTY_FOUR, A0_A1, "Channel 19");
	ClockSensor clock;

	BaseSensorVector sensors;
	sensors.add(&clock);
	sensors.add(&gps);
	sensors.add(&acc);
	sensors.add(&ads8);
	sensors.add(&ads4);
	sensors.add(&ads3);
	sensors.add(&ads2);
	sensors.add(&ads1);
	sensors.add(&ads9);
	sensors.add(&ads7);
	sensors.add(&ads6);
	sensors.add(&ads5);
	sensors.add(&ads10);

	sensors.start();

	std::fstream file;
	std::cout << getTime() << std::endl;
	file.open(getTime(), std::fstream::out);
	std::ostream& os = file;

	os << sensors.getHeaders() << std::endl;
	sensors.run(os, 16666);

	return 0;
}
