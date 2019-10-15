/*
 * DataSource.h
 *
 *  Created on: Oct 10, 2019
 *      Author: gales
 */

#ifndef SRC_DATASOURCE_DATASOURCE_H_
#define SRC_DATASOURCE_DATASOURCE_H_

#include <string>
#include "../tape/Tape.h"

// Abstract class linking all possible data sources that can feed records into tapes
class DataSource {
public:
	virtual ~DataSource() {}
	virtual void InitialDistribution(Tape* Tapes[], int numOfTapes, std::string arg, int verbosity_level) = 0;
};

#endif /* SRC_DATASOURCE_DATASOURCE_H_ */
