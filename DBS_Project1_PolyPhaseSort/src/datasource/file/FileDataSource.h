/*
 * FileDataSource.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#ifndef SRC_DATASOURCE_FILE_FILEDATASOURCE_H_
#define SRC_DATASOURCE_FILE_FILEDATASOURCE_H_

#include "../../tape/Tape.h"
#include "../DataSource.h"
#include <fstream>

class FileDataSource: public DataSource {
public:
	/*
	 * Reads file once and distributes runs into N-1 tapes
	 * */
	~FileDataSource() {}
	void InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg);
private:
	Record loadFromFile(std::fstream& fs);
	Record getRunFromFile(std::fstream& fs, Tape& tape, Record rec_cont);

};

#endif /* SRC_DATASOURCE_FILE_FILEDATASOURCE_H_ */
