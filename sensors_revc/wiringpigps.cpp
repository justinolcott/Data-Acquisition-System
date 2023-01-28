#include <wiringSerial.h>
#include <iostream>

int main() {
	int serial = serialOpen("/dev/serial0", 9600);

	while(1) {
		std::cout << (char)serialGetchar(serial);
	}


	return 0;
}
