#ifndef VECTOR_H_
#define VECTOR_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/vector.h
// Vector Primitive Object derived from matrix

#include "matrix.h"
#include "point.h"

#ifdef FLOAT_PRECISION
	typedef float vector_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double vector_precision;
#endif

class vector : public matrix <vector_precision, 4, 1> {
public:
	vector() {
		clear();
	}

	vector(vector_precision x, vector_precision y, vector_precision z) {
		this->clear();
		this->element(0, 0) = x;
		this->element(1, 0) = y;
		this->element(2, 0) = z;
		this->element(3, 0) = 1.0f;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline vector_precision &x() { return this->element(0, 0); }
	inline vector_precision &y() { return this->element(1, 0); }
	inline vector_precision &z() { return this->element(2, 0); }
	inline vector_precision &w() { return this->element(3, 0); }

	inline vector_precision &x(vector_precision val) { return this->element(0, 0) = val; }
	inline vector_precision &y(vector_precision val) { return this->element(1, 0) = val; }
	inline vector_precision &z(vector_precision val) { return this->element(2, 0) = val; }
	inline vector_precision &w(vector_precision val) { return this->element(3, 0) = val; }

	double magnitude() {
		double sqaureSum = pow(x(), 2) + pow(y(), 2) + pow(z(), 2);
		return sqrt(sqaureSum);
	}

	// Will normalize this vector
	RESULT Normalize() {
		vector_precision denom = 0;

		for (int i = 0; i < 4; i++)
			denom += pow(element(i, 0), 2);

		denom = sqrt(denom);

		for (int i = 0; i < 4; i++)
			element(i, 0) = element(i, 0) / denom;

		return R_PASS;
	}

	// Return a normalized version of this vector
	vector Normal() {
		vector result = this;
		result.Normalize();
		return result;
	}
};

#endif // !VECTOR_H_