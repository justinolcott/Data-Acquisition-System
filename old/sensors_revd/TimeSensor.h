#pragma once
#ifndef TIMESENSOR_H
#define TIMESENSOR_H

/*
	Class for a time
		It is derived from BaseSensor.h, so it
		is compatible with the other sensors; however,
		TimeSensor::start starts an internal clock
			If a list of sensors are all started, there
			will be a delay between starting and outputing,
			so the output won't necessarily start at zero, 
			but it will be relatively accurate with the 
			other sensors
*/

#include "BaseSensor.h"
#include <chrono>
#include <string>

class TimeSensor : public BaseSensor {
private:
	std::chrono::high_resolution_clock::time_point initialTime;
public:
	TimeSensor() {
		setHeader("Time(s),");
	}
	void start() {
		initialTime = std::chrono::high_resolution_clock::now();
	}
	double getTime() {
		std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> time_chrono = current - initialTime;
		return time_chrono.count() / 1000000;
	}
	std::string get() {
		return std::to_string(getTime()) + ",";
	}
};
#endif



