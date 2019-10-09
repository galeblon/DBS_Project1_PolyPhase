/*
 * tape.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#include "Tape.h"

Tape::Tape(): head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false){
	this->name = "Tape_default";
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = false;
}

Tape::Tape(std::string id):  head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = false;
}

Tape::Tape(std::string id, bool debug): head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false){
	this->name = std::string("Tape_") + id;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = debug;
}

Tape::Tape(const Tape& other): head(), runs(0), runsDummy(0), endReached(0), currPosition(0), readMode(false){
	this->name = other.name;
	this->ws.open(this->name.c_str(), std::ios::out);
	this->rs.open(this->name.c_str(), std::ios::in);
	this->debug = other.debug;
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
	this->head = record;
	this->ws.flush();
}


void Tape::GotoLine(unsigned int pos){
	this->rs.seekg(pos);
}


void Tape::printContents(){
	double prev_key;
	bool first = true;
	std::string line;
	std::cout << this->name << ' ' << this->runs+this->runsDummy << '(' << this->runsDummy <<") runs | contents:\n";
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
		}
		prev_key = -b/a;
		std::cout << '\n';
		first = false;
	}
	this->rs.clear();
	this->GotoLine(this->currPosition);
}
