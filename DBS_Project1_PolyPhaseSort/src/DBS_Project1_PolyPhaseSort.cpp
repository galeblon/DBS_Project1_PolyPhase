//============================================================================
// Name        : DBS_Project1_PolyPhaseSorter.cpp
// Author      : galeblon
// Version     :
// Copyright   : Your copyright notice
// Description : PolyPhase sorting program.
//============================================================================

#include <iostream>
#include <cstdlib>
#include <./tape/Tape.h>
using namespace std;

int main() {
	Tape myTape(std::string("1"));
	for(int i=0; i<100; i++){
		Record rec_1 = Record((double)std::rand()/RAND_MAX, 2.9, 3.6);
		myTape.WriteRecord(rec_1);
		//Record rec_2 = myTape.ReadRecord();
	}
	return 0;
}
