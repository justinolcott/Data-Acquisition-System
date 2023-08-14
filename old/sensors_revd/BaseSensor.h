#pragma once
#ifndef BASESENSOR_H
#define BASESENSOR_H

#include <string>
#include <mutex>

class BaseSensor {
public:
	void set(std::string input) {
		std::lock_guard<std::mutex> lk(mx);
		data = input;
	}

	std::string get() {
		std::lock_guard<std::mutex> lk(mx);
		return data;
	}

	std::string getHeader() {
		return header;
	}

	void setHeader(std::string input) {
		header = input;
	}

	virtual void start() = 0;

protected:
	std::string data;
	std::mutex mx;
	std::string header;
};

#endif