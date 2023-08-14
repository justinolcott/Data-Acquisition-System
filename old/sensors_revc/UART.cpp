//#ifndef UART_H
//#define UART_H

#include <stdio.h>
#include <stdlib.h>
//#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <string.h>
#include <termios.h>
//Will want to get rid of namespace eventually
using namespace std;

#include "MyException.h"

//Exception could go into anther header file
//class MyException : public std::exception {
//private://
	//std::string message;
//public:
//	MyException(const std::string& message) noexcept : message(message) {
//		std::cout << this->message << std::endl;
//	}
//	const char* what() const noexcept override {
//		return this->message.c_str();
//	}
//};

//error handling needs to be added
class SerialDevice {
private:
	int serial;
	std::string errorMessage;
	bool errorFlag;

	void closeSerial() {
		if (serial) {
			close(serial);
			serial = 0;
		}
	}

	int errorMsg(std::string message) {
		errorMessage = message;
		errorFlag = true;
		closeSerial();
		return -1;
	}

	void int2Bus(int bus, char* busArray) {
		std::string s = "/dev/serial";
		s.append(to_string(bus));
		strcpy(busArray, s.c_str());
	}

public:
	SerialDevice(int bus = 1, const int baudSpeed = 115200) {
		errorMessage = "";
		errorFlag = false;
		char busArray[20];
		int2Bus(bus, busArray);
		cout << busArray << endl;
		try {
			struct termios options ;
			speed_t myBaud ;
  			int     status;

  			switch (baudSpeed) {
    				case      50:	myBaud =      B50 ; break ;
    				case      75:	myBaud =      B75 ; break ;
    				case     110:	myBaud =     B110 ; break ;
    				case     134:	myBaud =     B134 ; break ;
    				case     150:	myBaud =     B150 ; break ;
    				case     200:	myBaud =     B200 ; break ;
    				case     300:	myBaud =     B300 ; break ;
    				case     600:	myBaud =     B600 ; break ;
    				case    1200:	myBaud =    B1200 ; break ;
    				case    1800:	myBaud =    B1800 ; break ;
    				case    2400:	myBaud =    B2400 ; break ;
    				case    4800:	myBaud =    B4800 ; break ;
    				case    9600:	myBaud =    B9600 ; break ;
    				case   19200:	myBaud =   B19200 ; break ;
    				case   38400:	myBaud =   B38400 ; break ;
    				case   57600:	myBaud =   B57600 ; break ;
    				case  115200:	myBaud =  B115200 ; break ;
    				case  230400:	myBaud =  B230400 ; break ;
    				case  460800:	myBaud =  B460800 ; break ;
    				case  500000:	myBaud =  B500000 ; break ;
    				case  576000:	myBaud =  B576000 ; break ;
    				case  921600:	myBaud =  B921600 ; break ;
    				case 1000000:	myBaud = B1000000 ; break ;
    				case 1152000:	myBaud = B1152000 ; break ;
    				case 1500000:	myBaud = B1500000 ; break ;
    				case 2000000:	myBaud = B2000000 ; break ;
    				case 2500000:	myBaud = B2500000 ; break ;
    				case 3000000:	myBaud = B3000000 ; break ;
    				case 3500000:	myBaud = B3500000 ; break ;
    				case 4000000:	myBaud = B4000000 ; break ;
    				default:
				throw MyException("Error choosing speed");
  			}

  			if ((serial = open (busArray, O_RDWR | O_NOCTTY/* | O_NDELAY | O_NONBLOCK*/)) == -1)
    				throw MyException("Error opening serial");

  			fcntl (serial, F_SETFL, O_RDWR) ;

			// Get and modify current options:

  			tcgetattr (serial, &options);

    			cfmakeraw   (&options) ;
    			cfsetispeed (&options, myBaud) ;
    			cfsetospeed (&options, myBaud) ;

    			options.c_cflag |= (CLOCAL | CREAD) ;
    			options.c_cflag &= ~PARENB ;
    			options.c_cflag &= ~CSTOPB ;
    			options.c_cflag &= ~CSIZE ;
    			options.c_cflag |= CS8 ;
    			options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    			options.c_oflag &= ~OPOST ;

	    		options.c_cc [VMIN]  =   0 ;
	    		options.c_cc [VTIME] = 10 ;	// one seconds (10 deciseconds)

	  		tcsetattr (serial, TCSANOW, &options) ;

	  		ioctl (serial, TIOCMGET, &status);

  			status |= TIOCM_DTR ;
  			status |= TIOCM_RTS ;

	  		ioctl (serial, TIOCMSET, &status);

	  		usleep (10000) ;
		}
		catch (const MyException & e) {
			std::cout << "Exception caught" << e.what() << std::endl;
		}
	}

	~SerialDevice() {
		close(serial);
	}

	void receive(unsigned char* data, int length) {
		int bytecount;
		//cout << sizeof(&data) << endl;
		if ((bytecount = read(serial, data, length)) < 0) {
			throw errorMsg("serial read error");
		}
		data[bytecount] = '\0';
	}

	void send(unsigned char* data) {
		if (write(serial, data, sizeof(&data)) != sizeof(data)) {
			throw errorMsg("i2c write error");
		}
	}

	int available() {
		int result;
		if (ioctl(serial, FIONREAD, &result) == -1) {
			return -1;
		}
		return result;
	}

	int getChar () {
  		uint8_t x ;

  		if (read (serial, &x, 1) != 1)
    			return -1 ;

  		return ((int)x) & 0xFF ;
	}


	std::string getLine() {
		unsigned char buffer[1];
		std::string response = "";
		do {
			read(serial, buffer, 1);
			response += buffer[0];

		} while (buffer[0] != '\n');

		return response;

	}


	void flush() {
		tcflush(serial, TCIOFLUSH);
	}
};

#endif
