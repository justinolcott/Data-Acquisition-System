#pragma once
#ifndef TESTING_H
#define TESTING_H

#include <thread>
#include <chrono>
//#include <atomic> //dont need this one
//#include <ctime> //mayybe dont need this one

#include "Sensors.h"


#define ITERATIONS 100

class testing {
private:
	SensorArray* sensors;
	long double time2Test;
	//needs to be fixed or removed
	long double time4testing(int iterations) {
		long double sumTime = 0;
		clock_t start; //here
		clock_t end;
		double data[iterations][sensors->size()];
		for (int i = 0; i < iterations; i++) {
			start = clock();
			sensors->getValues(data[i]);
			end = clock();
			sumTime += (end - start) / (long double) CLOCKS_PER_SEC;
		}
		return sumTime / iterations;
	}
public:
	testing(SensorArray* sensors) : sensors(sensors) {
		time2Test = time4testing(ITERATIONS);
	}

	testing() {}

	void setSensors(SensorArray* sensors) {
		this->sensors = sensors;
	}

	//I dont think this is being used
	void setTime(int iterations, void (*f)(SensorArray*)) {
		long double sumTime = 0;
		for (int i = 0; i < iterations; i++) {
			auto start = std::chrono::high_resolution_clock::now();
			(*f)(sensors);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<long double, std::micro> timeChrono = end - start;
			long double time = timeChrono.count();
			sumTime += time;
		}
		time2Test = sumTime / ((long double)iterations);
	}

	//I dont think this is being used either
	void setTimeAvg(int iterations, double** data, void (*f)(SensorArray*, double**)) {
		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < iterations; i++) {
			(*f)(sensors, data);
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<long double, std::micro> timeChrono = end - start;
 		long double time = timeChrono.count();
		time2Test = time / ((long double)iterations);
	}

	//Returns time to run in microseconds
	long double microseconds(int iterations, double** data, void (*f)(SensorArray*, double**), long double sleep) {
		auto start = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < iterations; i++) {
                        (*f)(sensors, data);
			this_thread::sleep_for(std::chrono::microseconds((long int)sleep));
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<long double, std::micro> timeChrono = end - start;
                return timeChrono.count();
	}


	//Returns time to sleep in microseconds
	long double time2sleep(long double hertz, int iterations, double** data, void (*f)(SensorArray*, double**), long double sleep) {
		long double rate = (long double)iterations * 1000000 / (microseconds(iterations, data, (*f), sleep));
		long double errorPercent = (rate - hertz) / hertz;
		cout << "Rate was " << rate << " and the errorPercent was " << errorPercent << " and sleep is " << sleep << endl;
		if (errorPercent <= .005 && errorPercent >= -.005) {
			return sleep;
		}
		else if (sleep < 100) {
			return 0.0;
		}
		else {
			return time2sleep(hertz, iterations, data, (*f), sleep + (sleep * errorPercent));
		}
	}
};
#endif
