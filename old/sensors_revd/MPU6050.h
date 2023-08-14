#pragma once
#ifndef MPU6050_H
#define MPU6050_H

#include <string>

#include "I2CBus.h"
#include "BaseSensor.h"

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

class MPU6050 : public BaseSensor {
private:
	I2CBus* bus;
	size_t i2c;

	unsigned char config;
	double accelFactor;
	double gyroFactor;

public:
	MPU6050(I2CBus* bus, uint16_t address = 0x68) {
		setHeader("AccX(g), AccY(g), AccZ(g), GyX(°/s), GyY(°/s), GyZ(°/s), ");
		this->bus = bus;
		i2c = bus->add(address);

		unsigned char config[2] = {0};
		config[0] = SMPLRT_DIV;
		config[1] =  0x07;
		bus->send(i2c, config, 2);
		config[0] = PWR_MGMT_1;
		config[1] =  0x01;
		bus->send(i2c, config, 2);
		config[0] = CONFIG;
		config[1] =  0;
		bus->send(i2c, config, 2);
		config[0] = GYRO_CONFIG;
		config[1] =  0x18;
		bus->send(i2c, config, 2);
		config[0] = INT_ENABLE;
		config[1] =  0x01;
		bus->send(i2c, config, 2);
		accelFactor = 16384.0;
		gyroFactor = 131;
	}
	~MPU6050() {}

	short read_raw(unsigned char addr) {
		short value;
		unsigned char data[1] = {0};
		unsigned char address[1] = {addr};
		bus->send(i2c, address, 1);
		bus->receive(i2c, data, 1);
		short highByte = data[0];
		address[0] = addr + 1;
		bus->send(i2c, address, 1);
		bus->receive(i2c, data, 1);
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

	void start() {
		std::string out;
		while (1) {
			out = "";
			out += to_string(AccX()) + ",";
			out += to_string(AccY()) + ",";
			out += to_string(AccZ()) + ",";
			out += to_string(gyroX()) + ",";
			out += to_string(gyroY()) + ",";
			out += to_string(gyroZ()) + ",";
			set(out);
		}
	}
};

#endif
