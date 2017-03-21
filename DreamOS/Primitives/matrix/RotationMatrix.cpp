#include "RotationMatrix.h"

#include "Primitives/ray.h"

RotationMatrix::RotationMatrix() :
	m_type(ARBITRARY_AXIS)
{
	identity();
}

RotationMatrix::RotationMatrix(vector rotationAxis, rotation_precision theta) :
	m_type(ARBITRARY_AXIS)
{
	SetArbitraryVectorRotationMatrix(rotationAxis, theta);
}

RotationMatrix::RotationMatrix(rotation_precision thetaX, rotation_precision thetaY, rotation_precision thetaZ) :
	m_type(XYZ_AXIS)
{
	SetXYZRotationMatrix(thetaX, thetaY, thetaZ);
}

RotationMatrix::RotationMatrix(vector vEuler) :
	m_type(XYZ_AXIS)
{
	SetXYZRotationMatrix(vEuler.x(), vEuler.y(), vEuler.z());
}

RotationMatrix::RotationMatrix(ROTATION_MATRIX_TYPE axis, rotation_precision theta) :
	m_type(axis)
{
	switch (axis) {
	case X_AXIS: SetXRotationMatrix(theta); break;
	case Y_AXIS: SetYRotationMatrix(theta); break;
	case Z_AXIS: SetZRotationMatrix(theta); break;
	}
}

RotationMatrix::RotationMatrix(quaternion q) :
	m_type(QUATERNION)
{
	SetQuaternionRotationMatrix(q);
}

RotationMatrix::RotationMatrix(vector v, vector up) :
	m_type(VECTOR)
{
	SetVectorRotationMatrix(v, up);
}

RotationMatrix::~RotationMatrix() {
	// empty stub
}

// http://www.cprogramming.com/tutorial/3d/quaternions.html
RESULT RotationMatrix::SetQuaternionRotationMatrix(quaternion q) {
	m_type = QUATERNION;
	q.Normalize();

	this->identity();

	this->element(0, 0) = 1.0f - 2 * q.y2() - 2 * q.z2();
	this->element(0, 1) = 2 * q.x()*q.y() - 2 * q.w()*q.z();
	this->element(0, 2) = 2 * q.x()*q.z() + 2 * q.w()*q.y();

	this->element(1, 0) = 2 * q.x()*q.y() + 2 * q.w()*q.z();
	this->element(1, 1) = 1.0f - 2 * q.x2() - 2 * q.z2();
	this->element(1, 2) = 2 * q.y()*q.z() - 2 * q.w()*q.x();

	this->element(2, 0) = 2 * q.x()*q.z() - 2 * q.w()*q.y();
	this->element(2, 1) = 2 * q.y()*q.z() + 2 * q.w()*q.x();
	this->element(2, 2) = 1.0f - 2 * q.x2() - 2 * q.y2();

	return R_PASS;
}

// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
quaternion RotationMatrix::GetQuaternion() {
	quaternion q;
	q.w() = sqrt(1.0f + this->element(0, 0) + this->element(1, 1) + this->element(2, 2)) / 2.0f;

	float w4 = q.w() * 4.0f;

	q.x() = (this->element(1, 2) - this->element(2, 1)) / w4;
	q.y() = (this->element(2, 0) - this->element(0, 2)) / w4;
	q.z() = (this->element(0, 1) - this->element(1, 0)) / w4;

	return q;
}

// This is defined based on a basis of {1,0,0}, {0,1,0}, {0,0,-1}
RESULT RotationMatrix::SetVectorRotationMatrix(vector v, vector up) {
	m_type = VECTOR;
	v.Normalize();
	identity();

	vector zAxis = -1.0f * v.Normal();
	vector xAxis;

	if (((zAxis * up) != 0.0f) && (zAxis != up))
		xAxis = up.cross(zAxis);
	else
		xAxis = vector(-zAxis.y(), zAxis.x(), zAxis.z());

	xAxis.Normalize();

	vector yAxis = zAxis.cross(xAxis);
	yAxis.Normalize();

	this->element(0, 0) = xAxis.x();
	this->element(0, 1) = xAxis.y();
	this->element(0, 2) = xAxis.z();

	this->element(1, 0) = yAxis.x();
	this->element(1, 1) = yAxis.y();
	this->element(1, 2) = yAxis.z();

	this->element(2, 0) = zAxis.x();
	this->element(2, 1) = zAxis.y();
	this->element(2, 2) = zAxis.z();

	this->element(3, 3) = 1.0f;

	return R_PASS;
}

// https://en.wikipedia.org/wiki/Rotation_matrix
RESULT RotationMatrix::SetArbitraryVectorRotationMatrix(vector u, rotation_precision theta) {
	m_type = ARBITRARY_AXIS;

	this->identity();

	rotation_precision cosTheta = static_cast<rotation_precision>(theta);
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(theta);

	this->element(0, 0) = cosTheta + (static_cast<rotation_precision>(pow(u.x(), 2)) * (1 - cosTheta));
	this->element(0, 1) = (u.x() * u.y() * (1 - cosTheta)) - (u.z() * sinTheta);
	this->element(0, 2) = (u.x() * u.z() * (1 - cosTheta)) + (u.y() * sinTheta);

	this->element(1, 0) = (u.x() * u.y() * (1 - cosTheta)) + (u.z() * sinTheta);
	this->element(1, 1) = cosTheta + (static_cast<rotation_precision>(pow(u.y(), 2)) * (1 - cosTheta));
	this->element(1, 2) = (u.y() * u.z() * (1 - cosTheta)) - (u.x() * sinTheta);

	this->element(2, 0) = (u.x() * u.z() * (1 - cosTheta)) - (u.y() * sinTheta);
	this->element(2, 1) = (u.y() * u.z() * (1 - cosTheta)) + (u.x() * sinTheta);
	this->element(2, 2) = cosTheta + (static_cast<rotation_precision>(pow(u.z(), 2)) * (1 - cosTheta));

	return R_PASS;
}

// https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
RESULT RotationMatrix::SetXYZRotationMatrix(rotation_precision phi, rotation_precision theta, rotation_precision psi) {
	m_type = XYZ_AXIS;
	this->identity();

	rotation_precision cosPhi = static_cast<rotation_precision>(cos(phi));
	rotation_precision sinPhi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(phi));

	rotation_precision cosTheta = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	rotation_precision cosPsi = static_cast<rotation_precision>(cos(psi));
	rotation_precision sinPsi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(psi));

	this->element(0, 0) = cosTheta * cosPsi;
	this->element(0, 1) = (cosPhi * sinPsi) + (sinPhi * sinTheta * cosPsi);
	this->element(0, 2) = (sinPhi * sinPsi) - (cosPhi * sinTheta * cosPsi);

	this->element(1, 0) = -cosTheta * sinPsi;
	this->element(1, 1) = (cosPhi * cosPsi) - (sinPhi * sinTheta * sinPsi);
	this->element(1, 2) = (sinPhi * cosPsi) + (cosPhi * sinTheta * sinPsi);

	this->element(2, 0) = sinTheta;
	this->element(2, 1) = -sinPhi * cosTheta;
	this->element(2, 2) = cosPhi * cosTheta;

	return R_PASS;
}

// https://en.wikipedia.org/wiki/Rotation_matrix
RESULT RotationMatrix::SetXRotationMatrix(rotation_precision theta) {
	m_type = X_AXIS;

	this->identity();

	rotation_precision cosPhi = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinPhi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	this->element(1, 1) = cosPhi;
	this->element(1, 2) = -sinPhi;
	this->element(2, 1) = sinPhi;
	this->element(2, 2) = cosPhi;

	return R_PASS;
}

RESULT RotationMatrix::SetYRotationMatrix(rotation_precision theta) {
	m_type = Y_AXIS;

	this->identity();

	rotation_precision cosTheta = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinTheta = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	this->element(0, 0) = cosTheta;
	this->element(0, 2) = sinTheta;
	this->element(2, 0) = -sinTheta;
	this->element(2, 2) = cosTheta;

	return R_PASS;
}

RESULT RotationMatrix::SetZRotationMatrix(rotation_precision theta) {
	m_type = Z_AXIS;

	this->identity();

	rotation_precision cosPsi = static_cast<rotation_precision>(cos(theta));
	rotation_precision sinPsi = ROTATION_HAND_SIGN * static_cast<rotation_precision>(sin(theta));

	this->element(0, 0) = cosPsi;
	this->element(0, 1) = -sinPsi;
	this->element(1, 0) = sinPsi;
	this->element(1, 1) = cosPsi;

	return R_PASS;
}

RESULT RotationMatrix::PrintMatrix() {
	DEBUG_LINEOUT("Rotation Matrix");
	return matrix<rotation_precision, 4, 4>::PrintMatrix();
}