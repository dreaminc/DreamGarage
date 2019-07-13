#ifndef POINT_2D_H_
#define POINT_2D_H_

#include "RESULT/EHM.h"

#include <string>

// DREAM OS
// DreamOS/Dimension/Primitives/point2D.h
// 2D Point Primitive Object derived from matrix
// based largely on point - but should be converted to interface/template
// solution in the future

#include "matrix/matrix.h"
#include <limits>

class vector;

#ifdef FLOAT_PRECISION
	typedef float point_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double point_precision;
#endif

// Difference between two points will be a vector
//vector operator-(const point &lhs, const point &rhs);

class point2D : public matrix <point_precision, 2, 1> {
public:
	enum component {
		X,
		Y,
		INVALID
	};

public:
	point2D();
	point2D(point_precision val);
	point2D(point_precision x, point_precision y);
	point2D(point_precision values[2]);
	point2D(const matrix<point_precision, 2, 1>& arg);

	RESULT Reverse();
	bool IsZero();

	// TODO: Understand performance implications of this although both element and this are inline
	inline point_precision &x() { return this->element(0, 0); }
	inline point_precision &y() { return this->element(1, 0); }

	inline point_precision &x(point_precision val) { return this->element(0, 0) = val; }
	inline point_precision &y(point_precision val) { return this->element(1, 0) = val; }

	RESULT translate(point_precision x, point_precision y);
	RESULT translate(vector v);
	RESULT translateX(point_precision x);
	RESULT translateY(point_precision y);

	RESULT Print(const char* const &pszOptName = nullptr, bool fReturn = false);
	std::string toString();

	inline point_precision &GetComponent(component c);
	RESULT swap(component a, component b);

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

	
	// Adding a vector to a point gives us a point
	//friend point operator+(point &lhs, vector &rhs);
	point2D operator+(const point2D& rhs) const;
	point2D& operator+=(const point2D& rhs);

	point2D operator+(const vector& rhs) const;
	point2D& operator+=(const vector& rhs);

	// For now subtracting two point2Ds results in a point
	// since we don't have a vector2D - or a vector from point2D constructor
	//vector operator-(const point2D& rhs) const;
	point2D operator-(const point2D& rhs) const;

	// Subtracting a vector from a point gives us a point
	point2D operator-(const vector& rhs) const;
	point2D& operator-=(const vector& rhs);

	// Explicitly specializing the assignment operator
	point2D& operator=(const matrix<point_precision, 2, 1> &arg);

	bool operator>(point2D &rhs);
	bool operator>=(point2D &rhs);
	bool operator<(point2D &rhs);
	bool operator<=(point2D &rhs);

	static point2D max(point2D &lhs, point2D &rhs);
	static point2D min(point2D &lhs, point2D &rhs);
	static point2D midpoint(point2D &lhs, point2D &rhs);

	static point2D maxpoint() { return point2D(std::numeric_limits<point_precision>::max()); }
	static point2D minpoint() { return point2D(std::numeric_limits<point_precision>::min()); }

	/*
	point& operator+=(const matrix<point_precision, 4, 1>& arg) {
		return (point)(matrix<point_precision, 4, 1>::operator+=(arg));
	}
	*/
};

#endif // !POINT_2D_H_
