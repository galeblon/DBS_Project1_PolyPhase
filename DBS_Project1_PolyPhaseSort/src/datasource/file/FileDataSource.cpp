/*
 * FileDataSource.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#include <datasource/file/FileDataSource.h>

void FileDataSource::InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg){
	std::fstream baseFile;
	baseFile.open(arg.c_str(), std::ios::in);
	int fib1 = 1;
	int fib2 = 1;
	Record rec_cont;
	while(!baseFile.eof()){
		for(int i=0; i<numOfTapes-1; i++){
			while(tapes[i]->runs <fib2 && !baseFile.eof()){
				rec_cont = getRunFromFile(baseFile, *tapes[i], rec_cont);
			}
			if(baseFile.eof()){
				tapes[i]->runsDummy = fib2 - tapes[i]->runs;
				break;
			}
			fib2 += fib1;
			fib1 = fib2 - fib1;
		}
	}
	baseFile.close();
}

Record FileDataSource::loadFromFile(std::fstream& fs){
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

Record FileDataSource::getRunFromFile(std::fstream& fs, Tape& tape, Record rec_cont){
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
				return rec_curr;
			}
		if(rec_curr.isValid()){
			if(first &&
					((tape.head.isValid() && tape.head.GetKey() > rec_curr.GetKey())
					|| !tape.head.isValid()))
				tape.runs++;
			tape.WriteRecord(rec_curr);
		}
		rec_prev = rec_curr;
		first = false;
	}
	return Record();
}
