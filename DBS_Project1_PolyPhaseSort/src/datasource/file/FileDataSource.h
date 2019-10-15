/*
 * FileDataSource.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#ifndef SRC_DATASOURCE_FILE_FILEDATASOURCE_H_
#define SRC_DATASOURCE_FILE_FILEDATASOURCE_H_

#include "../DataSource.h"
#include "../../tape/Tape.h"
#include "../../VerbosityLevels.h"
#include <fstream>

class FileDataSource: public DataSource {
public:
	/*
	 * Reads file once and distributes runs into N-1 tapes
	 * */
	FileDataSource(): verbosityLevel(0) {}
	~FileDataSource() {}
	void InitialDistribution(Tape* tapes[], int numOfTapes, std::string arg, int verbosity_level);
private:
	Record getRunFromFile(std::fstream& fs, Tape& tape, Record rec_cont);
	Record loadFromFile(std::fstream& fs);
	int verbosityLevel;
};

#endif /* SRC_DATASOURCE_FILE_FILEDATASOURCE_H_ */
