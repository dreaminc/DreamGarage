#ifndef POINT_H_
#define POINT_H_

#include "RESULT/EHM.h"

#include <string>

// DREAM OS
// DreamOS/Dimension/Primitives/point.h
// Point Primitive Object derived from matrix

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

// TODO: This has been removed and a point2D object has been created
// Next step is to move to a point interface that allows seamless creation/conversion
// between point2,3,4 etc using templates and typed object definitions similar to how 
// OGL Uniforms are done
//typedef matrix<float, 2, 1> point2D;

class point : public matrix <point_precision, 4, 1> {
public:
	enum component {
		X,
		Y,
		Z,
		W,
		INVALID
	};

public:
	point();
	point(point_precision val);
	point(point_precision x, point_precision y, point_precision z);
	point(point_precision values[3]);
	point(point_precision x, point_precision y, point_precision z, point_precision w);
	point(const matrix<point_precision, 4, 1>& arg);

	RESULT SetZeroW();
	RESULT Reverse();
	bool IsZero();

	// TODO: Understand performance implications of this although both element and this are inline
	inline point_precision &x() { return this->element(0, 0); }
	inline point_precision &y() { return this->element(1, 0); }
	inline point_precision &z() { return this->element(2, 0); }
	inline point_precision &w() { return this->element(3, 0); }

	inline point_precision &x(point_precision val) { return this->element(0, 0) = val; }
	inline point_precision &y(point_precision val) { return this->element(1, 0) = val; }
	inline point_precision &z(point_precision val) { return this->element(2, 0) = val; }
	inline point_precision &w(point_precision val) { return this->element(3, 0) = val; }

	RESULT translate(point_precision x, point_precision y, point_precision z);
	RESULT translate(vector v);
	RESULT translateX(point_precision x);
	RESULT translateY(point_precision y);
	RESULT translateZ(point_precision z);

	//RESULT Print(char *pszOptName = nullptr, bool fReturn = false);
	RESULT Print(const char* const &pszOptName = nullptr, bool fReturn = false);
	std::string toString(bool fW = false);

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
	point operator+(const point& rhs) const;
	point& operator+=(const point& rhs);

	point operator+(const vector& rhs) const;
	point& operator+=(const vector& rhs);

	vector operator-(const point& rhs) const;

	// Subtracting a vector from a point gives us a point
	point operator-(const vector& rhs) const;
	point& operator-=(const vector& rhs);

	// Explicitly specializing the assignment operator
	point& operator=(const matrix<point_precision, 4, 1> &arg);

	bool operator>(point &rhs);
	bool operator>=(point &rhs);
	bool operator<(point &rhs);
	bool operator<=(point &rhs);

	static point max(point &lhs, point &rhs);
	static point min(point &lhs, point &rhs);
	static point midpoint(point &lhs, point &rhs);

	static point maxpoint() { return point(std::numeric_limits<point_precision>::max()); }
	static point minpoint() { return point(std::numeric_limits<point_precision>::min()); }

	/*
	point& operator+=(const matrix<point_precision, 4, 1>& arg) {
		return (point)(matrix<point_precision, 4, 1>::operator+=(arg));
	}
	*/
};

#endif // !POINT_H_
