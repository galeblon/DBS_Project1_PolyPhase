/*
 * KeyBoardDataSource.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#include <datasource/keyboard/KeyBoardDataSource.h>

void KeyBoardDataSource::InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg){
	std::istringstream(arg) >> this->recordsToGenerate;
	int fib1 = 1;
	int fib2 = 1;
	Record rec_cont;
	while(this->recordsToGenerate > 0){
		for(int i=0; i<numOfTapes-1; i++){
			while(tapes[i]->runs < fib2 && this->recordsToGenerate > 0){
				rec_cont = getRunFromKeyBoard(*tapes[i], rec_cont);
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
