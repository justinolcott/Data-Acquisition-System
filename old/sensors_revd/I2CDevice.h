#pragma once
#ifndef I2CDEVICE_H
#define I2CDEVICE_H

/*
	This is an instance of an I2C device or
	slave. It creates an individual file with
	a I2C bus and address

	I2CDevices should only be used within
	an I2C Bus if threads are being used in order
	to prevent a data race
*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>

#include "MyException.h"

class I2CDevice {
private:
	int i2c;

	void closeI2C() {
		if (i2c) {
			close(i2c);
			i2c = 0;
		}
	}

	void int2Bus(int bus, char* busArray) {
		std::string s = "/dev/i2c-";
		s.append(std::to_string(bus));
		std::strcpy(busArray, s.c_str());
	}

public:
	I2CDevice(int bus = 1, uint16_t address = 0x48) {
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

#endif
