//============================================================================
// Name        : DBS_Project1_PolyPhaseSorter.cpp
// Author      : galeblon
// Version     :
// Copyright   : Your copyright notice
// Description : PolyPhase sorting program.
//============================================================================

#include <fstream>

#include <iostream>
#include <cstdlib>
#include "./tape/Tape.h"
#include "datasource/file/FileDataSource.h"
#include "datasource/random/RandomDataSource.h"
#include "datasource/keyboard/KeyBoardDataSource.h"
#include "datasource/DataSource.h"
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char** argv) {
	std::srand(time(NULL));
	if(argc < 4){
		std::cout << "This is a simple program presenting the PolyPhase merging algorithm used on records in form of coefficients of polynomial of degree 2, with delta >= 0\n"
				  << "Program usage:\n"
				  << "programName [numberOfTapes] [dataSource] [arg] [verbose]\n"
				  << "\tnumberOfTapes - number of tapes used during the algorithm (note only N-1 tapes take part in merging at any time)\n"
				  << "\tdataSource - only takes 'F' - file, 'R' - random generated, 'K' - standard input\n"
				  << "\targ - if file data source is selected provide file name containing records, otherwise provide number of records to generate/read from input\n"
				  << "\tverbose optional parameter if present with any value will print the tapes state after each phase\n";
		return 0;
	}

	int TAPE_NUM;
	DataSource* src;
	std::string arg;
	bool verbose;
	try{
		std::istringstream(argv[1]) >> TAPE_NUM;
		switch(argv[2][0]){
			case 'f':
			case 'F':
				src = new FileDataSource();
			break;
			case 'k':
			case 'K':
				src = new KeyBoardDataSource();
			break;
			case 'r':
			case 'R':
				src = new RandomDataSource();
			break;
		}
		arg = argv[3];
		verbose = argc >=5 ? true : false;
	} catch(...){
		std::cerr << "Exception during parsing of arguments";
		return 1;
	}

	Tape** tapes = new Tape*[TAPE_NUM];
	std::stringstream ss;
	for(int i=0; i<TAPE_NUM; i++){
		ss << i+1;
		tapes[i] = new Tape(ss.str());
		ss.str("");
	}

	try{
		src->InitialDistribution(tapes, TAPE_NUM, arg);
		delete src;
	} catch (char const* err){
		std::cerr << "Exception during initial distribution: "<< err;
		return 2;
	}

	if(verbose){
		for(int i=0; i<TAPE_NUM; i++){
			tapes[i]->printContents();
		}
	}

	// Cleanup
	for(int i=0; i<TAPE_NUM; i++)
		delete tapes[i];
	delete[] tapes;

	return 0;
}
