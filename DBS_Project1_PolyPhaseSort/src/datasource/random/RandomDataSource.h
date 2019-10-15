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
#include "../../VerbosityLevels.h"
#include <sstream>
#include <cstdlib>

// The coefficients value will be random between -COEFFICIENT_MAX and COEFFICIENT_MAX
#define COEFFICIENT_MAX 100

class RandomDataSource: public DataSource {
public:
	RandomDataSource(): recordsToGenerate(0), verbosityLevel(0) {}
	~RandomDataSource() {}
	void InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg, int verbosity_level);
private:
	Record generateRecord();
	Record getRunFromRandom(Tape& tape, Record rec_cont);
	unsigned int recordsToGenerate;
	int verbosityLevel;
};

#endif /* SRC_DATASOURCE_RANDOM_RANDOMDATASOURCE_H_ */
