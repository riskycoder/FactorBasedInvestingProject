#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SUBSET_SIZE 450		//Size of stock selection
#define PERIODICITY 1		//Number of periods in a year, eg Quarterly = 4
#define NUM_YEARS 31		//Set to number of years included in the analysis
#define DLT1 1.00			//Must be non-negative - Determines how much the gradient value is multiplied with
#define DLT2 1.00			//Must be non-negative - Determines how far the the program goes to evaluate the gradient
#define EPSLN 0.00			//Must be non-negative - Threshold error for algorithm to stop
#define SIZE 5				//Must be a positive integer - Number of factors
#define RF_RATE 0.025		//Risk-Free rate

#endif