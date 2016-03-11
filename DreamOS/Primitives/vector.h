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

	// Calculate the cross product
	// Not assumed to be normalized
	vector(vector rhs, vector lhs) {
		clear();

		x(rhs(1) * lhs(2)) - (rhs(2) * lhs(1));
		y(rhs(2) * lhs(0)) - (rhs(0) * lhs(2));
		z(rhs(0) * lhs(1)) - (rhs(1) * lhs(0));
		
		// For good measure
		w(1);
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
		vector result = *this;
		result.Normalize();
		return result;
	}

	// Dot Product
	// This calculates the dot product as if it is a R3 vector (ignores the w parameter) 
	vector_precision dot(vector& rhs) {
		vector_precision result = 0;

		for (int i = 0; i < 3; i++)
			result += element(i, 0) * rhs.element(i, 0);

		return result;
	}

	// Cross Product
	vector cross(vector rhs) {
		return vector(*this, rhs);
	}

	// Normalized Cross Product
	vector NormalizedCross(vector rhs) {
		return vector(this->Normal(), rhs.Normal());
	}

	// Explicitly specializing the assignment operator
	vector& operator=(const matrix<vector_precision, 4, 1> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(vector_precision) * 4 * 1);

		return *this;
	}

	// Utility
public:
	static vector XUnitVector() { return vector(1.0f, 0.0f, 0.0f); }
	static vector YUnitVector() { return vector(0.0f, 1.0f, 0.0f); }
	static vector ZUnitVector() { return vector(0.0f, 0.0f, 1.0f); }

	static vector iVector() { return vector(1.0f, 0.0f, 0.0f); }
	static vector jVector() { return vector(0.0f, 1.0f, 0.0f); }
	static vector kVector() { return vector(0.0f, 0.0f, 1.0f); }
};

#endif // !VECTOR_H_