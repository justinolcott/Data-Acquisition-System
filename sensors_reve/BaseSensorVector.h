#pragma once
#ifndef BASESENSORVECTOR_H
#define BASESENSORVECTOR_H

#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include "BaseSensor.h"

class BaseSensorVector {
private:
	std::vector<BaseSensor*> sensors;
	std::vector<std::thread> threads;

public:
	BaseSensorVector() {}
	~BaseSensorVector() {
		for (auto& thread : threads) {
			thread.join();
		}
	}

	void add(BaseSensor* sensor) {
		sensors.push_back(sensor);
	}

	void start() {
		for (auto& sensor : sensors) {
			threads.push_back(std::thread(&BaseSensor::start, sensor));
		}
	}

	std::string get() {
		std::string out;
		for (auto& sensor : sensors) {
			out += sensor->get();
		}
		return out;
	}

	void run(std::ostream& out, int microseconds) {
		while (1) {
			out << get() << std::endl;
			std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
		}
	}

	std::string getHeaders() {
		std::string out;
		for (auto& sensor : sensors) {
			out += sensor->getHeader();
		}
		return out;
	}

};


#endif
