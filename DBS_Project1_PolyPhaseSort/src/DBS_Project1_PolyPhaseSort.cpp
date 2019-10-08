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
#include <./tape/Tape.h>
using namespace std;

#define TAPE_NUM 3

int main() {
	Tape tapes[TAPE_NUM] = {Tape(std::string("1"), true), Tape(std::string("2"), true), Tape(std::string("3"), true)};
	int cycle = 0;
	fstream baseFile;
	baseFile.open("records", std::ios::in);
	while(!baseFile.eof()){
		double a, b, c;

		std::string line;
		std::getline(baseFile, line);
		if(line != ""){
			std::stringstream ss(line);
			ss >> a >> b >> c;
			Record rec =  Record(a, b, c);
			tapes[cycle].WriteRecord(rec);
			cycle = (cycle+1)%3;
		}
	}
	int i=1;
	while(!tapes[0].IsAtEnd()){
		std::cout << "||||Phase " << i << "\n";
		i++;
		Record rec = tapes[0].ReadRecord();
		if(tapes[0].IsAtEnd())
			break;
		tapes[1].WriteRecord(rec);

		for(int i=0; i<TAPE_NUM; i++){
			tapes[i].printContents();
		}
	}
	return 0;
}
