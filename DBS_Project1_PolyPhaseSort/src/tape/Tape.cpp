/*
 * tape.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#include "Tape.h"

Tape::Tape(){
	this->name = "Tape_default";
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = false;
	this->readMode = false;
	this->endReached = false;
	this->currPosition = 0;
}

Tape::Tape(std::string id) {
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = false;
	this->readMode = false;
	this->endReached = false;
	this->currPosition = 0;
}

Tape::Tape(std::string id, bool debug){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = debug;
	this->readMode = false;
	this->endReached = false;
	this->currPosition = 0;
}

Tape::Tape(const Tape& other){
	this->name = other.name;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = other.debug;
	this->readMode = other.readMode;
	this->endReached = false;
	this->currPosition = 0;
}

Tape::~Tape() {
	this->ws.close();
	this->rs.close();
}

Record Tape::ReadRecord(){
	double a, b, c;
	std::string line;
	std::getline(this->rs, line);
	this->currPosition = this->rs.tellg();
	if(line == ""){
		this->endReached = true;
		this->rs.clear();
	}
	std::stringstream ss(line);
	ss >> a >> b >> c;
	return Record(a, b, c);
}

void Tape::WriteRecord(Record record){
	this->ws << record.GetA() << " " << record.GetB() << " " << record.GetC() << '\n';
	this->ws.flush();
}


void Tape::GotoLine(unsigned int pos){
	this->rs.seekg(pos);
}


void Tape::printContents(){
	std::string line;
	std::cout << "\n|========================|\n" << this->name << " contents:\n";
	while(!this->rs.eof()){
		double a, b, c;
		std::string line;
		std::getline(this->rs, line);
		std::stringstream ss(line);
		ss >> a >> b >> c;
		std::cout << this->rs.eof() << rs.fail() << rs.bad() << rs.good();
		if(line != ""){
			std::cout << "\tRecord: ";
			std::cout.width(40);
			std::cout << line;
			std::cout.width(0);
			std::cout << " -> [" << -b/a  << "]";
		}
		std::cout << '\n';
	}
	this->rs.clear();
	this->GotoLine(this->currPosition);
}
