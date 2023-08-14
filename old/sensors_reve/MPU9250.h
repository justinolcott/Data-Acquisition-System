#pragma once
#ifndef MPU9250_H
#define MPU9250_H

#include <thread>
#include <chrono>

#include "SPIBus.h"
#include "BaseSensor.h"
#include "MPU9250Reg.h"

#include <iostream>


class MPU9250 {
private:
	SPIBus* bus;
	size_t spi;
	double accDivider;
	double gyroDivider;

	unsigned int transfer(uint8_t writeData) {
		unsigned char buf[1] = {writeData};
		unsigned char rec[1] = {0};
		bus->transfer(spi, buf, rec, 1);
		return rec[0];
	}

	unsigned int writeReg(uint8_t writeAddr, uint8_t writeData) {
		unsigned char reg[1] = {writeAddr};
		bus->write(spi, reg, 1);
		return transfer(writeData);
	}

	unsigned int readReg(uint8_t writeAddr, uint8_t writeData) {
		return writeReg(writeAddr | READ_FLAG, writeData);
	}

	void readRegs(uint8_t readAddr, uint8_t* rxBuf, unsigned int bytes) {
		for (unsigned int i = 0; i < bytes; i++) {
			rxBuf[i] = transfer(0x00);
		}
	}

	unsigned int setAcc(int scale) {
		writeReg(MPUREG_ACCEL_CONFIG, scale);
		switch (scale){
        		case BITS_FS_2G:
            			accDivider=16384;
        		break;
        		case BITS_FS_4G:
            			accDivider=8192;
        		break;
        		case BITS_FS_8G:
            			accDivider=4096;
        		break;
        		case BITS_FS_16G:
            		accDivider=2048;
        		break;
    		}
		unsigned int tempScale = writeReg(MPUREG_ACCEL_CONFIG | READ_FLAG, 0x00);

		switch (tempScale){
        	case BITS_FS_2G:
            		tempScale=2;
        	break;
        	case BITS_FS_4G:
            		tempScale=4;
        	break;
        	case BITS_FS_8G:
            		tempScale=8;
        	break;
        	case BITS_FS_16G:
            		tempScale=16;
        	break;
    		}
    		return tempScale;
	}

	unsigned int setGyro(int scale) {
    		unsigned int temp_scale;
    		writeReg(MPUREG_GYRO_CONFIG, scale);

    		switch (scale){
        		case BITS_FS_250DPS:   gyroDivider = 131;  break;
        		case BITS_FS_500DPS:   gyroDivider = 65.5; break;
        		case BITS_FS_1000DPS:  gyroDivider = 32.8; break;
        		case BITS_FS_2000DPS:  gyroDivider = 16.4; break;
    		}

	    	temp_scale = writeReg(MPUREG_GYRO_CONFIG|READ_FLAG, 0x00);

	    	switch (temp_scale){
	        	case BITS_FS_250DPS:   temp_scale = 250;    break;
	        	case BITS_FS_500DPS:   temp_scale = 500;    break;
	        	case BITS_FS_1000DPS:  temp_scale = 1000;   break;
	        	case BITS_FS_2000DPS:  temp_scale = 2000;   break;
	    	}
    		return temp_scale;
	}

public:
	MPU9250(SPIBus* bus, int cs = 0, int mode = 0, int speed = 1000000, uint8_t bitsPerWord = 8) {
		this->bus = bus;
		spi = bus->add(cs, mode, speed, bitsPerWord);

		int MPU_InitRegNum = 17;

		uint8_t MPU_Init_Data[MPU_InitRegNum][2] = {
        		{BIT_H_RESET, MPUREG_PWR_MGMT_1},        // Reset Device
        		{0x01, MPUREG_PWR_MGMT_1},               // Clock Source
        		{0x00, MPUREG_PWR_MGMT_2},               // Enable Acc & Gyro
        		{BITS_DLPF_CFG_188HZ, MPUREG_CONFIG},     // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
	        	{BITS_FS_250DPS, MPUREG_GYRO_CONFIG},    // +-250dps
	        	{BITS_FS_2G, MPUREG_ACCEL_CONFIG},       // +-2G
	        	{BITS_DLPF_CFG_188HZ, MPUREG_ACCEL_CONFIG_2}, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
	        	{0x12, MPUREG_INT_PIN_CFG},      //
	        	//{0x40, MPUREG_I2C_MST_CTRL},   // I2C Speed 348 kHz
	        	//{0x20, MPUREG_USER_CTRL},      // Enable AUX
	        	{0x30, MPUREG_USER_CTRL},        // I2C Master mode and set I2C_IF_DIS to disable slave mode I2C bus
		        	{0x0D, MPUREG_I2C_MST_CTRL},     // I2C configuration multi-master  IIC 400KHz

		        {AK8963_I2C_ADDR, MPUREG_I2C_SLV0_ADDR},  // Set the I2C slave addres of AK8963 and set for write.
		        //{0x09, MPUREG_I2C_SLV4_CTRL},
		        //{0x81, MPUREG_I2C_MST_DELAY_CTRL}, // Enable I2C delay

		        {AK8963_CNTL2, MPUREG_I2C_SLV0_REG}, // I2C slave 0 register address from where to begin data transfer
		        {0x01, MPUREG_I2C_SLV0_DO},   // Reset AK8963
		        {0x81, MPUREG_I2C_SLV0_CTRL}, // Enable I2C and set 1 byte

	        	{AK8963_CNTL1, MPUREG_I2C_SLV0_REG}, // I2C slave 0 register address from where to begin data transfer
	        	{0x16, MPUREG_I2C_SLV0_DO},   // Register value to 100Hz continuous measurement in 16bit
	        	{0x81, MPUREG_I2C_SLV0_CTRL}  //Enable I2C and set 1 byte

		};

		for (int i = 0; i < MPU_InitRegNum; i++) {
			writeReg(MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
		}

		setAcc(BITS_FS_2G);
		setGyro(BITS_FS_250DPS);



	}
	~MPU9250() {}


	void readAcc() {
    		uint8_t response[6];
    		int16_t bit_data;
    		//float data;
    		int i;
   		readRegs(MPUREG_ACCEL_XOUT_H,response,6);
   		for(i = 0; i < 3; i++) {
   	     		bit_data = ((int16_t)response[i*2]<<8)|response[i*2+1];
   	     		std::cout << (float)bit_data / accDivider << ", ";
        		//accel_data[i] = data/acc_divider - a_bias[i];
    		}
	}





};

#endif
