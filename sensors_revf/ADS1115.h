#pragma once
#ifndef ADS1115_H
#define ADS1115_H

#include <thread>
#include <chrono>
#include <string>
#include <vector>

#include "BaseSensor.h"
#include "I2CDevice.h"
#include "I2CBus.h"

#include "ADS111X.h"
#include <iostream>

class ADS1115 : public BaseSensor {
private:
	I2CBus* bus;
	size_t i2c;
	std::vector<std::vector<unsigned char>> configs;
	std::vector<double> factors;
	size_t numChannels;

	void sendConfig(std::vector<unsigned char> config) {
		unsigned char configData[3] = {config.at(0), config.at(1), config.at(2)};
		bus->send(i2c, configData, 3);
		std::this_thread::sleep_for(std::chrono::microseconds(4000));
	}

	double getFactor(gainValue gain) {
		if (gain == 0) {
			return .1875;
		}
		else {
			return .125 / 16;
		}
	}

public:
 	ADS1115(I2CBus* bus, uint16_t address = 0x48) {
		this->bus = bus;
		i2c = bus->add(address);
		numChannels = 0;
	}
	~ADS1115() {}

	void addChannel(gainValue gain, speedValue speed, multiplexer mux, std::string header) {
		setHeader(getHeader() + header + ",");
		++numChannels;
		std::vector<unsigned char> config;
		config.push_back(0x01);
		config.push_back((mux << 4) | (gain << 1) | 0);
                config.push_back((speed << 5));
		configs.push_back(config);
		factors.push_back(getFactor(gain));
	}

	int getRaw() {
		unsigned char rec[2] = { 0 };
		bus->get(i2c, 0x00, rec, 2);
//		unsigned char reg[1] = { 0x00 };
//		bus->send(i2c, reg, 1);
//		unsigned char data[2] = { 0 };
//		bus->receive(i2c, data, 2);
		int raw = (rec[0] * 256 + rec[1]);
		if (raw > 32767) {
			raw -= 65535;
		}
		return raw;
	}

	double getVoltage(size_t channel) {
		if (numChannels != 1) {
			sendConfig(configs.at(channel));
		}
		return getRaw() * factors.at(channel);
	}

	void start() {
		if (numChannels == 1) {
			sendConfig(configs.at(0));
		}
		std::string out;
		while (1) {
			out = "";
			for (size_t i = 0; i < numChannels; i++) {
				out += std::to_string(getVoltage(i));
				out += ",";
			}
			set(out);
		}
	}

};

#endif
