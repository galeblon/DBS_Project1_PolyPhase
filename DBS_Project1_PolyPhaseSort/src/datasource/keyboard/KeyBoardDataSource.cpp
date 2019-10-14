/*
 * KeyBoardDataSource.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#include <datasource/keyboard/KeyBoardDataSource.h>

void KeyBoardDataSource::InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg, int verbosity_level){
	this->verbosity_level = verbosity_level;
	std::istringstream(arg) >> this->recordsToGenerate;
	Record rec_cont;
	//Initial run to start the fill
	rec_cont = getRunFromKeyBoard(*tapes[0], rec_cont);
	int cycle_tape_index = 0;

	while(this->recordsToGenerate > 0){
		for(int i=0; i<numOfTapes-1; i++){
			if(i == cycle_tape_index)
				continue;
			int runsToGet = tapes[cycle_tape_index]->runs;
			while(runsToGet > 0 && this->recordsToGenerate > 0){
				int old_runs = tapes[i]->runs;
				rec_cont = getRunFromKeyBoard(*tapes[i], rec_cont);
				if(old_runs != tapes[i]->runs)
					runsToGet--;
			}
			if(this->recordsToGenerate == 0){
				tapes[i]->runsDummy = runsToGet;
			}
		}
		cycle_tape_index = (cycle_tape_index+1)%(numOfTapes-1);
	}

}

Record KeyBoardDataSource::askForRecord(){
	double a, b, c;
	std::cin >> a >> b >> c;
	if((b*b -4*a*c) < 0)
		throw "Wrong input data";
	return Record(a, b, c);
}

Record KeyBoardDataSource::getRunFromKeyBoard(Tape& tape, Record rec_cont){
	Record rec_prev, rec_curr;
	bool first = true;
	while(this->recordsToGenerate > 0){
		if(first && rec_cont.isValid())
			rec_curr = rec_cont;
		else
			rec_curr = askForRecord();

		if(!first)
			// Ordered by smallest first
			if(rec_prev.GetKey() > rec_curr.GetKey())
				return rec_curr;
		if(rec_curr.isValid()){
			if(first &&
					((tape.head.isValid() && tape.head.GetKey() > rec_curr.GetKey())
					|| !tape.head.isValid()))
				tape.runs++;
			tape.WriteRecord(rec_curr);
			this->recordsToGenerate--;
		}
		rec_prev = rec_curr;
		first = false;
	}
	return Record();
}
