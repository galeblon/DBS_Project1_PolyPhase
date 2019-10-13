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
			  diskWrites(0){
	this->name = "Tape_default";
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
	this->debug = false;
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
							 diskWrites(0){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
	this->debug = false;
}

Tape::Tape(std::string id, bool debug): head(),
										runs(0),
										runsDummy(0),
										endReached(0),
										currPosition(0),
										readMode(false),
										buffer_pointer(0),
										buffer_loaded(false),
										diskReads(0),
										diskWrites(0){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
	this->debug = debug;
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
							   diskWrites(0){
	this->name = other.name;
	this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
	this->rs.open(this->name.c_str(), std::ios::in | std::ios::binary);
	this->debug = other.debug;
}

Tape::~Tape() {
	this->ws.close();
	this->rs.close();
}

void Tape::LoadBufferInternal(){
	for(int i=0; i<BUFFER_SIZE; i++){
		buffer[i] = ReadRecordInternal();
		if(!buffer[i].isValid())
			break;
	}
	if(buffer[0].isValid()){
		this->buffer_pointer = 0;
		this->buffer_loaded = true;
		this->diskReads++;
	}
}

void Tape::SaveBufferInternal(){
	bool saved = false;
	for(int i=0; i<this->buffer_pointer; i++){
		if(buffer[i].isValid()){
			WriteRecordInternal(buffer[i]);
			saved = true;
		}
	}
	if(saved){
		this->buffer_pointer = 0;
		this->buffer_loaded = false;
		this->diskWrites++;
	}
}

Record Tape::ReadRecord(){
	if(!this->readMode){
		// Save buffer if necessary
		this->SaveBufferInternal();
		this->readMode = true;
		this->rs.seekg(0);
	}
	if(!this->buffer_loaded)
		LoadBufferInternal();
	// Grab from buffer
	Record rec = this->buffer[this->buffer_pointer++];
	if(this->buffer_pointer >= BUFFER_SIZE){
		this->buffer_loaded = false;
		this->buffer_pointer = 0;
	}
	this->head = rec;
	return rec;
}

Record Tape::ReadRecordInternal(){
	double arr[3];
	this->rs.read((char*) arr, 3*sizeof(double));
	this->head = Record(arr[0], arr[1], arr[2]);
	this->currPosition = this->rs.tellg();
	if(this->rs.fail() || this->rs.eof()){
		this->endReached = true;
		this->rs.clear();
		this->head = Record();
	}
	return this->head;
}

void Tape::WriteRecord(Record record){
	if(this->readMode){
		this->readMode = false;
		this->ws.close();
		this->ws.open(this->name.c_str(), std::ios::out | std::ios::binary);
		this->buffer_loaded = false;
		this->buffer_pointer =0;
	}
	if(this->buffer_pointer >= BUFFER_SIZE)
		this->SaveBufferInternal();
	this->buffer[this->buffer_pointer++] = record;
	this->head = record;
}

void Tape::WriteRecordInternal(Record record){
	double arr[3] = {record.GetA(), record.GetB(), record.GetC()};
	this->ws.write((char*)arr, 3*sizeof(double));
	this->head = record;
	this->ws.flush();
}


void Tape::GotoLine(unsigned int pos){
	this->rs.seekg(pos);
}

double Tape::printBufferReadMode(){
	double prev_key;
	bool first = true;
	for(int i=this->buffer_pointer; i<BUFFER_SIZE; i++){
		Record rec = this->buffer[i];
		if(!rec.isValid())
			break;
		if(!first && prev_key > rec.GetKey())
			std::cout << "|====================================================================|\n";
		this->printSingleRecord(rec);
		prev_key = rec.GetKey();
	}
	return prev_key;
}

void Tape::printBufferWriteMode(bool first, double prev_key){
	for(int i=0; i<this->buffer_pointer; i++){
		std::stringstream ss;
		Record rec = this->buffer[i];
		if(!first && prev_key > rec.GetKey())
			std::cout << "|====================================================================|\n";
		this->printSingleRecord(rec);
		prev_key = rec.GetKey();
	}
}

void Tape::printSingleRecord(Record rec){
		std::cout << "\tRecord: ";
		std::stringstream ss;
		ss << rec.GetA() << " " << rec.GetB() << " " << rec.GetC();
		std::cout << std::setw(30);
		std::cout << ss.str();
		ss.str(std::string(""));
		ss <<" -> [" << rec.GetKey()  << "]\n";
		std::cout << ss.str();
}

void Tape::printContents(){
	double prev_key;
	bool first = true;
	std::string line;
	if(this->readMode && !this->endReached){
		prev_key = this->printBufferReadMode();
		if(this->buffer_pointer < BUFFER_SIZE)
			first = false;
	}
	if(!readMode)
		this->rs.seekg(0);
	while(!this->rs.eof()){
		double arr[3];
		this->rs.read((char*) arr, 3*sizeof(double));
		Record rec = Record(arr[0], arr[1], arr[2]);
		//std::cout << this->rs.eof() << rs.fail() << rs.bad() << rs.good();
		if(!this->rs.fail()){
			if(!first && prev_key > rec.GetKey())
				std::cout << "|====================================================================|\n";
			this->printSingleRecord(rec);
		} else
			continue;
		prev_key = rec.GetKey();
		first = false;
	}
	if(!this->readMode)
		this->printBufferWriteMode(first, prev_key);
	this->rs.clear();
	this->GotoLine(this->currPosition);
}
