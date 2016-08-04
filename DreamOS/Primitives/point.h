#ifndef POINT_H_
#define POINT_H_

#include "RESULT/EHM.h"

#include <string>

// DREAM OS
// DreamOS/Dimension/Primitives/point.h
// Point Primitive Object derived from matrix

#include "matrix.h"
//#include "vector.h"

class point;
class vector;

#ifdef FLOAT_PRECISION
	typedef float point_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double point_precision;
#endif

// Difference between two points will be a vector
//vector operator-(const point &lhs, const point &rhs);

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

	point(point_precision values[3]) {
		this->clear();
		this->element(0, 0) = values[0];
		this->element(1, 0) = values[1];
		this->element(2, 0) = values[2];
		this->element(3, 0) = 1.0f;
	}

	point(point_precision x, point_precision y, point_precision z, point_precision w) {
		this->clear();

		this->element(0, 0) = x;
		this->element(1, 0) = y;
		this->element(2, 0) = z;
		this->element(3, 0) = w;
	}

	point(const matrix<point_precision, 4, 1>& arg) :
		matrix<point_precision, 4, 1>(arg)
	{
		// empty
	}

	RESULT SetZeroW() {
		this->element(3, 0) = 1.0f;
		return R_PASS;
	}

	RESULT Reverse() {
		
		x() = -x();
		y() = -y(); 
		z() = -z();

		return R_PASS;
	}

	bool IsZero() {
		if ((x() != 0) || (y() != 0) || (z() != 0))
			return false;
		return true;
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

	RESULT translateX(point_precision x) {
		this->x() += x;
		return R_PASS;
	}

	RESULT translateY(point_precision y) {
		this->y() += y;
		return R_PASS;
	}

	RESULT translateZ(point_precision z) {
		this->z() += z;
		return R_PASS;
	}

	// This should also work with vector
	RESULT translate(matrix <point_precision, 4, 1> v) {
		(*this).operator+=((matrix <point_precision, 4, 1>&)v);
		return R_PASS;
	}

	std::string toString(bool fW = false) {
		std::string strRet = "(";

		strRet += std::to_string(x()); 
		strRet += ",";
		strRet += std::to_string(y());
		strRet += ",";
		strRet += std::to_string(z());

		if (fW) {
			strRet += ",";
			strRet += std::to_string(w());
		}

		strRet += ")";

		return strRet;
	}

	// Subtracting points results in vector
	/*
	point& operator-=(const matrix<point_precision, 4, 1> &arg) {
		subData((point_precision*)(arg.m_data));
		return (*this);
	}

	vector operator-(const matrix<point_precision, 4, 1>&arg) const {
		matrix<point_precision, 4, 1> retMat = matrix<point_precision, 4, 1>(*this).operator-=(arg);
		vector *pv = reinterpret_cast<vector*>(&retMat);
		return (*pv);
	}
	*/

	friend vector operator-(point &lhs, point &rhs);

	// Explicitly specializing the assignment operator
	point& operator=(const matrix<point_precision, 4, 1> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(point_precision) * 4 * 1);

		return *this;
	}

	/*
	point& operator+=(const matrix<point_precision, 4, 1>& arg) {
		return (point)(matrix<point_precision, 4, 1>::operator+=(arg));
	}
	*/
};

#endif // !POINT_H_
