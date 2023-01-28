#pragma once
#ifndef FILEMANIP_H
#define FILEMANIP_H

#include <chrono>
#include <ctime>
#include <string>
#include <algorithm>
#include <cctype>

std::string getTime() {
	std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string out = "/home/pi/sensors_reve/data/";
	out += std::ctime(&end_time);
	out.erase(std::remove(out.begin(), out.end(), ' '), out.end());
	out.pop_back();
	out += ".csv";
	return out;
}

#endif
