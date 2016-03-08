#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/quaternion.h
// A quaternion is vector and magnitude such that the combination of the two
// define a given rotation about the vector in space.  These can be used to represent
// efficient rotations in 3D that also avoid the problem of gimbal lock

#include <math.h>

#ifdef FLOAT_PRECISION
	typedef float quaternion_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double quaternion_precision;
#endif

class quaternion {
public:
	quaternion() {
		SetValues(0, 0, 0, 0);
	}

	quaternion(quaternion_precision w, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
		SetValues(w, x, y, z);
	}

	RESULT SetValues(quaternion_precision w, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
		m_w = w;
		m_x = x;
		m_y = y;
		m_z = z;

		return R_PASS;
	}

	RESULT Normalize() {
		quaternion_precision magnitude = sqrt(pow(m_w, 2) + pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2));

		m_w /= magnitude;
		m_x /= magnitude;
		m_y /= magnitude;
		m_z /= magnitude;

		return R_PASS;
	}

	RESULT RotateByVector(vector v, quaternion_precision theta) {
		quaternion localRotation;
		quaternion_precision halfTheta = theta / 2.0f;

		localRotation.w() = cos(halfTheta);
		localRotation.x() = v.x() * sin(halfTheta);
		localRotation.y() = v.y() * sin(halfTheta);
		localRotation.z() = v.z() * sin(halfTheta);

		quaternion(*this).operator*=(localRotation);

		return R_PASS;
	}

	RESULT RotateX(quaternion_precision xDegrees) {

	}

	// TODO: Understand performance implications of this although both element and this are inline
	inline quaternion_precision &w() { return m_w; }
	inline quaternion_precision &x() { return m_x; }
	inline quaternion_precision &y() { return m_y; }
	inline quaternion_precision &z() { return m_z; }

	inline quaternion_precision w2() { return (m_w * m_w); }
	inline quaternion_precision x2() { return (m_x * m_x); }
	inline quaternion_precision y2() { return (m_y * m_y); }
	inline quaternion_precision z2() { return (m_z * m_z); }

	inline quaternion_precision &w(quaternion_precision val) { return m_w = val; }
	inline quaternion_precision &x(quaternion_precision val) { return m_x = val; }
	inline quaternion_precision &y(quaternion_precision val) { return m_y = val; }
	inline quaternion_precision &z(quaternion_precision val) { return m_z = val; }

	quaternion& operator*=(const quaternion& rhs) {
		m_w = m_w * rhs.m_w - m_x*rhs.m_x - m_y*rhs.m_y - m_z*rhs.m_z;
		m_x = m_w * rhs.m_x + m_x*rhs.m_w + m_y*rhs.m_z - m_z*rhs.m_y;
		m_y = m_w * rhs.m_y - m_x*rhs.m_z + m_y*rhs.m_w + m_z*rhs.m_x;
		m_z = m_w * rhs.m_z + m_x*rhs.m_y - m_y*rhs.m_x + m_z*rhs.m_w;

		return *this;
	}

	const quaternion& operator*(const quaternion& arg) const {
		return quaternion(*this).operator*=(arg);
	}

	quaternion& operator+=(const quaternion& rhs) {
		m_w += rhs.m_w;
		m_x += rhs.m_x;
		m_y += rhs.m_y;
		m_z += rhs.m_z;
		return *this;
	}

	const quaternion& operator+(const quaternion& arg) const {
		return quaternion(*this).operator+=(arg);
	}

	quaternion& operator-=(const quaternion& rhs) {
		m_w -= rhs.m_w;
		m_x -= rhs.m_x;
		m_y -= rhs.m_y;
		m_z -= rhs.m_z;
		return *this;
	}

	const quaternion& operator+(const quaternion& arg) const {
		return quaternion(*this).operator-=(arg);
	}

private:
	quaternion_precision m_w;
	quaternion_precision m_x;
	quaternion_precision m_y;
	quaternion_precision m_z;
};

#endif // ! QUATERNION_H_