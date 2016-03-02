#ifndef POINT_H_
#define POINT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/point.h
// Point Primitive Object derived from matrix

#include "matrix.h"
#include "vector.h"

class point;
class vector;

#ifdef FLOAT_PRECISION
	typedef float point_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double point_precision;
#endif

// Difference between two points will be a vector
vector operator-(const point &lhs, const point &rhs);

class point : public matrix <point_precision, 4, 1> {
public:
	point() {
		clear();
	}

	point(point_precision x, point_precision y, point_precision z) {
		this->clear();
		this->element(0, 0) = x;
		this->element(1, 0) = y;
		this->element(2, 0) = z;
		this->element(3, 0) = 1.0f;
	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline point_precision &x() { return this->element(0, 0); }
	inline point_precision &y() { return this->element(1, 0); }
	inline point_precision &z() { return this->element(2, 0); }
	inline point_precision &w() { return this->element(3, 0); }

	inline point_precision &x(point_precision val) { return this->element(0, 0) = val; }
	inline point_precision &y(point_precision val) { return this->element(1, 0) = val; }
	inline point_precision &z(point_precision val) { return this->element(2, 0) = val; }
	inline point_precision &w(point_precision val) { return this->element(3, 0) = val; }

	RESULT translate(point_precision x, point_precision y, point_precision z) {
		this->x() += x;
		this->y() += y;
		this->z() += z;

		return R_PASS;
	}

	// This should also work with vector
	RESULT translate(matrix <point_precision, 4, 1> v) {
		(*this).operator+=((matrix <point_precision, 4, 1>&)v);
		return R_PASS;
	}

	/*
	// Subtracting points results in vector
	matrix& operator-=(const matrix& rhs) {
		subData(rhs.m_data);
		return *this;
	}
	*/


	/*
	vector<TMatrix, N, M> operator-(const matrix<TMatrix, N, M>&arg) const {
		return matrix<TMatrix, N, M>(*this).operator-=(arg);
	}
	*/

	// Explicitly specializing the assignment operator
	point& operator=(const matrix<point_precision, 4, 1> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(point_precision) * 4 * 1);

		return *this;
	}
};

#endif // !POINT_H_
