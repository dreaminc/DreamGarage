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

//#define DEFAULT_SLERP_PI_DIVIDER (32.0f)
//#define DEFAULT_SLERP_LIMIT_VALUE (quaternion_precision)(M_PI/(DEFAULT_SLERP_PI_DIVIDER))
#define DEFAULT_SLERP_LIMIT_VALUE 0.1f

constexpr quaternion_precision operator "" _q(long double number)
{
	return static_cast<quaternion_precision>(number);
}

typedef struct {
	quaternion_precision x, y, z, w;
} quaternionXYZW;

class quaternion {
public:
	quaternion();
	quaternion(vector v);
	quaternion(quaternion_precision theta, vector vectorAxis);
	quaternion(quaternion_precision theta, quaternion_precision x, quaternion_precision y, quaternion_precision z);
	quaternion(quaternion_precision values[4]);
	quaternion(quaternionXYZW qXYZW);
	quaternion(vector i, vector j, vector k);

	RESULT Print(char *pszOptName = nullptr);

	RESULT SetQuaternion(quaternion_precision theta, quaternion_precision x, quaternion_precision y, quaternion_precision z);
	RESULT SetQuaternion(vector i, vector j, vector k);
	RESULT SetQuaternionBasis(vector i, vector j, vector k);

	RESULT SetValues(quaternion_precision w, quaternion_precision x, quaternion_precision y, quaternion_precision z);
	RESULT SetValues(quaternion_precision values[4]);
	RESULT SetValues(quaternionXYZW qXYZW);
	RESULT SetValuesXYZW(quaternion_precision values[4]);

	quaternion Normalize();
	quaternion_precision Magnitude();

	RESULT RotateByVector(vector v, quaternion_precision theta);
	RESULT RotateByVectorSlerp(vector v, quaternion_precision theta, quaternion_precision slerpLimitValue = DEFAULT_SLERP_LIMIT_VALUE);

	RESULT RotateX(quaternion_precision theta);
	RESULT RotateY(quaternion_precision theta);
	RESULT RotateZ(quaternion_precision theta);
	RESULT RotateXYZ(quaternion_precision phi, quaternion_precision theta, quaternion_precision psi);

	// Currently Unstable
	// Euler Conversions: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	quaternion_precision GetEulerAngleZ();
	quaternion_precision GetEulerAngleX();
	quaternion_precision GetEulerAngleY();
	RESULT GetEulerAngles(quaternion_precision *x, quaternion_precision *y, quaternion_precision *z);

	static quaternion MakeQuaternionWithEuler(quaternion_precision x, quaternion_precision y, quaternion_precision z);

	RESULT Reverse();
	quaternion GetReverse();
	quaternion GetConjugate();
	static quaternion Conjugate(quaternion arg);

	vector GetVector();

	// http://www.mathworks.com/help/aeroblks/quaternionrotation.html
	vector RotateVector(vector v);

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

	quaternion& operator*=(const quaternion& r);
	const quaternion& operator*(const quaternion& arg) const;

	quaternion& operator+=(const quaternion& rhs);
	const quaternion& operator+(const quaternion& arg) const;

	quaternion& operator-=(const quaternion& rhs);
	const quaternion& operator-(const quaternion& arg) const;

	// Utility
public:
	static quaternion iQuaternion(quaternion_precision theta);
	static quaternion jQuaternion(quaternion_precision theta);
	static quaternion kQuaternion(quaternion_precision theta);

private:
	// theta (q0)
	quaternion_precision m_w;

	// vector (q1, q2, q3)
	quaternion_precision m_x;
	quaternion_precision m_y;
	quaternion_precision m_z;
};

#endif // ! QUATERNION_H_