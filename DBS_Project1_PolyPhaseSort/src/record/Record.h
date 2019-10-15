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

	double sumOfRoots(){ return -b/a;}

	double getA(){ return a;}
	double getB(){ return b;}
	double getC(){ return c;}

	double getKey(){ return -b/a;}

	bool isValid() {return correct;}

private:
	double a;
	double b;
	double c;
	bool correct;
};

#endif /* RECORD_RECORD_H_ */
