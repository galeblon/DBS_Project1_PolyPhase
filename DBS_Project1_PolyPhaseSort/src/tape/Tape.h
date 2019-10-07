/*
 * tape.h
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#ifndef TAPE_TAPE_H_
#define TAPE_TAPE_H_

#include<fstream>
#include "../record/Record.h"

class Tape {
public:
	Tape();
	Tape(std::string id);
	virtual ~Tape();

	Record ReadRecord();
	void WriteRecord(Record record);
private:
	std::fstream fs;
};

#endif /* TAPE_TAPE_H_ */
