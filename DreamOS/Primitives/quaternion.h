#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/quaternion.h
// A quaternion is vector and magnitude such that the combination of the two
// define a given rotation about the vector in space.  These can be used to represent
// efficient rotations in 3D that also avoid the problem of gimbal lock

#include <math.h>

#include "vector.h"

#ifdef FLOAT_PRECISION
	typedef float quaternion_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double quaternion_precision;
#endif

constexpr quaternion_precision operator "" _q(long double number)
{
	return static_cast<quaternion_precision>(number);
}

typedef struct {
	quaternion_precision x, y, z, w;
} quaternionXYZW;

class quaternion {
public:
	quaternion() {
		SetQuaternion(0.0f, 0.0f, 1.0f, 0.0f);
		Normalize();
	}

	quaternion(vector v) {
		SetValues(0.0f, v.x(), v.y(), v.z());
		Normalize();
	}

	quaternion(quaternion_precision theta, vector vectorAxis) {
		SetQuaternion(theta, vectorAxis.x(), vectorAxis.y(), vectorAxis.z());
	}

	quaternion(quaternion_precision theta, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
		SetQuaternion(theta, x, y, z);
	}

	quaternion(quaternion_precision values[4]) {
		SetValues(values);
		Normalize();
	}

	quaternion(quaternionXYZW qXYZW) {
		SetValues(qXYZW);
		Normalize();
	}

	RESULT Reverse() {
		//m_w = m_w;

		m_x *= -1.0f;
		m_y *= -1.0f;
		m_z *= -1.0f;

		return R_PASS;
	}

	RESULT SetQuaternion(quaternion_precision theta, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
		quaternion_precision factor = static_cast<quaternion_precision>(sin(theta / 2.0));

		quaternion_precision w = static_cast<quaternion_precision>(cos(theta / 2.0f));

		x *= factor;
		y *= factor;
		z *= factor;

		SetValues(w, x, y, z);
		
		Normalize();

		return R_PASS;
	}

	RESULT SetValues(quaternion_precision w, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
		m_w = w;
		m_x = x;
		m_y = y;
		m_z = z;

		if (Magnitude() > 0)
			return R_PASS;
		else
			return R_INVALID_PARAM;
	}

	RESULT SetValues(quaternion_precision values[4]) {
		m_w = values[0];
		m_x = values[1];
		m_y = values[2];
		m_z = values[3];

		if (Magnitude() > 0)
			return R_PASS;
		else
			return R_INVALID_PARAM;
	}

	RESULT SetValues(quaternionXYZW qXYZW) {
		m_w = qXYZW.w;
		m_x = qXYZW.x;
		m_y = qXYZW.y;
		m_z = qXYZW.z;

		if (Magnitude() > 0)
			return R_PASS;
		else
			return R_INVALID_PARAM;
	}

	RESULT SetValuesXYZW(quaternion_precision values[4]) {
		m_x = values[0];
		m_y = values[1];
		m_z = values[2];
		m_w = values[3];

		if (Magnitude() > 0)
			return R_PASS;
		else
			return R_INVALID_PARAM;
	}

	quaternion Normalize() {
		quaternion_precision magnitude = static_cast<quaternion_precision>(sqrt(pow(m_w, 2) + pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2)));

		m_w /= magnitude;
		m_x /= magnitude;
		m_y /= magnitude;
		m_z /= magnitude;

		return (*this);
	}

	quaternion_precision Magnitude() {
		return static_cast<quaternion_precision>(pow(m_w, 2) + pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2));
	}

	RESULT RotateByVector(vector v, quaternion_precision theta) {
		quaternion localRotation(theta, v);
		(*this) *= localRotation;

		return R_PASS;
	}

	RESULT RotateX(quaternion_precision theta) {
		return RotateByVector(vector::iVector(), theta);
	}

	RESULT RotateY(quaternion_precision theta) {
		return RotateByVector(vector::jVector(), theta);
	}

	RESULT RotateZ(quaternion_precision theta) {
		return RotateByVector(vector::kVector(), theta);
	}

	// Euler Conversions: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	quaternion_precision GetEulerAngelZ() {
		quaternion_precision phi = static_cast<quaternion_precision>( atan2((2.0f * ((m_w * m_x) + (m_y * m_z))), (1 - 2.0f * (m_x*m_x + m_y*m_y))) );
		return phi;
	}

	quaternion_precision GetEulerAngelX() {
		quaternion_precision theta = static_cast<quaternion_precision>( asin((2.0f * ((m_w * m_y) - (m_z * m_x)))) );
		return theta;
	}

	quaternion_precision GetEulerAngelY() {
		quaternion_precision psi = static_cast<quaternion_precision>( atan2((2.0f * ((m_w * m_z) + (m_x * m_y))), (1 - 2.0f * (m_y*m_y + m_z*m_z))) );
		return psi;
	}

	RESULT GetEulerAngles(quaternion_precision *x, quaternion_precision *y, quaternion_precision *z) {
		*x = GetEulerAngelX();
		*y = GetEulerAngelY();
		*z = GetEulerAngelZ();
		return R_PASS;
	}

	quaternion GetConjugate() {
		quaternion q;

		q.m_w = m_w;
		
		q.m_x = -1.0f * m_x;
		q.m_y = -1.0f * m_y;
		q.m_z = -1.0f * m_z;

		return q;
	}

	static quaternion Conjugate(quaternion arg) {
		return arg.GetConjugate();
	}

	vector GetVector() {
		return vector(m_x, m_y, m_z);
	}

	// http://www.mathworks.com/help/aeroblks/quaternionrotation.html
	vector RotateVector(vector v) {
		vector retVal;

		retVal.x() = v.x() * (1.0f - 2 * (y2() + z2()))	+
				     v.y() * (2 * (x()*y() + w()*z()))	+
					 v.z() * (2 * (x()*z() - w()*y()));

		retVal.y() = v.x() * (2 * (x()*y() - w()*z())) +
					 v.y() * (1.0f - 2 * (x2() + z2())) +
					 v.z() * (2 * (y()*z() + w()*x()));

		retVal.z() = v.x() * (2 * (x()*z() + w()*y())) +
					 v.y() * (2 * (y()*z() - w()*x())) +
					 v.z() * (1.0f - 2 * (x2() + y2()));

		return retVal;
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

	// http://www.mathworks.com/help/aeroblks/quaternionmultiplication.html
	quaternion& operator*=(const quaternion& r) {
		/*
		m_w = m_w * r.m_w - m_x*r.m_x - m_y*r.m_y - m_z*r.m_z;
		m_x = m_w * r.m_x + m_x*r.m_w - m_y*r.m_z + m_z*r.m_y;
		m_y = m_w * r.m_y + m_x*r.m_z + m_y*r.m_w - m_z*r.m_x;
		m_z = m_w * r.m_z - m_x*r.m_y + m_y*r.m_x + m_z*r.m_w;
		//*/

		///*
		m_w = r.m_w * m_w - r.m_x * m_x - r.m_y * m_y - r.m_z * m_z;
		m_x = r.m_w * m_x + r.m_x * m_w - r.m_y * m_z + r.m_z * m_y;
		m_y = r.m_w * m_y + r.m_x * m_z + r.m_y * m_w - r.m_z * m_x;
		m_z = r.m_w * m_z - r.m_x * m_y + r.m_y * m_x + r.m_z * m_w;
		//*/

		return (*this);
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

	const quaternion& operator-(const quaternion& arg) const {
		return quaternion(*this).operator-=(arg);
	}

	// Utility
public:
	static quaternion iQuaternion(quaternion_precision theta) { 
		return quaternion(theta, 1.0f, 0.0f, 0.0f); 
	}

	static quaternion jQuaternion(quaternion_precision theta) { 
		return quaternion(theta, 0.0f, 1.0f, 0.0f); 
	}
	
	static quaternion kQuaternion(quaternion_precision theta) { 
		return quaternion(theta, 0.0f, 0.0f, 1.0f); 
	}

private:
	// theta (q0)
	quaternion_precision m_w;

	// vector (q1, q2, q3)
	quaternion_precision m_x;
	quaternion_precision m_y;
	quaternion_precision m_z;
};

#endif // ! QUATERNION_H_