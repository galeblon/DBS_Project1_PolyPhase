/*
 * RandomDataSource.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#include <datasource/random/RandomDataSource.h>

void RandomDataSource::InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg){
	std::istringstream(arg) >> this->recordsToGenerate;
	Record rec_cont;
	//Initial run to start the fill
	rec_cont = getRunFromRandom(*tapes[0], rec_cont);
	int cycle_tape_index = 0;

	while(this->recordsToGenerate > 0){
		for(int i=0; i<numOfTapes-1; i++){
			if(i == cycle_tape_index)
				continue;
			int runsToGet = tapes[cycle_tape_index]->runs;
			while(runsToGet > 0 && this->recordsToGenerate > 0){
				int old_runs = tapes[i]->runs;
				rec_cont = getRunFromRandom(*tapes[i], rec_cont);
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


Record RandomDataSource::generateRecord(){
	int COEFFICIENT_MAX = 100;
	double a = 1;
	double b = 0;
	double c = 1;
	while((b*b - 4*a*c) < 0){
		a = -COEFFICIENT_MAX + (double)std::rand() / RAND_MAX * COEFFICIENT_MAX * 2;
		b = -COEFFICIENT_MAX + (double)std::rand() / RAND_MAX * COEFFICIENT_MAX * 2;
		c = -COEFFICIENT_MAX + (double)std::rand() / RAND_MAX * COEFFICIENT_MAX * 2;
	}
	return Record(a, b, c);
}

Record RandomDataSource::getRunFromRandom(Tape& tape, Record rec_cont){
	Record rec_prev, rec_curr;
	bool first = true;
	while(this->recordsToGenerate > 0){
		if(first && rec_cont.isValid())
			rec_curr = rec_cont;
		else
			rec_curr = generateRecord();

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
