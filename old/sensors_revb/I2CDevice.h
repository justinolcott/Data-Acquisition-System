#ifndef I2CDEVICE_H
#define I2CDEVICE_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
//Will want to get rid of namespace eventually
using namespace std;

#include <exception>

//Exception could go into anther header file
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

	//Add a function that automatically sends to a register to receive a byte

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
