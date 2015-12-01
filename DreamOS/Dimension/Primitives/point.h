#ifndef POINT_H_
#define POINT_H_

// DREAM OS
// DreamOS/Dimension/Primitives/point.h
// 64 bit Point Primitive Object

template <typename T = double, int dimension = 4>
class point {

    point() {
        memset(&m_data, 0, sizeof(T) * dimension);
    }

private:
    T m_data[dimensions];
}

// TODO: Convert to different classes
typedef point<double, 4> point4d;
typedef point<float, 4> point4f;
typedef point<double, 3> point3d;
typedef point<float, 3> point3f;
typedef point<double, 2> point2d;
typedef point<float, 2> point2f;

#endif // !POINT_H_
