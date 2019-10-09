/*
 * Record.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: gales
 */

#include "Record.h"

Record::Record() : a(0), b(0), c(0), correct(false){}
Record::Record(double a, double b, double c) : a(a), b(b), c(c), correct(true) {}

Record::~Record() {}

