#include "Testing.h"
#include "Sensors.h"

void run(Sensor* sensor) {
	sensor->toString();
}

int main() {
	MPU6050* sensor = new MPU6050();
	SensorArray sensors;
	sensors.addSensor(sensor);
	testing test;
	test.setSensors(&sensors);
	cout << test.getFrequency(sensor, &run) << endl;
	return 0;
}
