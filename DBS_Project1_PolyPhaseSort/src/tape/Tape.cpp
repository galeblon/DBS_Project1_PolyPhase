/*
 * tape.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#include <sstream>
#include "Tape.h"

Tape::Tape(){
	//TODO: set correct mode whether you want to read or write tape
	this->fs.open("Tape_default", std::fstream::out);
}

Tape::Tape(std::string id) {
	std::string tapeName = std::string("Tape_") + id;
	this->fs.open(tapeName.c_str(), std::fstream::out | std::fstream::in);
}

Tape::~Tape() {
	this->fs.close();
}

Record Tape::ReadRecord(){
	double a, b, c;

	std::string line;
	std::getline(this->fs, line);
	std::stringstream ss(line);
	ss >> a >> b >> c;
	return Record(a, b, c);
}

void Tape::WriteRecord(Record record){
	this->fs << record.GetA() << " " << record.GetB() << " " << record.GetC() << '\n';
	this->fs.flush();
}
