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

// Abstract class linking all possible datasources
class DataSource {
public:
	virtual ~DataSource() {}
	virtual void InitialDistribution(Tape* Tapes[], int numOfTapes, std::string arg) = 0;
};

#endif /* SRC_DATASOURCE_DATASOURCE_H_ */
