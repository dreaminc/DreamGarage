#include "quaternion.h"

quaternion::quaternion() {
	SetQuaternion(0.0f, 0.0f, 0.0f, -1.0f);
	Normalize();
}

quaternion::quaternion(vector v) {
	SetValues(0.0f, v.x(), v.y(), v.z());
	Normalize();
}

quaternion::quaternion(quaternion_precision theta, vector vectorAxis) {
	SetQuaternion(theta, vectorAxis.x(), vectorAxis.y(), vectorAxis.z());
}

quaternion::quaternion(quaternion_precision theta, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	SetQuaternion(theta, x, y, z);
}

quaternion::quaternion(quaternion_precision values[4]) {
	SetValues(values);
	Normalize();
}

quaternion::quaternion(quaternionXYZW qXYZW) {
	SetValues(qXYZW);
	Normalize();
}

// From basis
quaternion::quaternion(vector i, vector j, vector k) {
	SetQuaternion(i, j, k);
}

RESULT quaternion::Reverse() {
	//m_w = m_w;

	m_x *= -1.0f;
	m_y *= -1.0f;
	m_z *= -1.0f;

	return R_PASS;
}

quaternion quaternion::GetReverse() {
	quaternion qNew = (*this);
	qNew.Reverse();
	return qNew;
}

RESULT quaternion::Print(char *pszOptName) {
	DEBUG_LINEOUT("%s(%f, %f, %f, %f)", (pszOptName != nullptr) ? pszOptName : "pt", m_x, m_y, m_z, m_w);
	return R_PASS;
}

RESULT quaternion::SetQuaternion(quaternion_precision theta, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	quaternion_precision factor = static_cast<quaternion_precision>(sin(theta / 2.0));
	quaternion_precision w = static_cast<quaternion_precision>(cos(theta / 2.0f));

	x *= factor;
	y *= factor;
	z *= factor;

	SetValues(w, x, y, z);

	Normalize();

	return R_PASS;
}

RESULT quaternion::SetQuaternion(vector i, vector j, vector k) {
	vector in = i.Normal();
	vector jn = j.Normal();
	vector kn = k.Normal();

	quaternion_precision w = 0.5f * sqrt(1.0f + in.x() + jn.y() + kn.z());

	quaternion_precision w4 = w * 4.0f;

	quaternion_precision x = (kn.y() - jn.z()) / w4;
	quaternion_precision y = (in.z() - kn.x()) / w4;
	quaternion_precision z = (jn.x() - in.y()) / w4;

	SetValues(w, x, y, z);

	Normalize();

	return R_PASS;
}

RESULT quaternion::SetValues(quaternion_precision w, quaternion_precision x, quaternion_precision y, quaternion_precision z) {
	m_w = w;
	m_x = x;
	m_y = y;
	m_z = z;

	if (Magnitude() > 0)
		return R_PASS;
	else
		return R_INVALID_PARAM;
}

RESULT quaternion::SetValues(quaternion_precision values[4]) {
	m_w = values[0];
	m_x = values[1];
	m_y = values[2];
	m_z = values[3];

	if (Magnitude() > 0)
		return R_PASS;
	else
		return R_INVALID_PARAM;
}

RESULT quaternion::SetValues(quaternionXYZW qXYZW) {
	m_w = qXYZW.w;
	m_x = qXYZW.x;
	m_y = qXYZW.y;
	m_z = qXYZW.z;

	if (Magnitude() > 0)
		return R_PASS;
	else
		return R_INVALID_PARAM;
}

RESULT quaternion::SetValuesXYZW(quaternion_precision values[4]) {
	m_x = values[0];
	m_y = values[1];
	m_z = values[2];
	m_w = values[3];

	if (Magnitude() > 0)
		return R_PASS;
	else
		return R_INVALID_PARAM;
}

quaternion quaternion::Normalize() {
	quaternion_precision magnitude = static_cast<quaternion_precision>(sqrt(pow(m_w, 2) + pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2)));

	m_w /= magnitude;
	m_x /= magnitude;
	m_y /= magnitude;
	m_z /= magnitude;

	return (*this);
}

quaternion_precision quaternion::Magnitude() {
	return static_cast<quaternion_precision>(pow(m_w, 2) + pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2));
}

RESULT quaternion::RotateByVector(vector v, quaternion_precision theta, quaternion_precision slerpLimitValue) {
	
	// Slerping to get this to work for arbitrary angles
	
	bool fPositive = (theta > 0.0f) ? true : false;
	quaternion_precision thetaLeft = theta;

	while(fabs((double)(thetaLeft)) != 0.0f) {
		if (thetaLeft > slerpLimitValue) {
			quaternion localRotation(slerpLimitValue, v);
			(*this) *= localRotation;
		}
		else {
			quaternion localRotation(thetaLeft, v);
			(*this) *= localRotation;
		}

		if (fPositive) {
			thetaLeft -= slerpLimitValue;
			if (thetaLeft < 0.0f)
				thetaLeft = 0.0f;
		}
		else {
			thetaLeft += slerpLimitValue;
			if (thetaLeft > 0.0f)
				thetaLeft = 0.0f;
		}

		Normalize();
	}

	return R_PASS;
}

RESULT quaternion::RotateX(quaternion_precision theta) {
	return RotateByVector(vector::iVector(), theta);
}

RESULT quaternion::RotateY(quaternion_precision theta) {
	return RotateByVector(vector::jVector(), theta);
}

RESULT quaternion::RotateZ(quaternion_precision theta) {
	return RotateByVector(vector::kVector(), theta);
}

// Euler Conversions: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
quaternion_precision quaternion::GetEulerAngelZ() {
	quaternion_precision phi = static_cast<quaternion_precision>(atan2((2.0f * ((m_w * m_x) + (m_y * m_z))), (1 - 2.0f * (m_x*m_x + m_y*m_y))));
	return phi;
}

quaternion_precision quaternion::GetEulerAngelX() {
	quaternion_precision theta = static_cast<quaternion_precision>(asin((2.0f * ((m_w * m_y) - (m_z * m_x)))));
	return theta;
}

quaternion_precision quaternion::GetEulerAngelY() {
	quaternion_precision psi = static_cast<quaternion_precision>(atan2((2.0f * ((m_w * m_z) + (m_x * m_y))), (1 - 2.0f * (m_y*m_y + m_z*m_z))));
	return psi;
}

RESULT quaternion::GetEulerAngles(quaternion_precision *x, quaternion_precision *y, quaternion_precision *z) {
	*x = GetEulerAngelX();
	*y = GetEulerAngelY();
	*z = GetEulerAngelZ();
	return R_PASS;
}

quaternion quaternion::GetConjugate() {
	quaternion q;

	q.m_w = m_w;

	q.m_x = -1.0f * m_x;
	q.m_y = -1.0f * m_y;
	q.m_z = -1.0f * m_z;

	return q;
}

quaternion quaternion::Conjugate(quaternion arg) {
	return arg.GetConjugate();
}

vector quaternion::GetVector() {
	return vector(m_x, m_y, m_z);
}

// http://www.mathworks.com/help/aeroblks/quaternionrotation.html
vector quaternion::RotateVector(vector v) {
	vector retVal;

	retVal.x() =	v.x() * (1.0f - 2.0f * (y2() + z2())) +
					v.y() * (2.0f * (x()*y() + w()*z())) +
					v.z() * (2.0f * (x()*z() - w()*y()));

	retVal.y() =	v.x() * (2.0f * (x()*y() - w()*z())) +
					v.y() * (1.0f - 2.0f * (x2() + z2())) +
					v.z() * (2.0f * (y()*z() + w()*x()));

	retVal.z() =	v.x() * (2.0f * (x()*z() + w()*y())) +
					v.y() * (2.0f * (y()*z() - w()*x())) +
					v.z() * (1.0f - 2.0f * (x2() + y2()));

	return retVal;
}



// http://www.mathworks.com/help/aeroblks/quaternionmultiplication.html
quaternion& quaternion::operator*=(const quaternion& r) {
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

const quaternion& quaternion::operator*(const quaternion& arg) const {
	return quaternion(*this).operator*=(arg);
}

quaternion& quaternion::operator+=(const quaternion& rhs) {
	m_w += rhs.m_w;
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	m_z += rhs.m_z;

	return *this;
}

const quaternion& quaternion::operator+(const quaternion& arg) const {
	return quaternion(*this).operator+=(arg);
}

quaternion& quaternion::operator-=(const quaternion& rhs) {
	m_w -= rhs.m_w;
	m_x -= rhs.m_x;
	m_y -= rhs.m_y;
	m_z -= rhs.m_z;

	return *this;
}

const quaternion& quaternion::operator-(const quaternion& arg) const {
	return quaternion(*this).operator-=(arg);
}

// Utility
quaternion quaternion::iQuaternion(quaternion_precision theta) {
	return quaternion(theta, 1.0f, 0.0f, 0.0f);
}

quaternion quaternion::jQuaternion(quaternion_precision theta) {
	return quaternion(theta, 0.0f, 1.0f, 0.0f);
}

quaternion quaternion::kQuaternion(quaternion_precision theta) {
	return quaternion(theta, 0.0f, 0.0f, 1.0f);
}