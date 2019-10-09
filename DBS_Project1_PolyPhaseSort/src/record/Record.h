/*
 * Record.h
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#ifndef RECORD_RECORD_H_
#define RECORD_RECORD_H_

class Record {
public:
	Record();
	Record(double a, double b, double c);
	virtual ~Record();

	double SumOfRoots(){ return -b/a;}

	double GetA(){ return a;}
	double GetB(){ return b;}
	double GetC(){ return c;}

	double GetKey(){ return -b/a;}

	bool isValid() {return correct;}

private:
	double a;
	double b;
	double c;
	bool correct;
};

#endif /* RECORD_RECORD_H_ */
