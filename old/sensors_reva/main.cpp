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
	string file;
	if (argc >= 2) {
		microseconds = atof(argv[1]);
	}
	else {
		microseconds = 0;
	}
	if (argc == 3) {
		file = "/media/pi/6000-91CD/";
		file.append(argv[2]);
	}
	else {
		file = "data.csv";
	}

	//Create sensors
	MPU6050* sensor = new MPU6050();

	SensorArray sensors;
	sensors.addSensor(sensor);

	sensors.run(file, microseconds);

	return 0;
}
