#include <iomanip>
#include "csvio.h"
#include "bloomberg.h"
#include "vector.h"

std::vector <std::vector< std::vector<double>>> dataset;
std::vector <std::vector <std::string>> constituents;


const char* filenames[SIZE + 1] = { "..\\..\\..\\PToB.csv", "..\\..\\..\\PToE.csv",
	"..\\..\\..\\MarketCap.csv", "..\\..\\..\\Volatility.csv",
	"..\\..\\..\\CapitalExp.csv", "..\\..\\..\\PriceChange.csv" };
	

//const char* filenames[SIZE + 1] = { "..\\..\\..\\Volatility.csv" };

using namespace BloombergLP;
using namespace blpapi;

const double shrinkFactor[SIZE + 1] = { 1, 1, 1000, 1, 1000, 1};

double obj_fn_test(double(&pos)[SIZE]) {
    return -pow(pos[0], 2) - pow(pos[1], 2); // f(x) = -x1^2 - x2^2
}

bool sortcol(const std::vector<double>& v1, const std::vector<double>& v2) {
	return v1[1] > v2[1];
}

double backtest(double(&pos)[SIZE], bool verbose) {
	if(verbose) std::cout << "\n\n\nSUMMARY:\n" << std::endl;

	std::vector< std::vector< double>> scores;
	std::vector<double> returns;
	int NUM_PERIODS = dataset[0].size();

	for (int i = 0; i < NUM_PERIODS; i++)
	{
		int NUM_EQUITIES = dataset[0][i].size() - 1;

		if (verbose) std::cout << "\nIn Q" << i% PERIODICITY + 1
			<< " " << 1990 + i/ PERIODICITY
			<< " you invest as follows:" << std::endl;

		//Create a vector of NUM_EQUITIES vectors to store scores and corresponding index in dataset[0][i]
		for (int j = 0; j < NUM_EQUITIES; j++) {
			std::vector<double> temp;
			//First element in every vector is j - represents index number
			temp.push_back(j);
			scores.push_back(temp);
		}
		
		//Evaluate the score for each of the equities
		for (int x = 0; x < NUM_EQUITIES; x++) {
			scores[x].push_back(0);
			for (int a = 0; a < SIZE; a++) {
				//Missing values were set to FLT_MAX when retrieving data from Bloomberg
				//If a missing value is found, score is set to 0
				if (dataset[a][i][x+1] > FLT_MAX/10000000) {
					scores[x][1] = 0;
					break;
				}
				scores[x][1] += (dataset[a][i][x+1] * pos[a]);
			}
		}

		//Sort the list based on scores
		std::sort(scores.begin(), scores.end(), sortcol);

		//Print stock selection for current period
		if (i < NUM_PERIODS) {
			for (int z = 0; z < SUBSET_SIZE; z++) {
				if (verbose) std::cout
					<< constituents[i][(int)scores[z][0] + 1]
					//<< "\tScore = " << scores[z][1]
					//<< "Return = " << dataset[SIZE][i][(int)scores[z][0] + 1] << "\n"
					<< (z == SUBSET_SIZE - 1 ? "\n" : "\t");
			}
		}

		//Calculate returns for current period
		if (i < NUM_PERIODS - PERIODICITY) {
			returns.push_back(0);
			for (int z = 0; z < SUBSET_SIZE; z++) {
				returns.back() += dataset[SIZE][i][(int)scores[z][0] + 1];
			}
			returns.back() /= SUBSET_SIZE;
		}

		if (i < NUM_PERIODS - PERIODICITY && verbose) std::cout
			<< "Average return in the following year would be "
			<< (returns.back() - 1) * 100 << "%" << std::endl;
		scores.clear();
	}


	double realizedGrowth[PERIODICITY] = { 1 };
	double sum_squares[PERIODICITY] = { 0 };
	double avg_returns[PERIODICITY] = { 0 };
	double sharpe_ratio_sum = 0;

	for (int j = 0; j < PERIODICITY; j++) {
		double periodReturn;
		std::cout << std::fixed << std::setprecision(2);
		if (verbose) std::cout << "\n\n$1 invested in Q" << j + 1 << " 1990 grows as follows:\n1.00\t";
		int count = 0;
		int numTerms = 0;
		
		for (auto i = returns.begin(); i != returns.end(); i++) {
			if (count % PERIODICITY == j) {
				realizedGrowth[j] *= (*i);
				if (verbose) std::cout << std::fixed << realizedGrowth[j] << '\t';
				avg_returns[j] += (*i - 1);
				numTerms++;
			}
			count++;
		}
		avg_returns[j] /= numTerms;

		for (auto i = returns.begin(); i != returns.end(); i++) {
			if (count % PERIODICITY == j) {
				sum_squares[j] += pow(*i - 1 - avg_returns[j], 2);
			}
		}

		periodReturn = (pow(realizedGrowth[j], 1.00 / numTerms) - 1);

		double sd = pow(sum_squares[j]/numTerms, 0.5);
		double period_sharpe_ratio = (avg_returns[j] - RF_RATE) / sd;
		if (verbose) std::cout << "\n\nArithmetic mean return would be "
			<< avg_returns[j]*100 << "%" << "\n"
			<< "Geometric mean return would be "
			<< periodReturn * 100 << "%" << "\n"
			<< "Standard Deviaiton is " << sd*100 << "%"
			<< " Sharpe Ratio would be " << period_sharpe_ratio << std::endl;

		sharpe_ratio_sum += period_sharpe_ratio;
	}

	if (verbose) std::cout << "\nBacktesting complete"<< std::endl;
	return sharpe_ratio_sum / PERIODICITY;
}

double obj_fn_main(double(&pos)[SIZE]) {
	return backtest(pos, false);
}

double summary(double(&pos)[SIZE]) {
	backtest(pos, true);
	return -1;
}

void addData(std::vector<std::vector<double>> fld) {
	for(auto i = fld.begin(); i!= fld.end(); i++){
		for (int j = 0; j < SIZE + 1; j++) {
			std::ofstream file;
			file.open(filenames[j], std::fstream::app);
			file << i->at(j) << ",";
			file.close();
		}
	}
	for (int j = 0; j < SIZE + 1; j++) {
		std::ofstream file;
		file.open(filenames[j], std::fstream::app);
		file << "\n";
		file.close();
	}
}

int main(void) {

	/*
	* RETRIEVING S&P 500 CONSTITUENTS
	*/
	CSVReader* reader = new CSVReader;
	reader->readFromFile("..\\..\\..\\Constituents.csv", constituents);

	/*
	* POPULATING DATA FROM BLOOMBERG
	*/

	/*
	//Print constituents for testing
	for (auto i = constituents.begin(); i != constituents.end(); i++) {
		for (auto j = i->begin(); j != i->end(); j++) {
			std::cout << *j << "\t";
		}
		std::cout << std::endl;
	}
	*/

	/*
    HistoricData data;
	std::vector<double> head;

	int numCompleteRows = 6;
	for (int i = numCompleteRows; i < constituents.size() + 6; i++) {
		std::cout << "Progress: " << (double)i * 100.00 / constituents.size() << "%" << std::endl;
		std::vector<std::vector<double>> yearData;
		int date = (1990 + i/4) * 10000 + std::stod(constituents[i][0]);

		for (int i = 0; i < SIZE + 1; i++) {
			head.push_back((double)date);
		}
		yearData.push_back(head);
		head.clear();

        std::string startYear = std::to_string(date);
		std::string endYear = std::to_string(date + 10000);
		data.run(constituents[i], startYear.c_str(), endYear.c_str(), std::cout, yearData);


		//addData(yearData);
		std::cout << "Added data for " << std::fixed << std::setprecision(0) << date << std::endl;
		
		yearData.clear();
    }

	/*
	* PERFORMING GRADIENT DESCENT
	*/

	
	//Import data from files
	for (int i = 0; i < SIZE + 1; i++) {
		std::vector< std::vector<double>> data;
		reader->readFromFile(filenames[i], data, shrinkFactor[i]);
		dataset.push_back(data);
		data.clear();
	}
	
	double pt1[SIZE] = { 0, 0, 0, 0, 0 };
	double pt2[SIZE] = { 1, -1, 1, -1, 1 };
	//double pt2[SIZE] = { 1, 0, 0, 0, 0 };
	//double pt2[SIZE] = { -0.612104,-0.589133,0.0720451,0.111108,0.522917 };
	//double pt2[SIZE] = { 0.301323,0.186751,0.202712,0.447677,0.795501 };
	//double pt2[SIZE] = { -0.50471,0.75995,0.02320,-0.14100,0.38383 };
	//double pt2[SIZE] = { -0.82,0.45,-0.11,-0.13,0.30 };

    Vector cur_pos(pt2);						// Starting position
	Vector new_pos(pt1);
    Vector grad(pt1);							// Initial gradient vector
	Vector zero(pt1);


    int i = 0;
	int count = 0;
	double n_obj = 0;							// Stores the objective value at current position
	double o_obj = 0;
	double scale = 10;


	cur_pos.normalize();
	
	std::cout << "Function value at " << cur_pos << " is " << cur_pos.value(obj_fn_main) << std::endl;

	while (true) {
		cur_pos.gradient(grad, obj_fn_main, i%SIZE, scale);								// Evaluate gradient vactor at current position

		o_obj = cur_pos.value(obj_fn_main);												// Calculate new objective value

		new_pos = cur_pos;																// Add DLT1st multiple of gradient vector to current position
		new_pos += grad * DLT1 * scale;
		new_pos.normalize();
		n_obj = new_pos.value(obj_fn_main);												// Calculate new objective value
		std::cout << "Function value at " << new_pos << " is " << n_obj << std::endl;

		cur_pos = n_obj > o_obj ? new_pos : cur_pos;
		count = n_obj > o_obj ? 0 : count + 1;
		std::cout << "New position is " << cur_pos << "\n" << std::endl;
		i++;

		if (count == SIZE) {
			count = 0;
			scale *= 0.0032;
		}
		if (scale < 0.01) break;														// See if threshold is met - if yes then break
	}

	std::cout << "Optimal value is " << o_obj;
	std::cout << " and it is attained at the point " << std::setprecision(5) << cur_pos << std::endl;
	cur_pos.value(summary);

	/*
	* KEEPING TERMINAL OPEN
	*/

	std::cout << "Press ENTER to quit" << std::endl;
	char dummy[2];
	std::cin.getline(dummy, 2);

    return 0;
}
