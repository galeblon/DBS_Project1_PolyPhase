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
	Record(double a, double b, double c);
	virtual ~Record();

	double SumOfRoots(){ return -b/a;}

	double GetA(){ return a;}
	double GetB(){ return b;}
	double GetC(){ return c;}

private:
	double a;
	double b;
	double c;
};

#endif /* RECORD_RECORD_H_ */
