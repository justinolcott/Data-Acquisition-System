#pragma once
#ifndef SPIDEVICE_H
#define SPIDEVICE_H

#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string>

#include "MyException.h"

class SPIDevice {
private:
	int spi;
	int speed;
	uint8_t bitsPerWord;
	int mode;
	int cs;

public:
	SPIDevice(int bus = 0, int cs = 0, int mode = 1, int speed = 1000000, uint8_t bitsPerWord = 8) {
		std::string spiFile = "/dev/spidev" + std::to_string(bus) + "." + std::to_string(cs);
		this->mode = mode;
		this->speed = speed;
		this->bitsPerWord = bitsPerWord;
		this->cs = cs;

		spi = open(spiFile.c_str(), O_RDWR);
		if (spi < 0) {
                	throw MyException("could not open spi");
        	}

        	if (ioctl(spi, SPI_IOC_WR_MODE, &mode) < 0) {
                	throw MyException("could not set spi mode write");
        	}

        	if (ioctl(spi, SPI_IOC_RD_MODE, &mode) < 0) {
                	throw MyException("could not set spi mode read");
        	}

        	if (ioctl(spi, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord) < 0) {
                	throw MyException("could not set up bits per word wr");
        	}

        	if (ioctl(spi, SPI_IOC_RD_BITS_PER_WORD, &bitsPerWord) < 0) {
                	throw MyException("could not set up bits per word rd");
        	}

        	if (ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
                	throw MyException("Could not set up speed");
        	}

        	if (ioctl(spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
                	throw MyException("could not set up read speed");
        	}
	}

	~SPIDevice() {
		close(spi);
	}

	void read(unsigned char *buf, unsigned count) {
		int err;
		struct spi_ioc_transfer spiStruct;

		memset(&spiStruct, 0, sizeof(spiStruct));
		spiStruct.tx_buf        = (unsigned) NULL;
		spiStruct.rx_buf        = (unsigned) buf;
		spiStruct.len           = count;
   		spiStruct.speed_hz      = speed;
		spiStruct.delay_usecs   = 0;
		spiStruct.bits_per_word = bitsPerWord;
		spiStruct.cs_change     = cs;
		err = ioctl(spi, SPI_IOC_MESSAGE(1), &spiStruct);

   		if (err < 0) {
			throw MyException("error with reading spi");
		}
	}

	void write(unsigned char *buf, unsigned count) {
		int err;
		struct spi_ioc_transfer spiStruct;
		memset(&spiStruct, 0, sizeof(spiStruct));
	   	spiStruct.tx_buf        = (unsigned) buf;
	   	spiStruct.rx_buf        = (unsigned) NULL;
	   	spiStruct.len           = count;
	   	spiStruct.speed_hz      = speed;
		spiStruct.delay_usecs   = 0;
	   	spiStruct.bits_per_word = bitsPerWord;
	   	spiStruct.cs_change     = cs;

	   	err = ioctl(spi, SPI_IOC_MESSAGE(1), &spiStruct);

   		if (err < 0) {
			throw MyException("error with writing spi");
		}
	}

	void transfer(unsigned char *txBuf, unsigned char *rxBuf, unsigned count) {
		int err;
		struct spi_ioc_transfer spiStruct;
		memset(&spiStruct, 0, sizeof(spiStruct));

		spiStruct.tx_buf        = (unsigned long)txBuf;
		spiStruct.rx_buf        = (unsigned long)rxBuf;
		spiStruct.len           = count;
		spiStruct.speed_hz      = speed;
		spiStruct.delay_usecs   = 0;
		spiStruct.bits_per_word = bitsPerWord;
		spiStruct.cs_change     = cs;

		err = ioctl(spi, SPI_IOC_MESSAGE(1), &spiStruct);
		if (err < 0) {
			throw MyException("error with spi transfer");
		}
	}
};
#endif
