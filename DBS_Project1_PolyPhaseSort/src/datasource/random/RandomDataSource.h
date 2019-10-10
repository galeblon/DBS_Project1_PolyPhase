/*
 * RandomDataSource.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#ifndef SRC_DATASOURCE_RANDOM_RANDOMDATASOURCE_H_
#define SRC_DATASOURCE_RANDOM_RANDOMDATASOURCE_H_

#include "../../tape/Tape.h"
#include "../DataSource.h"
#include <sstream>
#include <cstdlib>

class RandomDataSource: public DataSource {
public:
	RandomDataSource(): recordsToGenerate(0) {}
	~RandomDataSource() {}
	void InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg);
private:
	Record generateRecord();
	Record getRunFromRandom(Tape& tape, Record rec_cont);
	unsigned int recordsToGenerate;
};

#endif /* SRC_DATASOURCE_RANDOM_RANDOMDATASOURCE_H_ */
