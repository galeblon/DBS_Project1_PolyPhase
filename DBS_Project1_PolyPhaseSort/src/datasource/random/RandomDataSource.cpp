/*
 * RandomDataSource.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#include <datasource/random/RandomDataSource.h>

void RandomDataSource::InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg){
	std::istringstream(arg) >> this->recordsToGenerate;
	int fib1 = 1;
	int fib2 = 1;
	Record rec_cont;
	while(this->recordsToGenerate > 0){
		for(int i=0; i<numOfTapes-1; i++){
			while(tapes[i]->runs < fib2 && this->recordsToGenerate > 0){
				rec_cont = getRunFromRandom(*tapes[i], rec_cont);
			}
			if(this->recordsToGenerate == 0){
				tapes[i]->runsDummy = fib2 - tapes[i]->runs;
				break;
			}
			fib2 += fib1;
			fib1 = fib2 - fib1;
		}
	}
}


Record RandomDataSource::generateRecord(){
	int COEFFICIENT_MAX = 100;
	double a = 1;
	double b = 0;
	double c = 1;
	while((b*b - 4*a*c) < 0){
		a = (double)std::rand() / RAND_MAX * COEFFICIENT_MAX;
		b = (double)std::rand() / RAND_MAX * COEFFICIENT_MAX;
		c = (double)std::rand() / RAND_MAX * COEFFICIENT_MAX;
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
