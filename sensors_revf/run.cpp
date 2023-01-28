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
using namespace std;

#include <exception>

class MyException : public std::exception {
private:
	std::string message;
public:
	MyException(const std::string& message) noexcept : message(message) {
		std::cout << this->message << std::endl;
	}
	const char* what() const noexcept override {
		return this->message.c_str();
	}
};
//error handling needs to be added
class I2CDevice {
private:
	int i2c;
	std::string errorMessage;
	bool errorFlag;
	
	void closeI2C() {
		if (i2c) {
			close(i2c);
			i2c = 0;
		}
	}
	
	int errorMsg(std::string message) {
		errorMessage = message;
		errorFlag = true;
		closeI2C();
		return -1;
	}

	void int2Bus(int bus, char* busArray) {
		std::string s = "/dev/i2c-";
		s.append(to_string(bus));
		strcpy(busArray, s.c_str());
	}

public:
	I2CDevice(int bus = 1, uint16_t address = 0x48) {
		errorMessage = "";
		errorFlag = false;
		char busArray[20];
		int2Bus(bus, busArray);
		try {
			if ((i2c = open(busArray, O_RDWR)) < 0) {
				throw MyException("Error opening bus");
			}
			if (ioctl(i2c, I2C_SLAVE, address) < 0) {
				throw MyException("Error with slave address");
			}
		}
		catch (const MyException & e) {
			std::cout << "Exception Caught" << e.what() << std::endl;
		}
	}
	~I2CDevice() {
		closeI2C();
	}

	void receive(unsigned char* data, int length) {
		if (read(i2c, data, length) != length) {
			throw errorMsg("i2c read error");
		}
	}
/*
	void receiveByte(short &data, unsigned char addr) {
		unsigned char address[1] = {addr};
		if (write(i2c, address, 1) != 1) {
			throw errorMsg("i2c write error in receiveShort");
		}
		if (read(i2c, data, 2) != 2) {
			throw errorMsg("i2c read error in receiveShort");
		}
	}
*/
	void sendByte(unsigned char addr ,unsigned char data) {
		unsigned char sendData[2] = {addr, data};
		if (write(i2c, sendData, 2) != 2) {
			throw errorMsg("i2c write error in sendShort");
		}
	}

	void send(unsigned char* data, int length) {
		if (write(i2c, data, length) != length) {
			throw errorMsg("i2c write error");
		}
	}
};


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
	virtual void getData(double*&) = 0;
	virtual string toString() = 0;
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

	short read_raw(int addr) {
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



	double getValue() {
		return AccZ();
	}

	void getData(double* &data) {
		data[0] = AccX();
		data[1] = AccY();
		data[2] = AccZ();
		data[3] = gyroX();
		data[4] = gyroY();
		data[5] = gyroZ();
	}


	string toString() {
	//	double data[6] = {0};
	//	getData(&data);
		string out;
	/*	out += data[0];
		out += ", ";
		out += data[1];
		out += ", ";
		out += data[2];
		out += ", ";
		out += data[3];
		out += ", ";
		out += data[4];
                out += ", ";
		out += data[5]; */
                out += ", ";
		return out;
	}

	

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

	void getData(double* &data) {
		data[0] = {voltage()};
	}

	string toString() {
		string out;
		out += voltage();
		return out;
	}
};

#include <vector>
class SensorArray {
private:
	vector<Sensor*> sensors;
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

	void getDatas(double** data) {
		int n = sensors.size();
		for (int i = 0; i < n; i++) {
			sensors.at(i)->getData(data[i]);
		}
	}

	string toString() {
		string out;
		for (unsigned int i = 0; i < sensors.size(); i++) {
			out += sensors.at(i)->toString();
		}
		out.pop_back();
		out.pop_back();
		return out;
	}
};

#include <thread>
#include <chrono>
#include <atomic> //dont need this one
#include <ctime> //mayybe dont need this one
#define ITERATIONS 10
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

#include <ostream>
#include <fstream>

//function that will run each loop in order to set sleep time
void run(SensorArray* sensors, double** data) {
	sensors->getDatas(data);
}

//function that will output to a given stream - should be .csv
void output(ostream &out, SensorArray* sensors) {
        for (int i = 0; i < sensors->size(); i++) {
                out << sensors->toString() << endl;
        }
}

//void outputToFile(string file, SensorArray* sensors) {
//	ofstream outfile;
//	outfile.open(file, "w+");double time = time_chrono.count() / 1000000;
//}


//make time long double 

int main(int argc, char* argv[]) {

	//Take Hertz input
	double hertz;
	if (argc == 2) {
		hertz = atof(argv[1]);
	}
	else {
		hertz = 10;
	}

	cout << hertz << endl;

	//Create sensors
	MPU6050* sensor = new MPU6050();

	SensorArray sensors;
	sensors.addSensor(sensor);

	//Create Dynamic 2D array
	int height = 1;
	int length = 6;
	double** data = new double*[height];
	for (int i = 0; i < height; i++) {
		data[i] = new double[length];
	}
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

	long int timeSleep = 1000000;
	//Output data
	auto start = std::chrono::high_resolution_clock::now();
	while (1) {
		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::micro> time_chrono = current - start;
		double time = time_chrono.count() / 1000000;
		cout << time << ", ";
		sensors.getDatas(data);
		//output(cout, &sensors);
		cout << data[0][0] << ", " << data[0][1] << ", " << data[0][2] << ", " << data[0][3] << ", " << data[0][4] << ", " << data[0][5] << ", " << endl;
		this_thread::sleep_for(std::chrono::microseconds(timeSleep));
	}
	//Deallocate memory for dyanamic 2D array
	for (int i = 0; i < height; i++) {
		delete [] data[i];
	}
	delete [] data;

	return 0;
}

/*******
int main() {

	ADC* adc = new ADC();
	auto start = std::chrono::high_resolution_clock::now();
	double data[1] = {0.0};
	for (int i = 0; i < 1000; i++) {
		//adc->voltage() << endl;
		cout <<  adc->voltage() << endl;
		cout << adc->voltage() << endl;
		cout << adc->voltage() << endl;

	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<long double, std::micro> time_chrono = end - start;
	long double time = time_chrono.count() / 1000000;
//	double time = (end - start) / (double)CLOCKS_PER_SEC;
	cout << "rate is " << (time / 1000.0) << "hertz being " << 1 / (time / 1000.0);

	return 0;
}

*****/
#endif

