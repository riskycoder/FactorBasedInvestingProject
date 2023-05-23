#ifndef CSVREAD_H
#define CSVREAD_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

//Vector class declaration
class CSVReader {
public:
	void CSVReader::readFromFile(const char* path, std::vector <std::vector <std::string>> &data);
	void CSVReader::readFromFile(const char* path, std::vector <std::vector <double>> &data, double r);
	void print(std::vector <std::vector <std::string>> data, std::ostream& out);
};

class CSVWriter {
public:
	void CSVWriter::writeToFile(const char* path, std::vector <std::vector <std::string>> &data);
	void CSVWriter::writeToFile(const char* path, std::vector <std::vector <std::vector <double>>> &data, int index);
};
#endif