#ifndef POINT_H_
#define POINT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/point.h
// Point Primitive Object derived from matrix

#include "matrix.h"
#include "vector.h"

class point : public matrix <double, 4, 0> {
public:
	point() {
		clear();
	}

	point(double x, double y, double z) {
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

	/*
	// Subtracting points results in vector
	matrix& operator-=(const matrix& rhs) {
		subData(rhs.m_data);
		return *this;
	}
	*/
	
	// Difference between two points will be a vector
	friend vector operator-(point lhs, const point& rhs) {
		return retVector(rhs.x() - lhs.x(), rhs.y() - lhs.y(), rhs.z() - lhs.z());
	}

	/*
	vector<TMatrix, N, M> operator-(const matrix<TMatrix, N, M>&arg) const {
		return matrix<TMatrix, N, M>(*this).operator-=(arg);
	}
	*/
};

/*
template <typename T = double, int D = 4>
class PointBase : public matrix <T, D, 0> {
public:
    PointBase() {
        memset(&m_data, 0, sizeof(T) * D);
    }
};

template <typename T = double>
class point4 : public point<T, 4> {
    point(T x, T y, T z, T w) {
        m_data[0] = x;
        m_data[0] = y;
        m_data[0] = z;
        m_data[0] = w;
    }
};

// Currently support specialized constructors for 4, 3 and 2 dimensional points
template <typename T = double> class point4 : public PointBase<T, 4> {
    point(T x, T y, T z, T w) {
        m_data[0] = x;
        m_data[0] = y;
        m_data[0] = z;
        m_data[0] = w;
    }
};

template <typename T = double> class point3 : public PointBase<T, 3> {
    point(T x, T y, T z) {
        m_data[0] = x;
        m_data[0] = y;
        m_data[0] = z;
    }
};

template <typename T = double> class point2 : public PointBase<T, 2> {
    point(T x, T y) {
        m_data[0] = x;
        m_data[0] = y;
    }
};

typedef point4<double> point4d;
typedef point4<float> point4f;
typedef point3<double, 3> point3d;
typedef point3<float, 3> point3f;
typedef point2<double, 2> point2d;
typedef point2<float, 2> point2f;

// Default Point a 4 dimensional double point
typedef point4d point;
*/

#endif // !POINT_H_
