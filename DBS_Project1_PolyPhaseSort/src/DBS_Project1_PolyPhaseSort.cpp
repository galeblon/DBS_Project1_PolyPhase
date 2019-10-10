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
#include "datasource/DataSource.h"
#include <string>
#include <sstream>

using namespace std;

#define TAPE_NUM 3

void InitialDistribution(Tape tapes[], int numOfTapes);


int main() {
	//TODO set srand to time null
	Tape* tapes[TAPE_NUM];
	std::stringstream ss;
	for(int i=0; i<TAPE_NUM; i++){
		ss << i+1;
		tapes[i] = new Tape(ss.str());
		ss.str("");
	}
	FileDataSource fsrc;
	RandomDataSource rsrc;
	DataSource* src = &rsrc;
	src->InitialDistribution(tapes, TAPE_NUM, "100");

	for(int i=0; i<TAPE_NUM; i++){
		tapes[i]->printContents();
	}
	for(int i=0; i<TAPE_NUM; i++)
		delete tapes[i];

	return 0;
}
