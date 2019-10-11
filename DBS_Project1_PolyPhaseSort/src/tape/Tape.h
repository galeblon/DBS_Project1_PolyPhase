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

#define BUFFOR_SIZE 50

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

	int getDiskWrites(){ return this->diskWrites;}
	int getDiskReads(){ return this->diskReads;}

	Record head;
	int runs;
	int runsDummy;

private:
	void LoadBufforInternal();
	void SaveBufforInternal();
	Record ReadRecordInternal();
	void WriteRecordInternal(Record record);

	double printBufferReadMode();
	void printBufferWriteMode(bool first, double prev_key);


	std::fstream ws;
	std::fstream rs;

	std::string name;
	void GotoLine(unsigned int pos);
	bool debug;
	bool endReached;
	unsigned int currPosition;
	bool readMode;

	Record buffor[BUFFOR_SIZE];
	int buffer_pointer;
	bool buffer_loaded;

	int diskReads;
	int diskWrites;
};

#endif /* TAPE_TAPE_H_ */
