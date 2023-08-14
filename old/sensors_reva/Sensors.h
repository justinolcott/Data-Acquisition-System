#ifndef SENSOR_H
#define SENSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
//#include "MyException.h"
//Will want to get rid of namespace eventually
using namespace std;

#include "I2CDevice.h"

typedef enum : uint8_t {
	TWO_THIRDS,
	ONE,
	TWO,
	FOUR,
	EIGHT,
	SIXTEEN
} gainValue;

typedef enum : uint8_t {
	SPEED_EIGHT,
	SPEED_SIXTEEN,
	THIRTY_TWO,
	SIXTY_FOUR,
	ONE_HUNDRED_TWENTY_EIGHT,
	TWO_HUNDRED_FIFTY,
	FOUR_HUNDRED_SEVENTY_FIVE,
	EIGHT_HUNDRED_SIXTY
} speedValue;


class Sensor {
public:
	virtual ~Sensor() {}
	virtual double getValue() = 0;
	virtual void getData(double*) = 0;
	virtual string toString() = 0;
	virtual string header() = 0;
};

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define INT_ENABLE   0x38
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47

class MPU6050 : public Sensor {
private:
	I2CDevice* i2c;
	unsigned char config;
	double accelFactor;
	double gyroFactor;

public:
	MPU6050(int bus = 1, uint16_t address = 0x68) {
		i2c = new I2CDevice(bus, address);
		unsigned char config[2] = {0};
		config[0] = SMPLRT_DIV;
		config[1] =  0x07;
		i2c->send(config, 2);
		config[0] = PWR_MGMT_1;
		config[1] =  0x01;
		i2c->send(config, 2);
		config[0] = CONFIG;
		config[1] =  0;
		i2c->send(config, 2);
		config[0] = GYRO_CONFIG;
		config[1] =  0x18;
		i2c->send(config, 2);
		config[0] = INT_ENABLE;
		config[1] =  0x01;
		i2c->send(config, 2);
		accelFactor = 16384.0;
		gyroFactor = 131;
	}
	~MPU6050() {
		delete i2c;
	}

	short read_raw(unsigned char addr) {
		short value;
		unsigned char data[1] = {0};
		unsigned char address[1] = {addr};
		i2c->send(address, 1);
		i2c->receive(data, 1);
		short highByte = data[0];
		address[0] = addr + 1;
		i2c->send(address, 1);
		i2c->receive(data, 1);
		short lowByte = data[0];
		value = (highByte << 8) | lowByte;
		return value;
	}

	double AccX() {
		return double(read_raw(ACCEL_XOUT_H)) / accelFactor;
	}

	double AccY() {
                return double(read_raw(ACCEL_YOUT_H)) / accelFactor;
        }

	double AccZ() {
                return double(read_raw(ACCEL_ZOUT_H)) / accelFactor;
        }

	double gyroX() {
		return double(read_raw(GYRO_XOUT_H)) / gyroFactor;
	}

	double gyroY() {
                return double(read_raw(GYRO_YOUT_H)) / gyroFactor;
        }

        double gyroZ() {
                return double(read_raw(GYRO_ZOUT_H)) / gyroFactor;
        }
/*
	struct MPU6050Data {
		double accX;
		double accY;
		double accZ;
		double gyroX;
		double gyroY;
		double gyroZ;
	};

	MPU6050Data getMPU6050Data() {
		struct MPU6050Data data;
		data.accX = AccX();
		data.accY = AccY();
		data.accZ = AccZ();
		data.gyroX = gyroX();
		data.gyroY = gyroY();
		data.gyroZ = gyroZ();

		return data;
	}
*/
	double getValue() {
		return AccZ();
	}

	void getData(double* data) {
		data[0] = AccX();
		data[1] = AccY();
		data[2] = AccZ();
		data[3] = gyroX();
		data[4] = gyroY();
		data[5] = gyroZ();
	}

	string toString() {
		string out;
		out += to_string(AccX()) + ",";
		out += to_string(AccY()) + ",";
		out += to_string(AccZ()) + ",";
		out += to_string(gyroX()) + ",";
		out += to_string(gyroY()) + ",";
		out += to_string(gyroZ()) + ",";
		return out;
	}

	string header() {
		return "AccX(g), AccY(g), AccZ(g), GyX(°/s), GyY(°/s), GyZ(°/s), ";
	}
/*
	string toString() {
		struct MPU6050Data data = getMPU6050Data();
		string out;
		out += to_string(data.accX);
		out += ", ";
		out += to_string(data.accY);
                out += ", ";
		out += to_string(data.accZ);
                out += ", ";
		out += to_string(data.gyroX);
                out += ", ";
		out += to_string(data.gyroY);
                out += ", ";
		out += to_string(data.gyroZ);
                out += ", ";

		return out;
	}
*/
};


class ADC : public Sensor {
private:
	I2CDevice* i2c;
	unsigned char config[3];
	double factor;
	//double setFactor(gainValue gain) {
	//	switch (gain) {
	//	case TWO_THIRDS : 
	//	}
	//}

public:
	ADC(int bus = 1, uint16_t address = 0x48, uint8_t gain = 5, uint8_t speed = 7, uint8_t channel = 0, uint8_t mode = 0) {
		i2c = new I2CDevice(bus, address);
		config[0] = 0x01;
		config[1] = (channel << 4) | (gain << 1) | mode;
		config[2] = speed << 5;
		i2c->send(config, 3);
		factor = .000125 / 16; //edit to allow all gains
	}
	~ADC() {
		delete i2c;
	}

	int getRaw() {
		unsigned char reg[1] = { 0x00 };
		i2c->send(reg, 1);
		unsigned char data[2] = { 0 };
		i2c->receive(data, 2);
		int raw = (data[0] * 256 + data[1]);
		if (raw > 32767) {
			raw -= 65535;
		}
		return raw;
	}

	double voltage() {
		return getRaw() * factor;
	}

	double getValue() {
		return voltage();
	}

	void getData(double* data) {
		data[0] = voltage();
	}

	string toString() {
		return to_string(voltage()) + ",";
	}

	string header() {
		return "Volt(V), ";
	}
};

#include <chrono>

class TimeSensor : public Sensor {
private:
	std::chrono::high_resolution_clock::time_point initialTime;
public:
	void start() {
		initialTime = std::chrono::high_resolution_clock::now();
	}
	double getTime() {
		std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> time_chrono = current - initialTime;
		return time_chrono.count() / 1000000;
	}
	double getValue() {
		return getTime();
	}
	void getData(double* data) {
		data[0] = getTime();
	}
	string toString() {
		return to_string(getTime()) + ",";
	}

	string header() {
		return "Time(s), ";
	}
};

string getInput() {
	string input;
	cin >> input;
	return input;
}
#include <csignal>
#include <future>
#include <fstream>
#include <vector>
#include <thread>
class SensorArray {
private:
	vector<Sensor*> sensors;
	TimeSensor timeSensor;

public:
	SensorArray() {}
	~SensorArray() {
		for (int i = sensors.size() - 1; i >= 0; i--) {
			delete sensors.at(i);
		}
	}
	void addSensor(Sensor* sensor) {
		sensors.push_back(sensor);
	}
	void getValues(double* data) {
		int n = sensors.size();
		for (int i = 0; i < n; i++) {
			data[i] = sensors.at(i)->getValue();
		}
	}

	int size() {
		return sensors.size();
	}

	//doesnt have time in it
	void getDatas(double** data) {
		int n = sensors.size();
		for (int i = 0; i < n; i++) {
			sensors.at(i)->getData(data[i]);
		}
	}

	string headers() {
		string out = timeSensor.header();
		for (unsigned int i =0; i < sensors.size(); i++) {
			out += sensors.at(i)->header();
		}
		out.pop_back();
		out.pop_back();
		return out;
	}

	string toString() {
		string out = to_string(timeSensor.getTime()) + ",";
		for (unsigned int i = 0; i < sensors.size(); i++) {
			out += sensors.at(i)->toString();
		}
		out.pop_back();
		return out;
	}

	void start() {
		timeSensor.start();
	}

	void run(string file, int microseconds) {
		ofstream outfile;
		outfile.open(file);
		if (!outfile.is_open()) {
			outfile.open("backup.csv");
		}

		cout << "Ctrl + C to stop" << endl;
		outfile << headers() << endl;
		cout << headers() << endl;
		start();
		try {
			while(1) {
				string out = toString();
				outfile << out << endl;
				cout << out << endl;
				this_thread::sleep_for(std::chrono::microseconds(microseconds));
			}
		}
		catch(MyException& e) {
			outfile.close();
			return;
		}
	}
};

#endif
