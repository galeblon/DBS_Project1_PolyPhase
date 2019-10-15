//============================================================================
// Name        : DBS_Project1_PolyPhaseSorter.cpp
// Author      : galeblon
// Version     : 1.0.1
// Copyright   : Free as in green pepper.
// Description : PolyPhase sorting program.
//============================================================================

#include "./tape/Tape.h"
#include "datasource/DataSource.h"
#include "datasource/file/FileDataSource.h"
#include "datasource/random/RandomDataSource.h"
#include "datasource/keyboard/KeyBoardDataSource.h"
#include "VerbosityLevels.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool sortPolyPhaseSinglePhase(Tape* tapes[], int tapeNum, int verbose);
void merging(Tape* tapes[], int tapeNum, int outTape, int tapesMask);
void printPhase(Tape* tapes[], int tapeNum, int verbose);
void printHelp();
void getDiskUsage(Tape** tapes, int tapeNum, int* dr, int* dw);

int main(int argc, char** argv) {
	std::srand(time(NULL));
	if(argc < 4){
		printHelp();
		return 0;
	}

	// Number of tapes used in sorting (N-1 used for distribution, 1 for merging
	int tapeNum;
	// Datasource that will feed the tapes records
	DataSource* src;
	std::string arg;

	int verbosityLevel;

	try{
		std::istringstream(argv[1]) >> tapeNum;
		if(tapeNum < 3)
			throw "Not enough tapes";
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
		if(argc >= 5)
			std::istringstream(argv[4]) >> verbosityLevel;
	} catch(...){
		std::cerr << "Exception during parsing of arguments\n";
		return 1;
	}

	Tape** tapes = new Tape*[tapeNum];
	std::stringstream ss;
	for(int i=0; i<tapeNum; i++){
		ss << i+1;
		tapes[i] = new Tape(ss.str());
		ss.str("");
	}

	// Stage 1. distribution
	try{
		if(verbosityLevel >= VERBOSITY_NORMAL)
			std::cout << "\n\nInput file:\n";
		src->InitialDistribution(tapes, tapeNum, arg, verbosityLevel);
		delete src;
	} catch (char const* err){
		std::cerr << "Exception during initial distribution: "<< err << '\n';
		return 2;
	}

	// Reset heads
	for(int i=0; i<tapeNum; i++){
		tapes[i]->head = Record();
	}

	std::cout << "\n\nPhase 0 (After distribution)\n";
	printPhase(tapes, tapeNum, verbosityLevel);

	// Stage 2. merging
	// Repeat merging phase until we're left with one run.
	int dr=0, dw=0;
	int phase=1;
	do{
		getDiskUsage(tapes, tapeNum, &dr, &dw);
		std::cout << "\n\nPhase " << phase << "\t| Disk usage so far: W:" << dw << " R:" << dr << ":\n";
		phase++;
	} while(!sortPolyPhaseSinglePhase(tapes, tapeNum, verbosityLevel));

	getDiskUsage(tapes, tapeNum, &dr, &dw);
	std::cout << "\n\nDisk usage total: W:" << dw << " R:" << dr << '\n';
	// Print the sorted file
	if(verbosityLevel >= VERBOSITY_NORMAL){
		std::cout << "\n\nSorted file:\n";
		for(int i=0; i<tapeNum; i++)
			if(tapes[i]->runs ==  1){
				tapes[i]->printContents();
				break;
			}
	}

	// Cleanup
	for(int i=0; i<tapeNum; i++)
		delete tapes[i];
	delete[] tapes;

	return 0;
}

bool sortPolyPhaseSinglePhase(Tape* tapes[], int tapeNum, int verbose){
	// Find empty tape
	int outputTapeIndex;
	for(int i=0; i<tapeNum;i++)
		if(tapes[i]->runs == 0){
			outputTapeIndex = i;
			break;
		}

	// Find how many merges will be run
	int merges = 0;
	bool mergesSet = false;
	for(int i=0; i<tapeNum; i++)
		if((!mergesSet || merges > (tapes[i]->runs)) && tapes[i]->runs != 0 && tapes[i]->runsDummy == 0){
				merges = tapes[i]->runs;
				mergesSet = true;
		}

	while(merges > 0 && mergesSet){
		int tapesMask = 0;
		for(int i=0; i<tapeNum; i++){
			if(tapes[i]->runsDummy > 0){
				tapes[i]->runsDummy--;
				continue;
			}
			if(i != outputTapeIndex && tapes[i]->runs > 0){
				tapes[i]->runs--;
				tapesMask |= 1<<i;
			}
		}

		tapes[outputTapeIndex]->runs++;
		// Merge using tapes allowed by the mask
		merging(tapes, tapeNum, outputTapeIndex, tapesMask);
		merges--;
	}
	tapes[outputTapeIndex]->head = Record();
	printPhase(tapes, tapeNum, verbose);
	int nonZeroTapes = 0;
	for(int i=0; i<tapeNum; i++)
		nonZeroTapes += tapes[i]->runs > 0 ? 1 : 0;

	if(nonZeroTapes == 1)
		// Sorted
		return true;
	return false;
}


void merging(Tape* tapes[], int tapeNum, int outTape, int tapesMask){
	int endRunMask = 0 | (1 << outTape);
	int loadNewMask = 0;
	Record min;
	int min_index;
	while(true){
		for(int i=0; i<tapeNum; i++){
			// If this tape is supposed to be used this merge and has yet ended.
			if((tapesMask & 1<<i) && !(endRunMask & 1<<i)){
				Record curr_rec;
				if(!tapes[i]->head.isValid()){
					// Try to read record, if can't then we are at end
					curr_rec = tapes[i]->readRecord();
					if(!curr_rec.isValid()){
						endRunMask |= 1<<i;
						continue;
					}
				} else {
					// There is already valid record in head
					// Check if it was already used this merge
					if(loadNewMask & 1<<i){
						// It was already used, try to load a new record. Check if run ends
						Record old_rec = tapes[i]->head;
						curr_rec = tapes[i]->readRecord();
						if(!curr_rec.isValid() || old_rec.getKey() > curr_rec.getKey()){
							// Run has been broken
							endRunMask |= 1<<i;
							loadNewMask &= !(1<<i);
							continue;
						}
						loadNewMask &= !(1<<i);
					} else {
						// It wasn't yet used, no need to load new one
						curr_rec = tapes[i]->head;
					}
				}
				if(!min.isValid() || min.getKey() > curr_rec.getKey()){
					// New min
					min = curr_rec;
					min_index = i;
				}
			}
		}
		if(!min.isValid())
			// It appears we are done here
			break;
		tapes[outTape]->writeRecord(min);
		loadNewMask |= 1<<min_index;
		min = Record();
	}
}


void printPhase(Tape* tapes[], int tapeNum, int verbose){
	for(int i=0; i<tapeNum; i++){
			std::cout << tapes[i]->getName() << ' ' << tapes[i]->runs+tapes[i]->runsDummy << '(' << tapes[i]->runsDummy <<") runs";
		if(verbose >= VERBOSITY_DETAILED){
			std::cout << "|| contents: \n";
			tapes[i]->printContents();
		} else
			std::cout << '\n';
	}
}


void printHelp(){
	std::cout << "This is a simple program presenting the PolyPhase merging algorithm used on records in form of coefficients of polynomial of degree 2, with delta >= 0\n"
			  << "Program usage:\n"
			  << "programName [numberOfTapes] [dataSource] [arg] [verbosity*]\n"
			  << "\tnumberOfTapes - number of tapes used during the algorithm. Minimum 3 tapes required (note only N-1 tapes take part in merging at any time)\n"
			  << "\tdataSource - only takes 'F' - file, 'R' - random generated, 'K' - standard input\n"
			  << "\targ - if file data source is selected provide file name containing records, otherwise provide number of records to generate/read from input\n"
			  << "\tverbosity optional parameter, if present will set information printing verbosity:\n"
			  << "\t\t0 - Default value, just prints number of runs in each phase.\n"
			  << "\t\t1 - Everything in level 0 plus prints the contents to sort before and after sort\n"
			  << "\t\t2 - Everything in level 1 plus prints the contents of tapes in each phase\n";
}

void getDiskUsage(Tape** tapes, int tapeNum, int* dr, int* dw){
	*dr = 0;
	*dw = 0;
	for(int i=0; i<tapeNum; i++){
		*dr += tapes[i]->getDiskReads();
		*dw += tapes[i]->getDiskWrites();
	}
}
