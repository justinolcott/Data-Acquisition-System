// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <iostream>
using namespace std;
int main() {
	int serial = open("/dev/serial0", O_RDWR);

	if (serial < 0) {
		cout << "error opening" << strerror(errno) << endl;
	}

	struct termios tty;
	memset(&tty, 0, sizeof tty);

	if(tcgetattr(serial, &tty) != 0) {
    		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}

	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
//	tty.c_cflag |= PARENB;  // Set parity bit, enabling parity

	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
//	tty.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication


	tty.c_cflag |= CS8;

	tty.c_cflag &= ~CRTSCTS;

	tty.c_cflag |= CREAD | CLOCAL;

	tty.c_lflag &= ~ICANON;

	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo

	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR;

	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	// Save tty settings, also checking for error
	if (tcsetattr(serial, TCSANOW, &tty) != 0) {
    		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	}

	char read_buf [256];
	memset(&read_buf, '\0', sizeof(read_buf));

	//int n = read(serial, &read_buf, sizeof(read_buf));

	// settings above, specifically VMIN and VTIME
	int num_bytes = read(serial, &read_buf, sizeof(read_buf));

// n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
	if (num_bytes < 0) {
    		printf("Error reading: %s", strerror(errno));
	}

// Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
// print it to the screen like this!)
	printf("Read %i bytes. Received message: %s", num_bytes, read_buf);

	close(serial);
	return 0;
}
