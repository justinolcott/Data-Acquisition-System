#pragma once
#ifndef ADS1115_H
#define ADS1115_H

/*
Use this class for each instance of ADS1115 sensor
	For each channel -- differential or single-ended--
	add a channel. If there is more than one channel,
	the config register has to be rewritten which
	takes 4 ms.

	ADS1115 uses I2C protocol, so to prevent a data race,
	an I2CBus is passed in. This prevents multiple threads
	using the Bus at the same time.

	There are 3 enumerated types for gain, speeds, and 
	multiplexer configuration
*/

#include <threads>
#include <chrono>
#include <string>
#include <mutex>
#include <vector>

#include "I2CBus.h"
#include "BaseSensor.h"

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

typedef enum : uint8_t {
	A0_A1,
	A0_A3,
	A1_A3,
	A2_A3,
	A0,
	A1,
	A2,
	A3,
} multiplexer;

class ADS1115 : public BaseSensor {
private:
	I2CBus* bus;
	size_t i2c;
	size_t numChannels;
	std::vector<unsigned char[3]> configs;
	std::vector<double> factors;

	double setFactor(gainValue gain) {
		if (gain == 0) {
			return .1875;
		}
		else {
			return .125 / gain;
		}
	}

	int getRaw() {
		unsigned char reg[1] = { 0x00 };
		bus->send(i2c, reg, 1);
		unsigned char data[2] = { 0 };
		bus->receive(i2c, data, 2);
		int raw = (data[0] * 256 + data[1]);
		if (raw > 32767) {
			raw -= 65535;
		}
		return raw;
	}

	void sendConfig(unsigned char[3] config) {
		bus->send(i2c, config, 3);
		std::this_thread::sleep_for(std::chrono::microseconds(4000));
	}

	double getVoltage(size_t channel) {
		if (numChannels != 1) {
			sendConfig(configs.at(channel));
		}
		return getRaw() * factors.at(channel);
	}

public:
 	ADC(I2CBus* bus, uint16_t address = 0x48) {
		this->bus = bus;
		i2c = bus->add(address);
		setHeader("");
	}
	~ADC() {
		for (size_t i = 0; i < configs.size(); i++) {
			delete configs.at(i);
		}
	}

	addChannel(gainValue gain, multiplexer mulitplx, speedValue speed, std::string header) {
		uint8_t conversionMode = 0;
		++numChannels;
		configs.push_back(new unsigned char[3]);
		configs.back()[0] = 0x01;
		configs.back()[1] = (multiplexer << 4) | (gain << 1) | conversionMode; //currently it always runs continuous, but the mode allows reset of config
		configs.back()[2] = speed << 5;

		factors.push_back(setFactor(gain));
		setHeader(getHeader() + header + ",");
	}

	void start() {
		if (numChannels == 1) {
			sendConfig(configs.at(1));
		}
		std::string out;
		while (1) {
			out = "";
			for (size_t i = 0; i < numChannels; i++) {
				out += getVoltage(i);
				out += ",";
			}
			set(out);
		}
	}
};

#endif