#pragma once
#ifndef SPIBUS_H
#define SPIBUS_H

#include "MyException.h"
#include "SPIDevice.h"
#include <vector>
#include <mutex>

class SPIBus {
private:
	std::vector<SPIDevice*> spi;
	std::mutex mx;
	int bus;

	void closeBus() {
		for (size_t i = 0; i < spi.size(); i++) {
			delete spi.at(i);
		}
	}

public:
	SPIBus(int bus = 0) {
		this->bus = bus;
	}
	~SPIBus() {
		closeBus();
	}

	size_t add(int cs, int mode, int speed, uint8_t bitsPerWord) {
		spi.push_back(new SPIDevice(bus, cs, mode, speed, bitsPerWord));
		return spi.size() - 1;
	}

	void read(int spiIdentifier, unsigned char* data, unsigned length) {
		std::lock_guard<std::mutex> lk(mx);
		spi.at(spiIdentifier)->read(data, length);
	}

	void write(int spiIdentifier, unsigned char* data, unsigned length) {
		std::lock_guard<std::mutex> lk(mx);
		spi.at(spiIdentifier)->write(data, length);
	}

	void transfer(int spiIdentifier, unsigned char* txBuf, unsigned char* rxBuf, unsigned length) {
		std::lock_guard<std::mutex> lk(mx);
		spi.at(spiIdentifier)->transfer(txBuf, rxBuf, length);
	}
};

#endif

