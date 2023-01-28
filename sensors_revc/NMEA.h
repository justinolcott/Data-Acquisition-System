#pragma once
#ifndef NMEA_H
#define NMEA_H
#include <string>
#include <sstream>
#include <vector>

#define TIME_LENGTH 10
#define LAT_LENGTH 9
#define LONG_LENGTH 10

std::vector<std::string> splitStringByComma(std::string input) {
	std::vector<std::string> output;
	std::stringstream ss(input);
	std::string word;
	while(std::getline(ss, word, ',')) {
		output.push_back(word);
	}
	return output;
}

double minutesToDegreesLat(std::string input) {
	double degrees = stod(input.substr(0, 2));
	double minutes = stod(input.substr(2, 7));
	return degrees + (minutes / 60);
}

std::string getCoordinatesLat(std::string coordinates, std::string bearing) {
	double degrees = minutesToDegreesLat(coordinates);
	if (bearing == "S" || bearing == "W") {
		degrees *= -1.0;
	}
	return std::to_string(degrees);
}

double minutesToDegreesLong(std::string input) {
        double degrees = stod(input.substr(0, 3));
        double minutes = stod(input.substr(3, 7));
        return degrees + (minutes / 60);
}

std::string getCoordinatesLong(std::string coordinates, std::string bearing) {
        double degrees = minutesToDegreesLong(coordinates);
        if (bearing == "S" || bearing == "W") {
                degrees *= -1.0;
        }
        return std::to_string(degrees);
}

std::string timeFormat(std::string word) {
	std::ostringstream oss;
	oss << word.substr(0, 2) << ":";
	oss << word.substr(2, 2) << ":";
	oss << word.substr(4, 6);
	return oss.str();
}


//Given an NMEA sentence in rmc format, it will return Time, Lattitude, Longitutde, Altitude, Speed, bearing, 
std::string gprmc(std::vector<std::string> sentence) {
	std::ostringstream oss;
	oss << timeFormat(sentence.at(1)) << ",";
	oss << getCoordinatesLat(sentence.at(3), sentence.at(4)) << ",";
	oss << getCoordinatesLong(sentence.at(5), sentence.at(6)) << ",";
	oss << "NaN,"; //No altitude in RMC
	oss << sentence.at(7) << ",";
	oss << sentence.at(8) << ",";
	return oss.str();
}

bool isValidGPRMC(std::vector<std::string> sentence) {
	if (sentence.at(0) != "$GPRMC") { return false; }
	else if (sentence.at(2) != "A") { return false; }
	else if (sentence.size() <= 11) { return false; }
	else if (sentence.at(1).length() != TIME_LENGTH) { return false; }
	else if (sentence.at(3).length() != LAT_LENGTH) { return false; }
	else if (sentence.at(5).length() != LONG_LENGTH) {return false; }
	else { return true; }
}

//no NaN in this one
std::string gpgga(std::vector<std::string> sentence) {
	std::ostringstream oss;
	oss << timeFormat(sentence.at(1)) << ",";
	oss << getCoordinatesLat(sentence.at(2), sentence.at(3)) << ",";
	oss << getCoordinatesLong(sentence.at(4), sentence.at(5)) << ",";
	oss << sentence.at(9) << ",";
	//oss << "NaN,";
	//oss << "NaN,"
	return oss.str();
}

bool isValidGPGGA(std::vector<std::string> sentence) {
	if (sentence.at(0) != "$GPGGA") { return false; }
	else if (sentence.size() <= 14) { return false; }
	else if (sentence.at(1).length() != TIME_LENGTH) { return false; }
	else if (sentence.at(2).length() != LAT_LENGTH) { return false; }
	else if (sentence.at(4).length() != LONG_LENGTH) { return false; }
	else { return true; }
}

std::string unknown() {
	return "NaN,NaN,NaN,NaN,NaN,NaN,";
}

std::string format(std::string line) {
	std::vector<std::string> sentence = splitStringByComma(line);
	if (isValidGPRMC(sentence)) { return gprmc(sentence); }
	if (isValidGPGGA(sentence)) { return gpgga(sentence); }
	else { return unknown(); }
}

//Outputs all info given two strings both being either RMC or GGA
std::string custom(std::string line1, std::string line2) {
	std::vector<std::string> sentence1 = splitStringByComma(line1);
	std::vector<std::string> sentence2 = splitStringByComma(line2);
	std::ostringstream oss;
	if (isValidGPGGA(sentence1)) {
		oss << gpgga(sentence1);
		if (isValidGPRMC(sentence2)) {
			oss << sentence2.at(7) << "," << sentence2.at(8) << ",";
		}
		else {
			oss << "NaN,NaN,";
		}
	}
	else if (isValidGPGGA(sentence2)) {
		oss << gpgga(sentence2);
		if (isValidGPRMC(sentence1)) {
			oss << sentence1.at(7) << "," << sentence1.at(8) << ",";
		}
		else {
			oss << "NaN,NaN";
		}
	}
	else {
		return unknown();
	}
	return oss.str();
}

#endif
