#include "csvio.h"

// Taken from http://www.cplusplus.com/forum/beginner/150413/
void CSVReader::readFromFile(const char* path, std::vector <std::vector <std::string>> &data){
	std::ifstream infile(path);
	while (infile) {
		std::string s;
		if (!std::getline(infile, s)) break;

		std::istringstream ss(s);
		std::vector <std::string> record;

		while (ss) {
			std::string s;
			if (!getline(ss, s, ',')) break;
			if(!s.empty()) record.push_back(s);
		}
		data.push_back(record);
		record.clear();

	}
	if (!infile.eof()) {
		std::cerr << "Error! Can't open " << path << std::endl;
	}
}

void CSVReader::readFromFile(const char* path, std::vector <std::vector <double>> &data, double r) {
	std::ifstream infile(path);
	while (infile) {
		std::string s;
		if (!std::getline(infile, s)) break;

		std::istringstream ss(s);
		std::vector <double> record;
		
		while (ss) {
			std::string s;
			if (!getline(ss, s, ',')) break;
			if (s.empty()) continue;
			double val = std::stof(s);
			record.push_back(val >= FLT_MAX/10? val : val / r);
		}
		
		data.push_back(record);
		record.clear();

	}
	if (!infile.eof()) {
		std::cerr << "Error! Can't open " << path << std::endl;
	}
}

void CSVReader::print(std::vector <std::vector <std::string>> data, std::ostream& out) {
	for (auto i = data.begin(); i != data.end(); i++) {
		for (auto j = i->begin(); j != i->end(); j++) {
			out << *j << "\t";
		}
		out << std::endl;
	}
}

void CSVWriter::writeToFile(const char* path, std::vector <std::vector <std::string>> &data) {
	std::ofstream file;
	file.open(path);
	for (auto it = data.begin(); it != data.end(); it++) {
		for (auto it2 = it->begin(); it2 != it->end(); it2++) {
			file << *it2 << ",";
		}
		file << "\n";
	}
}

void CSVWriter::writeToFile(const char* path, std::vector <std::vector <std::vector <double>>> &data, int index) {
	std::ofstream file;
	file.open(path);
	for (auto it = data.begin(); it != data.end(); it++) {
		for (auto it2 = it->begin(); it2 != it->end(); it2++) {
			file << (*it2).at(index) << ",";
		}
		file << "\n";
	}
}

