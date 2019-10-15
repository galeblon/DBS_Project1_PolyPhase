/*
 * FileDataSource.cpp
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#include <datasource/file/FileDataSource.h>

void FileDataSource::InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg, int verbosity_level){
	this->verbosityLevel = verbosity_level;
	std::fstream baseFile;
	baseFile.open(arg.c_str(), std::ios::in);
	Record rec_cont;
	//Initial run to start the fill
	rec_cont = getRunFromFile(baseFile, *tapes[0], rec_cont);
	int cycle_tape_index = 0;

	while(!baseFile.eof()){
		for(int i=0; i<numOfTapes-1; i++){
			if(i == cycle_tape_index)
				continue;
			int runsToGet = tapes[cycle_tape_index]->runs;
			while(runsToGet > 0 && !baseFile.eof()){
				int old_runs = tapes[i]->runs;
				rec_cont = getRunFromFile(baseFile, *tapes[i], rec_cont);
				if(old_runs != tapes[i]->runs)
					runsToGet--;
			}
			if(baseFile.eof()){
				tapes[i]->runsDummy = runsToGet;
			}
		}
		cycle_tape_index = (cycle_tape_index+1)%(numOfTapes-1);
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
		if(this->verbosityLevel >= VERBOSITY_NORMAL)
			std::cout << a << " " << b << " " << c << '\n';
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
			if(rec_prev.getKey() > rec_curr.getKey()){
				return rec_curr;
			}
		if(rec_curr.isValid()){
			if(first &&
					((tape.head.isValid() && tape.head.getKey() > rec_curr.getKey())
					|| !tape.head.isValid()))
				tape.runs++;
			tape.writeRecord(rec_curr);
		}
		rec_prev = rec_curr;
		first = false;
	}
	return Record();
}
