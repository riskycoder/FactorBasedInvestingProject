#include "bloomberg.h"

using namespace BloombergLP;
using namespace blpapi;

const char* fields[SIZE + 1] = { "VOLATILITY_360D", "PX_TO_BOOK_RATIO",
	"PE_RATIO", "CUR_MKT_CAP", "CAPITAL_EXPEND", "PX_LAST" };

void HistoricData::run(std::vector<std::string> companyNames, const char* startYear,
	const char* endYear, std::ostream& out, std::vector<std::vector<double>> &retVal){

	out << "Num equities = " << companyNames.size() << std::endl;
	d_host = "localhost";
	d_port = 8194;

	SessionOptions sessionOptions;
	sessionOptions.setServerHost(d_host.c_str());
	sessionOptions.setServerPort(d_port);

	//Initiating connection with Bloomberg
	Session session(sessionOptions);
	if (!session.start()) {
		std::cerr << "Failed to start session." << std::endl;
	}
	if (!session.openService(APIREFDATA_SVC.c_str())) {
		std::cerr << "Failed to open //blp/refdata" << std::endl;
	}

	Service refDataService = session.getService(APIREFDATA_SVC.c_str());
	Request request = refDataService.createRequest("HistoricalDataRequest");


	//Append security names to request
	//Start at ++companyNames.begin() since the first element in the vector will be the quarter data

	int j = 0;
	for (auto i = ++companyNames.begin(); i != companyNames.end(); i++) {
		j++;
		request.getElement("securities").appendValue(i->c_str());
		//out << j << ") " << i->c_str() << std::endl;
	}

	//Append request fields
	for (int i = 0; i < SIZE+1; i++) {
		request.getElement("fields").appendValue(fields[i]);
	}

	//Set optional parameters
	request.set("periodicitySelection", "YEARLY");
	request.set("periodicityAdjustment", "ACTUAL");
	request.set("startDate", startYear);
	request.set("endDate", endYear);

	session.sendRequest(request);

	//Process response
	while (true) {
		Event event = session.nextEvent();
		MessageIterator msgIter(event);
		int count = 0;
		while (msgIter.next()) {
			Message msg = msgIter.message();
			msg.asElement().print(out);
			if (!strcmp(msg.messageType().string(), "HistoricalDataResponse")) {
				//if (count % 50 == 0) msg.asElement().print(out);
				count++;
				//Append individual fields
				std::vector<double> fld;
				for (int i = 0; i < SIZE; i++) {
					try {
						fld.push_back(msg.getElement(Name("securityData"))
							.getElement(Name("fieldData")).getValueAsElement((size_t)0)
							.getElement(fields[i]).getValueAsFloat64());
					}
					catch(Exception& e){
						//std::cerr << "Library Exception!!! " << e.description() << std::endl;
						fld.push_back(FLT_MAX);
					}
				}
				//Append price change
				try {
					fld.push_back(msg.getElement(Name("securityData"))
						.getElement(Name("fieldData")).getValueAsElement((size_t)1)
						.getElement("PX_LAST").getValueAsFloat64()/
						msg.getElement(Name("securityData"))
						.getElement(Name("fieldData")).getValueAsElement((size_t)0)
						.getElement("PX_LAST").getValueAsFloat64());
				}
				catch (Exception& e) {
					//std::cerr << "Library Exception!!! " << e.description() << std::endl;
					fld.push_back(0);
				}
				retVal.push_back(fld);
			}
		}
		if (event.eventType() == Event::RESPONSE) {
			break;
		}
	}

	//std::cout << "Length fo return value is " << retVal.size() << std::endl;
}

HistoricData::HistoricData() : APIREFDATA_SVC("//blp/refdata") {}

