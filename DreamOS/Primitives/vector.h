#ifndef VECTOR_H_
#define VECTOR_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/vector.h
// Vector Primitive Object derived from matrix

#include "matrix.h"
#include "point.h"

class vector : public matrix <double, 4, 0> {
public:
	vector() {
		clear();
	}

	vector(double x, double y, double z) {
		this->clear();
		this->element(0, 0) = x;
		this->element(1, 0) = y;
		this->element(2, 0) = z;
		this->element(3, 0) = 1.0f;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline double &x() { return this->element(0, 0); }
	inline double &y() { return this->element(1, 0); }
	inline double &z() { return this->element(2, 0); }
	inline double &w() { return this->element(3, 0); }

	double magnitude() {
		double sqaureSum = pow(x(), 2) + pow(y(), 2) + pow(z(), 2);
		return sqrt(sqaureSum);
	}
};

#endif // !VECTOR_H_