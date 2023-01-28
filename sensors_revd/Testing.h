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

	//Returns time to run in microseconds
        long double time2run(int iterations, long double sleep) {
		sensors->start();
		string out;
                auto start = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < iterations; i++) {
                        out = sensors->toString();
                        this_thread::sleep_for(std::chrono::microseconds((long int)sleep));
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<long double, std::micro> timeChrono = end - start;
                return timeChrono.count();
        }


        //Returns time to sleep in microseconds
        long double sleepTime(long double hertz, int iterations, long double sleep) {
                long double rate = (long double)iterations * 1000000 / (time2run(iterations, sleep));
                long double errorPercent = (rate - hertz) / hertz;
                cout << "Rate was " << rate << " and the errorPercent was " << errorPercent << " and sleep is " << sleep << endl;
                if (errorPercent <= .005 && errorPercent >= -.005) {
                        return sleep;
                }
                else if (sleep < 100) {
                        return 0.0;
                }
                else {
                        return sleepTime(hertz, iterations, sleep + (sleep * errorPercent));
                }
        }

	double getFrequency() {
		sensors->start();
		string out;
		auto start = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < ITERATIONS; i++) {
                        out = sensors->toString();
                }
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::micro> timeChrono = end - start;
		double totalTime = timeChrono.count() / 1000000;
		return ITERATIONS / totalTime;
	}
};
#endif
