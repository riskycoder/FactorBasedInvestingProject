#ifndef BLOOMBERG_H
#define BLOOMBERG_H

#include <blpapi_session.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <ctime>
#include "definitions.h"

class HistoricData {
	private:
		std::string         APIREFDATA_SVC;
		std::string         d_host;
		int                 d_port;

	public:
		void run(std::vector<std::string> companyNames, const char* startYear, const char* endYear, std::ostream& out, std::vector<std::vector<double>> &retVal);
		HistoricData();
};

#endif
