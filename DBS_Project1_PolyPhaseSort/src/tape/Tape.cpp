/*
 * tape.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#include "Tape.h"

Tape::Tape(): head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false), buffer_pointer(0), buffer_loaded(false){
	this->name = "Tape_default";
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = false;
}

Tape::Tape(std::string id):  head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false), buffer_pointer(0), buffer_loaded(false){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = false;
}

Tape::Tape(std::string id, bool debug): head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false), buffer_pointer(0), buffer_loaded(false){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = debug;
}

Tape::Tape(const Tape& other): head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false), buffer_pointer(0), buffer_loaded(false){
	this->name = other.name;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = other.debug;
}

Tape::~Tape() {
	this->ws.close();
	this->rs.close();
}

void Tape::LoadBufforInternal(){
	for(int i=0; i<BUFFOR_SIZE; i++){
		buffor[i] = ReadRecordInternal();
		if(!buffor[i].isValid())
			break;
	}
	if(buffor[0].isValid()){
		this->buffer_pointer = 0;
		this->buffer_loaded = true;
		// Increment disk reads
	}
}

void Tape::SaveBufforInternal(){
	bool saved = false;
	for(int i=0; i<this->buffer_pointer; i++){
		if(buffor[i].isValid()){
			WriteRecordInternal(buffor[i]);
			saved = true;
		}
	}
	if(saved){
		// Increment disk writes
		this->buffer_pointer = 0;
		this->buffer_loaded = false;
	}
}

Record Tape::ReadRecord(){
	if(!this->readMode){
		//save buffor if necessary
		this->SaveBufforInternal();
		this->readMode = true;
		this->rs.seekg(0);
	}
	if(!this->buffer_loaded)
		LoadBufforInternal();
	//Grab from buffer
	Record rec = this->buffor[this->buffer_pointer++];
	if(this->buffer_pointer >= BUFFOR_SIZE){
		this->buffer_loaded = false;
		this->buffer_pointer = 0;
	}
	this->head = rec;
	return rec;
}

Record Tape::ReadRecordInternal(){
	double a, b, c;
	std::string line;
	std::getline(this->rs, line);
	this->currPosition = this->rs.tellg();
	if(line == ""){
		this->endReached = true;
		this->rs.clear();
		this->head = Record();
	} else{
		std::stringstream ss(line);
		ss >> a >> b >> c;
		this->head = Record(a, b, c);
	}
	return this->head;
}

void Tape::WriteRecord(Record record){
	if(this->readMode){
		this->readMode = false;
		this->ws.close();
		this->ws.open(this->name.c_str(), std::ios::out);
		this->buffer_loaded = false;
		this->buffer_pointer =0;
	}
	// if buffer is full
	if(this->buffer_pointer >= BUFFOR_SIZE)
		this->SaveBufforInternal();
	this->buffor[this->buffer_pointer++] = record;
	this->head = record;
}

void Tape::WriteRecordInternal(Record record){
	this->ws << record.GetA() << " " << record.GetB() << " " << record.GetC() << '\n';
	this->head = record;
	this->ws.flush();
}


void Tape::GotoLine(unsigned int pos){
	this->rs.seekg(pos);
}

double Tape::printBufferReadMode(){
	double prev_key;
	bool first = true;

	for(int i=this->buffer_pointer; i<BUFFOR_SIZE; i++){
		Record rec = this->buffor[i];
		if(!rec.isValid())
			break;
		std::stringstream ss;
		ss << rec.GetA() << " " << rec.GetB() << " " << rec.GetC();
		if(!first && prev_key > rec.GetKey())
			std::cout << "|====================================================================|\n";
		std::cout << "\tRecord: ";
		std::cout.width(40);
		std::cout << ss.str();
		std::cout.width(0);
		std::cout << " -> [" << this->head.GetKey()  << "]\n";
	}

	return prev_key;
}

void Tape::printBufferWriteMode(bool first, double prev_key){
	for(int i=0; i<this->buffer_pointer; i++){
		std::stringstream ss;
		Record rec = this->buffor[i];
		ss << rec.GetA() << " " << rec.GetB() << " " << rec.GetC();
		if(!first && prev_key > rec.GetKey())
			std::cout << "|====================================================================|\n";
		std::cout << "\tRecord: ";
		std::cout.width(40);
		std::cout << ss.str();
		std::cout.width(0);
		std::cout << " -> [" << rec.GetKey()  << "]\n";
		prev_key = rec.GetKey();

	}
}

void Tape::printContents(){
	double prev_key;
	bool first = true;
	std::string line;
	if(this->readMode && !this->endReached){
		prev_key = this->printBufferReadMode();
		if(this->buffer_pointer < BUFFOR_SIZE)
			first = false;
	}
	if(!readMode)
		this->rs.seekg(0);
	while(!this->rs.eof()){
		double a, b, c;
		std::string line;
		std::getline(this->rs, line);
		std::stringstream ss(line);
		ss >> a >> b >> c;
		//std::cout << this->rs.eof() << rs.fail() << rs.bad() << rs.good();
		if(line != ""){
			if(!first && prev_key > -b/a)
				std::cout << "|====================================================================|\n";
			std::cout << "\tRecord: ";
			std::cout.width(40);
			std::cout << line;
			std::cout.width(0);
			std::cout << " -> [" << -b/a  << "]";
		} else
			continue;
		prev_key = -b/a;
		std::cout << '\n';
		first = false;
	}
	if(!this->readMode)
		this->printBufferWriteMode(first, prev_key);
	this->rs.clear();
	this->GotoLine(this->currPosition);
}
