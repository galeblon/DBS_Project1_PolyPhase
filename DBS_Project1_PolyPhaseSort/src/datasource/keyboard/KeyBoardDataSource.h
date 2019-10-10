/*
 * KeyBoardDataSource.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#ifndef SRC_DATASOURCE_KEYBOARD_KEYBOARDDATASOURCE_H_
#define SRC_DATASOURCE_KEYBOARD_KEYBOARDDATASOURCE_H_

#include "../../tape/Tape.h"
#include "../DataSource.h"
#include <iostream>

class KeyBoardDataSource: public DataSource {
public:
	KeyBoardDataSource(): recordsToGenerate(0) {}
	~KeyBoardDataSource() {}
	void InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg);
private:
	Record askForRecord();
	Record getRunFromKeyBoard(Tape& tape, Record rec_cont);
	unsigned int recordsToGenerate;
};

#endif /* SRC_DATASOURCE_KEYBOARD_KEYBOARDDATASOURCE_H_ */
