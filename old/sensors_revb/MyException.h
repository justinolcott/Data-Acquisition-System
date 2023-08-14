#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H
#include <exception>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string>

class MyException : public std::exception {
private:
        std::string message;
public:
        MyException(const std::string& message) noexcept : message(message) {
                std::cout << this->message << std::endl;
        }
        const char* what() const noexcept override {
                return this->message.c_str();
        }
};

void signalInteruption(int s) {
	throw MyException("Signal Interupt" + std::to_string(s));
}

#endif

