//============================================================================
// Name        : DBS_Project1_PolyPhaseSorter.cpp
// Author      : galeblon
// Version     :
// Copyright   : Your copyright notice
// Description : PolyPhase sorting program.
//============================================================================

#include "./tape/Tape.h"
#include "datasource/DataSource.h"
#include "datasource/file/FileDataSource.h"
#include "datasource/random/RandomDataSource.h"
#include "datasource/keyboard/KeyBoardDataSource.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

bool sortPolyPhaseSinglePhase(Tape* tapes[], int tapeNum, bool verbose);
void merging(Tape* tapes[], int tapeNum, int outTape, int tapesMask);
void printPhase(Tape* tapes[], int tapeNum, bool verbose);

// TODO make tape buffer size a program parameter
// TODO print latest disk usage after do while loop
int main(int argc, char** argv) {
	std::srand(time(NULL));
	if(argc < 4){
		std::cout << "This is a simple program presenting the PolyPhase merging algorithm used on records in form of coefficients of polynomial of degree 2, with delta >= 0\n"
				  << "Program usage:\n"
				  << "programName [numberOfTapes] [dataSource] [arg] [verbose]\n"
				  << "\tnumberOfTapes - number of tapes used during the algorithm. Minimum 3 tapes required (note only N-1 tapes take part in merging at any time)\n"
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
		if(TAPE_NUM < 3)
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

	// Reset heads
	for(int i=0; i<TAPE_NUM; i++){
		tapes[i]->head = Record();
	}

	// Print state after initial distribution
	std::cout << "\n\nPhase 0 (After distribution)\n";
	printPhase(tapes, TAPE_NUM, verbose);

	// Repeat merging phase until it's sorted
	int phase = 1;
	do{
		int dr=0, dw=0;
		for(int i=0; i<TAPE_NUM; i++){
			dr += tapes[i]->getDiskReads();
			dw += tapes[i]->getDiskWrites();
		}
		std::cout << "\n\nPhase " << phase << "\t| Disk usage so far: W:" << dw << " R:" << dr << ":\n";
		phase++;
	} while(!sortPolyPhaseSinglePhase(tapes, TAPE_NUM, verbose));

	// Cleanup
	for(int i=0; i<TAPE_NUM; i++)
		delete tapes[i];
	delete[] tapes;

	return 0;
}

bool sortPolyPhaseSinglePhase(Tape* tapes[], int tapeNum, bool verbose){
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
					curr_rec = tapes[i]->ReadRecord();
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
						curr_rec = tapes[i]->ReadRecord();
						if(!curr_rec.isValid() || old_rec.GetKey() > curr_rec.GetKey()){
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
				if(!min.isValid() || min.GetKey() > curr_rec.GetKey()){
					// New min
					min = curr_rec;
					min_index = i;
				}
			}
		}
		if(!min.isValid())
			// It appears we are done here
			break;
		tapes[outTape]->WriteRecord(min);
		loadNewMask |= 1<<min_index;
		min = Record();
	}
}


void printPhase(Tape* tapes[], int tapeNum, bool verbose){
	for(int i=0; i<tapeNum; i++){
			std::cout << tapes[i]->getName() << ' ' << tapes[i]->runs+tapes[i]->runsDummy << '(' << tapes[i]->runsDummy <<") runs";
		if(verbose){
			std::cout << "|| contents: \n";
			tapes[i]->printContents();
		} else
			std::cout << '\n';
	}
}
