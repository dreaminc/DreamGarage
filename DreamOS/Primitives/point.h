#ifndef POINT_H_
#define POINT_H_

// DREAM OS
// DreamOS/Dimension/Primitives/point.h
// Point Primitive Object derived from matrix

#include "matrix.h"

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

#endif // !POINT_H_
