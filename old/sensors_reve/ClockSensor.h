#pragma once
#ifndef CLOCKSENSOR_H
#define CLOCKSENSOR_H

#include <string>
#include <chrono>

class ClockSensor : public BaseSensor {
private:
	std::chrono::high_resolution_clock::time_point initialTime;
public:
	ClockSensor() {
		setHeader("Time(s),");
		initialTime = std::chrono::high_resolution_clock::now();
	}
	double getTime() {
		std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> time_chrono = current - initialTime;
		return time_chrono.count() / 1000000;
	}

	void start() {
		while (1) {
			set(std::to_string(getTime()) + ",");
		}
	}
};

#endif
