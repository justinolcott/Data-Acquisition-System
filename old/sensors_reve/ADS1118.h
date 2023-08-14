#include "BaseSensor.h"
#include "SPIBus.h"
#include <vector>
#include <string>
#include "ADS111X.h"

union Config {
                ///Structure of the config register of the ADS1118. (See datasheet [1])
	struct {
                uint8_t reserved:1;     ///< "Reserved" bit
                uint8_t noOperation:2;  ///< "NOP" bits
                uint8_t pullUp:1;               ///< "PULL_UP_EN" bit
                uint8_t sensorMode:1;   ///< "TS_MODE" bit
                uint8_t rate:3;                 ///< "DR" bits
                uint8_t operatingMode:1;///< "MODE" bit
                uint8_t pga:3;                  ///< "PGA" bits
                uint8_t mux:3;                  ///< "MUX" bits
        	uint8_t singleStart:1;  ///< "SS" bit
        } bits;
        uint16_t word;                          ///< Representation in word (16-bits) format
        struct {
                uint8_t lsb;                    ///< Byte LSB
		uint8_t msb;                    ///< Byte MSB
	} byte;                                         ///< Representation in bytes (8-bits) format
};


//no ability to read internal temperature
class ADS1118 : public BaseSensor {
private:
	SPIBus* bus;
	size_t spi;
	std::vector<Config> configs;
	std::vector<double> factors;
	size_t numChannels;

	void sendConfig(Config configRegister) {
		unsigned char config[2] = {configRegister.byte.msb, configRegister.byte.lsb};
		bus->write(spi, config, 2);
	}

	double getFactor(gainValue gain) {
		if (gain == TWO_THIRDS) {
			return .1875;
		}
		else {
			return .125 / gain;
		}
	}

	int getRaw() {
		unsigned char receive[2];
		bus->read(spi, receive, 2);
		int raw = (receive[0] * 256 + receive[1]);
       	 	if (raw > 32767) {
                	raw -= 65535;
        	}
		return raw;
	}

public:
	ADS1118(SPIBus* bus, int cs = 0, int mode = 1, int speed = 1000000, uint8_t bitsPerWord = 8) {
		this->bus = bus;
		spi = bus->add(cs, mode, speed, bitsPerWord);
		numChannels = 0;
	}
	~ADS1118() {}

	void addChannel(gainValue gain, speedValue speed, multiplexer mux, std::string header) {
		setHeader(getHeader() + header + ",");
		++numChannels;//         RESR, VCFG, DPLUP, temp, rate, mode, gain, mux, start-single
		Config configRegister = {0x01, 0x01, 0x01, 0, speed, 0, gain, mux, 0};
		configs.push_back(configRegister);
		factors.push_back(getFactor(gain));
	}

	double getVoltage(size_t channel) {
		if (numChannels != 1) {
			sendConfig(configs.at(0));
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
