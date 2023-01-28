#include "Sensors.h"
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Testing.h"
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
//		file = "/media/pi/6000-91CD/";
		file.append(argv[2]);
	}
	else {
		file = "data.csv";
	}

	//Create sensors
	//MPU6050* sensor = new MPU6050();

	ADC* loadCell1 = new ADC(1, 0x48, 3, 7, 0, .00003125, 1);
        ADC* loadCell2 = new ADC(1, 0x48, 3, 7, 3, .00003125, 1);
        ADC* loadCell3 = new ADC(1, 0x49, 3, 7, 0, .00003125, 1);
        ADC* loadCell4 = new ADC(1, 0x49, 3, 7, 3, .00003125, 1);
        ADC* loadCell5 = new ADC(1, 0x4a, 3, 7, 0, .00003125, 1);
        ADC* loadCell6 = new ADC(1, 0x4a, 3, 7, 3, .00003125, 1);
        ADC* current = new ADC(4, 0x49, 5, 7, 0, .0000078125, 1);
        ADC* voltage = new ADC(1, 0x4b, 0, 7, 0, .0001875, 1);
        ADC* frequency = new ADC(1, 0x4b, 0, 7, 3, .0001875, 1);
        ADC* thermo1 = new ADC(4, 0x48, 5, 7, 0, .0000078125, 1);
        ADC* thermo2 = new ADC(4, 0x48, 5, 7, 3, .0000078125, 1);


	SensorArray sensors;
	sensors.addSensor(loadCell1);
        sensors.addSensor(loadCell2);
        sensors.addSensor(loadCell3);
        sensors.addSensor(loadCell4);
        sensors.addSensor(loadCell5);
        sensors.addSensor(loadCell6);
        sensors.addSensor(current);
        sensors.addSensor(voltage);
        sensors.addSensor(frequency);
        sensors.addSensor(thermo1);
        sensors.addSensor(thermo2);

	testing test;
	test.setSensors(&sensors);
	cout << test.getFrequency();
	//sensors.run(file, microseconds);

	return 0;
}
