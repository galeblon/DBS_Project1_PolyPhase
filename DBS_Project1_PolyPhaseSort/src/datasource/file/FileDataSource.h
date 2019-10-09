/*
 * FileDataSource.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gales
 */

#ifndef SRC_DATASOURCE_FILE_FILEDATASOURCE_H_
#define SRC_DATASOURCE_FILE_FILEDATASOURCE_H_

#include "../../tape/Tape.h"
#include <fstream>

class FileDataSource {
public:
	/*
	 * Reads file once and distributes runs into N-1 tapes
	 * */
	static void InitialDistribution(Tape* tapes[], int numOfTapes, std::fstream& fs);
private:
	static Record loadFromFile(std::fstream& fs);
	static Record getRunFromFile(std::fstream& fs, Tape& tape, Record rec_cont);

};

#endif /* SRC_DATASOURCE_FILE_FILEDATASOURCE_H_ */
