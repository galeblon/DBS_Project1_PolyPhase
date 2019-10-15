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
#include <iomanip>
#include <sstream>
#include <limits>
#include "../record/Record.h"

#define BUFFER_SIZE 30
#define RECORD_SIZE (3*sizeof(double))
#define BUFFER_SIZE_BYTES (BUFFER_SIZE*RECORD_SIZE)

class Tape {
public:
	Tape();
	Tape(const Tape&);
	Tape(std::string id);

	virtual ~Tape();

	int getDiskWrites(){ return this->diskWrites;}
	int getDiskReads(){ return this->diskReads;}
	int getRecordWrites(){ return this->recordWrites;}
	int getRecordReads(){ return this->recordReads;}

	std::string getName() {return this->name;}

	void printContents();

	bool isAtEnd(){ return this->endReached;}

	Record readRecord();
	void writeRecord(Record record);

	Record head;
	int runs;
	int runsDummy;

private:
	void loadBufferInternal();
	void saveBufferInternal();

	Record readRecordInternal();
	void writeRecordInternal(Record record);

	double printBufferReadMode();
	void printBufferWriteMode(bool first, double prev_key);

	void printSingleRecord(Record rec);

	void gotoLine(unsigned int pos);

	std::fstream ws;
	std::fstream rs;

	std::string name;

	bool endReached;
	unsigned int currPosition;
	bool readMode;

	Record buffer[BUFFER_SIZE];
	int buffer_pointer;
	bool buffer_loaded;

	int diskReads;
	int diskWrites;
	int recordReads;
	int recordWrites;
};

#endif /* TAPE_TAPE_H_ */
