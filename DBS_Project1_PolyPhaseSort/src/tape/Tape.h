/*
 * tape.h
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#ifndef TAPE_TAPE_H_
#define TAPE_TAPE_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include "../record/Record.h"

class Tape {
public:
	Tape();
	Tape(std::string id);
	Tape(std::string id, bool debug);
	Tape(const Tape&);
	virtual ~Tape();

	Record ReadRecord();
	void WriteRecord(Record record);

	void printContents();

	std::string getName() {return this->name;}
	bool IsAtEnd(){ return this->endReached;}

	Record head;
	int runs;
	int runsDummy;

private:
	std::fstream ws;
	std::fstream rs;

	std::string name;
	void GotoLine(unsigned int pos);
	bool debug;
	bool endReached;
	unsigned int currPosition;
	bool readMode;
};

#endif /* TAPE_TAPE_H_ */
