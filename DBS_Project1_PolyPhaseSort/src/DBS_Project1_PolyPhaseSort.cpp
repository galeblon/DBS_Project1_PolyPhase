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

Record loadFromFile(fstream& fs);
Record getRunFromFile(fstream& fs, Tape& tape, Record rec_cont);

void InitialDistribution(Tape tapes[], int numOfTapes, fstream& baseFile);
void InitialDistribution(Tape tapes[], int numOfTapes, int numOfRecordsToGenerate);
void InitialDistribution(Tape tapes[], int numOfTapes);


int main() {
	Tape tapes[TAPE_NUM] = {Tape(std::string("1"), true), Tape(std::string("2"), true), Tape(std::string("3"), true)};
	fstream baseFile;
	baseFile.open("records", std::ios::in);
	InitialDistribution(tapes, TAPE_NUM, baseFile);
	baseFile.close();

	for(int i=0; i<TAPE_NUM; i++){
		tapes[i].printContents();
	}

	/*
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
	*/
	return 0;
}

void InitialDistribution(Tape tapes[], int numOfTapes, fstream& baseFile){
	int fib1 = 1;
	int fib2 = 1;
	Record rec_cont;
	while(!baseFile.eof()){
		while(tapes[0].runs < fib2 && !baseFile.eof()){
			rec_cont = getRunFromFile(baseFile, tapes[0], rec_cont);
		}
		if(baseFile.eof()){
			tapes[0].runsDummy = fib2 - tapes[0].runs;
			break;
		}
		fib2 += fib1;
		fib1 = fib2 - fib1;
		while(tapes[1].runs < fib2 && !baseFile.eof()){
			rec_cont = getRunFromFile(baseFile, tapes[1], rec_cont);
		}
		if(baseFile.eof()){
			tapes[1].runsDummy = fib2 - tapes[1].runs;

			break;
		}
		fib2 += fib1;
		fib1 = fib2 - fib1;
	}
	if(tapes[1].runsDummy > 0){
		tapes[1].runsDummy += tapes[0].runs;
	}
}

Record loadFromFile(fstream& fs){
	double a, b, c;
	std::string line;
	std::getline(fs, line);
	if(line != ""){
		std::stringstream ss(line);
		ss >> a >> b >> c;
		return Record(a, b, c);
	}
	return Record();
}

Record getRunFromFile(fstream& fs, Tape& tape, Record rec_cont){
	Record rec_prev, rec_curr;
	bool first = true;
	while(!fs.eof()){
		if(first && rec_cont.isValid())
			rec_curr = rec_cont;
		else
			rec_curr = loadFromFile(fs);

		if(!first)
			// Ordered by smallest first
			if(rec_prev.GetKey() > rec_curr.GetKey()){
				//tape.runs++;
				return rec_curr;
			}
		if(rec_curr.isValid()){
			if(first && ((tape.head.isValid() && tape.head.GetKey() > rec_curr.GetKey()) || !tape.head.isValid()))
				tape.runs++;
			tape.WriteRecord(rec_curr);
		}
		rec_prev = rec_curr;
		first = false;
	}
	//tape.runs++;
	return Record();
}
