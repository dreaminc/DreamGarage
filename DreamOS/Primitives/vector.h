#ifndef VECTOR_H_
#define VECTOR_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/vector.h
// Vector Primitive Object derived from matrix

#include "matrix/matrix.h"

#ifdef FLOAT_PRECISION
	typedef float vector_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double vector_precision;
#endif

class point;
class quaternion;

class vector : public matrix <vector_precision, 4, 1> {
public:
	vector();
	vector(matrix <vector_precision, 4, 1> &rhs);
	vector(vector_precision x, vector_precision y, vector_precision z);
	vector(vector rhs, vector lhs);	// Cross product - Not assumed to be normalized
	vector(const point& pt);

	// TODO: Understand performance implications of this although both element and this are inline
	inline vector_precision &x() { return this->element(0, 0); }
	inline vector_precision &y() { return this->element(1, 0); }
	inline vector_precision &z() { return this->element(2, 0); }
	inline vector_precision &w() { return this->element(3, 0); }

	inline vector_precision &x(vector_precision val) { return this->element(0, 0) = val; }
	inline vector_precision &y(vector_precision val) { return this->element(1, 0) = val; }
	inline vector_precision &z(vector_precision val) { return this->element(2, 0) = val; }
	inline vector_precision &w(vector_precision val) { return this->element(3, 0) = val; }

	double magnitude();
	
	RESULT Normalize();		// Will normalize this vector
	vector Normal();		// Return a normalized version of this vector

	RESULT Print(char *pszOptName = nullptr);

	vector_precision dot(vector& rhs);
	vector_precision dot(point& rhs);

	vector cross(vector rhs);
	vector NormalizedCross(vector rhs);

	vector RotateByQuaternion(quaternion q);

	// Explicitly specializing the assignment operator
	vector& operator=(const matrix<vector_precision, 4, 1> &arg);

	vector& operator*=(const vector_precision& a);
	vector operator*(const vector_precision& a) const;

	// Utility
public:
	static vector XUnitVector() { return vector(1.0f, 0.0f, 0.0f); }
	static vector YUnitVector() { return vector(0.0f, 1.0f, 0.0f); }
	static vector ZUnitVector() { return vector(0.0f, 0.0f, 1.0f); }

	static vector iVector(vector_precision value = 1.0f) { return vector(value, 0.0f, 0.0f); }
	static vector jVector(vector_precision value = 1.0f) { return vector(0.0f, value, 0.0f); }
	static vector kVector(vector_precision value = 1.0f) { return vector(0.0f, 0.0f, value); }
};

#endif // !VECTOR_H_