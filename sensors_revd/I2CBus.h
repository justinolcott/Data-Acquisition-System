#pragma once
#ifndef I2CBUS_H
#define I2CBUS_H

/*
	This is an object for each I2C bus
		An I2C Bus can be created by going into
		/boot/config.txt. Once there pull up the
		gpio pins with:
			gpio=<gpio_number>=pu
		and create the I2C bus with (1 line):
			dtoverlay=i2c-gpio,bus=<bus_number>,i2c_gpio_delay_us=<delay_num>,
			i2c_gpio_sda=<gpio_num>,i2c_gpio_scl=<gpio_num>
		The delay changes the baud rate for the I2C.
		Most sensors will have a recommended max of 
		400 kHz, but will work at 1000 kHz. It is not
		yet determined if delay can be less than 1 or not

		Each I2CBus has a mutex which prevents a data race
		for the bus
*/

#include "MyException.h"
#include "I2CDevice.h"
#include <vector>
#include <mutex>

class I2CBus {
private:
	std::vector<I2CDevice*> i2c;
	std::mutex mx;
	int bus;

	void closeBus() {
		for (size_t i = 0; i < i2c.size(); i++) {
			delete i2c.at(i);
		}
	}

	void int2Bus(int bus, char* busArray) {
		std::string s = "/dev/i2c-";
		s.append(to_string(bus));
		strcpy(busArray, s.c_str());
	}

public:
	I2CBus(int bus = 1) {
		this->bus = bus;
	}
	~I2CBus() {
		closeBus();
	}

	size_t add(uint16_t address) {
		i2c.push_back(new I2CDevice(bus, address));
		return i2c.size() - 1;
	}

	void receive(int i2cIdentifier, unsigned char* data, int length) {
		std::lock_guard<std::mutex> lk(mx);
		i2c.at(i2cIdentifier)->receive(data, length);
	}

	void sendByte(int i2cIdentifier, unsigned char addr ,unsigned char data) {
		std::lock_guard<std::mutex> lk(mx);
		i2c.at(i2cIdentifier)->sendByte(addr, data);
	}

	void send(int i2cIdentifier, unsigned char* data, int length) {
		std::lock_guard<std::mutex> lk(mx);
		i2c.at(i2cIdentifier)->send(data, length);
	}
};

#endif
