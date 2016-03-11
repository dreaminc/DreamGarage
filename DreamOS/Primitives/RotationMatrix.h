#ifndef ROTATION_MATRIX_H_
#define ROTATION_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/RotationMatrix.h
// Rotation Matrix 
// A general purpose rotation matrix

#include "matrix.h"
#include "vector.h"
#include "quaternion.h"

#ifdef FLOAT_PRECISION
	typedef float rotation_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double rotation_precision;
#endif

#ifdef RIGHT_HANDED
	#define ROTATION_HAND_SIGN 1
#elif defined(LEFT_HANDED)
	#define ROTATION_HAND_SIGN -1
#endif

class RotationMatrix : public matrix<rotation_precision, 4, 4> {
public:
	typedef enum {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		XYZ_AXIS,
		ARBITRARY_AXIS,
		QUATERNION,
		INVALID
	} ROTATION_MATRIX_TYPE;

	RotationMatrix(vector rotationAxis, rotation_precision theta) :
		m_type(ARBITRARY_AXIS)
	{
		SetArbitraryVectorRotationMatrix(rotationAxis, theta);
	}

	RotationMatrix(rotation_precision thetaX, rotation_precision thetaY, rotation_precision thetaZ) :
		m_type(XYZ_AXIS)
	{
		SetXYZRotationMatrix(thetaX, thetaY, thetaZ);
	}

	RotationMatrix(ROTATION_MATRIX_TYPE axis, rotation_precision theta) :
		m_type(axis)
	{
		switch (axis) {
			case X_AXIS: SetXRotationMatrix(theta); break;
			case Y_AXIS: SetYRotationMatrix(theta); break;
			case Z_AXIS: SetZRotationMatrix(theta); break;
		}
	}

	RotationMatrix(quaternion q) :
		m_type(QUATERNION)
	{
		SetQuaternionRotationMatrix(q);
	}

	// http://www.cprogramming.com/tutorial/3d/quaternions.html
	RESULT SetQuaternionRotationMatrix(quaternion q) {
		m_type = QUATERNION;
		q.Normalize();	

		this->identity();

		this->element(0, 0) = 1.0f - 2*q.y2() - 2*q.z2();
		this->element(0, 1) = 2*q.x()*q.y() - 2*q.w()*q.z();
		this->element(0, 2) = 2*q.x()*q.z() + 2*q.w()*q.y();

		this->element(1, 0) = 2*q.x()*q.y() + 2*q.w()*q.z();
		this->element(1, 1) = 1.0f - 2*q.x2() - 2*q.z2();
		this->element(1, 2) = 2*q.y()*q.z() - 2*q.w()*q.x();

		this->element(2, 0) = 2*q.x()*q.z() - 2*q.w()*q.y();
		this->element(2, 1) = 2*q.y()*q.z() + 2*q.w()*q.x();
		this->element(2, 2) = 1.0f - 2*q.x2() - 2*q.y2();

		return R_PASS;
	}

	// https://en.wikipedia.org/wiki/Rotation_matrix
	RESULT SetArbitraryVectorRotationMatrix(vector u, rotation_precision theta) {
		m_type = ARBITRARY_AXIS;

		this->identity();

		rotation_precision cosTheta = cos(theta);
		rotation_precision sinTheta = ROTATION_HAND_SIGN * sin(theta);

		this->element(0, 0) = cosTheta  + (pow(u.x(), 2) * (1 - cosTheta));
		this->element(0, 1) = (u.x() * u.y() * (1 - cosTheta)) - (u.z() * sinTheta);
		this->element(0, 2) = (u.x() * u.z() * (1 - cosTheta)) + (u.y() * sinTheta);

		this->element(1, 0) = (u.x() * u.y() * (1 - cosTheta)) + (u.z() * sinTheta);
		this->element(1, 1) = cosTheta + (pow(u.y(), 2) * (1 - cosTheta));
		this->element(1, 2) = (u.y() * u.z() * (1 - cosTheta)) - (u.x() * sinTheta);

		this->element(2, 0) = (u.x() * u.z() * (1 - cosTheta)) - (u.y() * sinTheta);
		this->element(2, 1) = (u.y() * u.z() * (1 - cosTheta)) + (u.x() * sinTheta);
		this->element(2, 2) = cosTheta + (pow(u.z(), 2) * (1 - cosTheta));

		return R_PASS;
	}

	// https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
	RESULT SetXYZRotationMatrix(rotation_precision phi, rotation_precision theta, rotation_precision psi) {
		m_type = XYZ_AXIS;
		this->identity();

		rotation_precision cosPhi = cos(phi);
		rotation_precision sinPhi = ROTATION_HAND_SIGN * sin(phi);

		rotation_precision cosTheta = cos(theta);
		rotation_precision sinTheta = ROTATION_HAND_SIGN * sin(theta);

		rotation_precision cosPsi = cos(psi);
		rotation_precision sinPsi = ROTATION_HAND_SIGN * sin(psi);

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
	RESULT SetXRotationMatrix(rotation_precision theta) {
		m_type = X_AXIS;

		this->identity();

		rotation_precision cosPhi = cos(theta);
		rotation_precision sinPhi = ROTATION_HAND_SIGN * sin(theta);

		this->element(1, 1) = cosPhi;
		this->element(1, 2) = -sinPhi;
		this->element(2, 1) = sinPhi;
		this->element(2, 2) = cosPhi;

		return R_PASS;
	}

	RESULT SetYRotationMatrix(rotation_precision theta) {
		m_type = Y_AXIS;

		this->identity();

		rotation_precision cosTheta = cos(theta);
		rotation_precision sinTheta = ROTATION_HAND_SIGN * sin(theta);

		this->element(0, 0) = cosTheta;
		this->element(0, 2) = sinTheta;
		this->element(2, 0) = -sinTheta;
		this->element(2, 2) = cosTheta;

		return R_PASS;
	}

	RESULT SetZRotationMatrix(rotation_precision theta) {
		m_type = Z_AXIS;

		this->identity();

		rotation_precision cosPsi = cos(theta);
		rotation_precision sinPsi = ROTATION_HAND_SIGN * sin(theta);

		this->element(0, 0) = cosPsi;
		this->element(0, 1) = -sinPsi;
		this->element(1, 0) = sinPsi;
		this->element(1, 1) = cosPsi;

		return R_PASS;
	}

	~RotationMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Rotation Matrix");
		return matrix<rotation_precision, 4, 4>::PrintMatrix();
	}

private:
	ROTATION_MATRIX_TYPE m_type;
};

#endif // ! TRANSLATION_MATRIX_H_
#pragma once
