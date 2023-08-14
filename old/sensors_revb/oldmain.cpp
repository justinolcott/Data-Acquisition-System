#include "Sensors.h"
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

int main(int argc, char* argv[]) {

	//Take microseconds as input
	int microseconds;
	if (argc == 2) {
		microseconds = atof(argv[1]);
	}
	else {
		microseconds = 10;
	}

	//Create sensors
	MPU6050* sensor = new MPU6050();

	SensorArray sensors;
	sensors.addSensor(sensor);

	sensors.run("out.csv", microseconds);


//Create Dynamic 2D array
//	int height = 1;
//	int length = 6;
//	double** data = new double*[height];
//	for (int i = 0; i < height; i++) {
//		data[i] = new double[length];
//	}
/*
	//Start Testing and set hertz
	testing test;
	test.setSensors(&sensors);
	long int timeSleep = (unsigned long int)(test.time2sleep(hertz, 10, data, run, 7600));
	int SAMPLES = 10;

	//Get frequency
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < SAMPLES; i++) {
		sensors.getDatas(data);
		this_thread::sleep_for(std::chrono::microseconds(timeSleep));
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> time_chrono = end - start;
	double time = time_chrono.count() / 1000000;
	double frequency = SAMPLES / time;
	cout << "Sleep time is " << timeSleep / 1000000.0 << " Time is " << time << " Samples rate is " << frequency << endl;
*/
/*
	long int timeSleep = 1000000;
	//Output data
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 100; i++) {
		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> time_chrono = current - start;
		double time = time_chrono.count() / 1000000;
		cout << time << ", ";
		sensors.getDatas(data);
		output(cout, &sensors);
		//cout << data[0][0] << ", " << data[0][1] << ", " << data[0][2] << ", " << data[0][3] << ", " << data[0][4] << ", " << data[0][5] << ", " << endl;
		this_thread::sleep_for(std::chrono::microseconds(timeSleep));
	}
	//Deallocate memory for dyanamic 2D array
	for (int i = 0; i < height; i++) {
		delete [] data[i];
	}
	delete [] data;

*/
	return 0;
}
