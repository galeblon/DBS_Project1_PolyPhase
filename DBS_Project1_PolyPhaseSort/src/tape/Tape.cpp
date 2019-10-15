/*
 * tape.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#include "Tape.h"

Tape::Tape(): head(),
			  runs(0),
			  runsDummy(0),
			  endReached(0),
			  currPosition(0),
			  readMode(false),
			  buffer_pointer(0),
			  buffer_loaded(false),
			  diskReads(0),
			  diskWrites(0),
			  recordReads(0),
			  recordWrites(0){
	this->name = "Tape_default";
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
}

Tape::Tape(std::string id):  head(),
							 runs(0),
							 runsDummy(0),
							 endReached(0),
							 currPosition(0),
							 readMode(false),
							 buffer_pointer(0),
							 buffer_loaded(false),
							 diskReads(0),
							 diskWrites(0),
							 recordReads(0),
							 recordWrites(0){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
}

Tape::Tape(const Tape& other): head(),
							   runs(0),
							   runsDummy(0),
							   endReached(0),
							   currPosition(0),
							   readMode(false),
							   buffer_pointer(0),
							   buffer_loaded(false),
							   diskReads(0),
							   diskWrites(0),
							   recordReads(0),
							   recordWrites(0){
	this->name = other.name;
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
}

Tape::~Tape() {
	this->ws.close();
	this->rs.close();
}

// This function doesn't contribute to disk writes/read counters
// because it's here only for visualisation purposes.
void Tape::printContents(){
	double prev_key;
	bool first = true;
	std::string line;
	if(this->head.isValid() && !this->buffer_pointer){
		first = false;
		printSingleRecord(head);
	}
	if(this->readMode && this->buffer_loaded){
		prev_key = this->printBufferReadMode();
		if(this->buffer_pointer < BUFFER_SIZE)
			first = false;
	}
	if(!readMode)
		this->rs.seekg(0);
	while(!this->rs.eof()){
		double arr[3];
		this->rs.read((char*) arr, RECORD_SIZE);
		Record rec = Record(arr[0], arr[1], arr[2]);
		//std::cout << this->rs.eof() << rs.fail() << rs.bad() << rs.good();
		if(!this->rs.fail()){
			if(!first && prev_key > rec.getKey())
				std::cout << "|====================================================================|\n";
			this->printSingleRecord(rec);
		} else
			continue;
		prev_key = rec.getKey();
		first = false;
	}
	if(!this->readMode)
		this->printBufferWriteMode(first, prev_key);
	this->rs.clear();
	this->gotoLine(this->currPosition);
}

Record Tape::readRecord(){
	if(!this->readMode){
		// Save buffer if necessary
		this->saveBufferInternal();
		this->readMode = true;
		this->rs.seekg(0);
	}
	if(!this->buffer_loaded)
		loadBufferInternal();
	// Grab from buffer
	Record rec = this->buffer[this->buffer_pointer++];
	if(this->buffer_pointer >= BUFFER_SIZE){
		this->buffer_loaded = false;
		this->buffer_pointer = 0;
	}
	this->currPosition = this->rs.tellg();
	this->head = rec;
	this->recordReads++;
	return rec;
}

void Tape::writeRecord(Record record){
	if(this->readMode){
		this->readMode = false;
		this->endReached = false;
		this->ws.close();
		this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
		this->buffer_loaded = false;
		this->buffer_pointer =0;
	}
	if(this->buffer_pointer >= BUFFER_SIZE)
		this->saveBufferInternal();
	this->buffer[this->buffer_pointer++] = record;
	this->head = record;
	this->recordWrites++;
}

void Tape::loadBufferInternal(){
	double arr[3*BUFFER_SIZE];
	this->rs.read((char*) arr, BUFFER_SIZE_BYTES);
	int bytes_read = this->rs.gcount();
	int elements_loaded = bytes_read/RECORD_SIZE;

	for(int i=0; i<elements_loaded; i++)
		buffer[i] = Record(arr[3*i], arr[3*i+1], arr[3*i+2]);
	for(int i=elements_loaded; i<BUFFER_SIZE; i++)
		buffer[i] = Record();

	if(buffer[0].isValid()){
		this->buffer_loaded = true;
		this->diskReads++;
	}
    if(elements_loaded < BUFFER_SIZE){
			this->endReached = true;
			this->rs.clear();
	}
}

void Tape::saveBufferInternal(){
	double arr[3*BUFFER_SIZE];
	bool toSave = false;
	for(int i=0; i<this->buffer_pointer; i++){
		if(buffer[i].isValid()){
			arr[3*i+0] = buffer[i].getA();
			arr[3*i+1] = buffer[i].getB();
			arr[3*i+2] = buffer[i].getC();
			toSave = true;
		}
		else
			break;
	}
	if(toSave){
		this->ws.write((char*)arr, RECORD_SIZE*this->buffer_pointer);
		this->ws.flush();
		this->head = buffer[this->buffer_pointer-1];
		this->buffer_pointer = 0;
		this->buffer_loaded = false;
		this->diskWrites++;
	}
}

//========================DONT USE THESE========================//
// Old function not used by this program anymore
Record Tape::readRecordInternal(){
	double arr[3];
	this->rs.read((char*) arr, RECORD_SIZE);
	this->head = Record(arr[0], arr[1], arr[2]);
	this->currPosition = this->rs.tellg();
	if(this->rs.fail() || this->rs.eof()){
		this->endReached = true;
		this->rs.clear();
		this->head = Record();
	}
	return this->head;
}

// Old function not used by this program anymore
void Tape::writeRecordInternal(Record record){
	double arr[3] = {record.getA(), record.getB(), record.getC()};
	this->ws.write((char*)arr, RECORD_SIZE);
	this->head = record;
	this->ws.flush();
}
//========================DONT USE THESE=========================//

double Tape::printBufferReadMode(){
	double prev_key;
	bool first = true;
	for(int i=this->buffer_pointer-1; i<BUFFER_SIZE; i++){
		Record rec = this->buffer[i];
		if(!rec.isValid())
			break;
		if(!first && prev_key > rec.getKey())
			std::cout << "|====================================================================|\n";
		this->printSingleRecord(rec);
		prev_key = rec.getKey();
		first = false;
	}
	return prev_key;
}

void Tape::printBufferWriteMode(bool first, double prev_key){
	for(int i=0; i<this->buffer_pointer; i++){
		std::stringstream ss;
		Record rec = this->buffer[i];
		if(!first && prev_key > rec.getKey())
			std::cout << "|====================================================================|\n";
		this->printSingleRecord(rec);
		prev_key = rec.getKey();
		first = false;
	}
}

void Tape::printSingleRecord(Record rec){
		std::cout << "\tRecord: ";
		std::stringstream ss;
		ss << rec.getA() << " " << rec.getB() << " " << rec.getC();
		std::cout << std::setw(30);
		std::cout << ss.str();
		ss.str(std::string(""));
		ss <<" -> [" << rec.getKey()  << "]\n";
		std::cout << ss.str();
}

void Tape::gotoLine(unsigned int pos){
	this->rs.seekg(pos);
}
