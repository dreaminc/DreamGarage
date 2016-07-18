#ifndef VIEW_MATRIX_H_
#define VIEW_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ViewMatrix.h
// View Matrix 
// This is a 4x4 matrix for purpose of converting model space into world space
// by way of a view matrix

#include "matrix.h"
#include "vector.h"
#include "point.h"
#include "quaternion.h"

#include "RotationMatrix.h"
#include "TranslationMatrix.h"

#ifdef FLOAT_PRECISION
	typedef float view_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double view_precision;
#endif

class ViewMatrix : public matrix<view_precision, 4, 4> {
public:
	ViewMatrix() {
		clear();
	}
	
	ViewMatrix(point ptPosition) {
		clear();
		SetViewMatrixPitchYawRoll(ptPosition, 0.0f, 0.0f, 0.0f);
	}

	// This will start with i, j, k vectors and effectively rotate them about the appropriate axes 
	// pitch is about the x axis, yaw is about the y axis and roll is about the z axis
	ViewMatrix(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll) {
		clear();
		SetViewMatrixPitchYawRoll(ptPosition, pitch, yaw, roll);
	}

	ViewMatrix(point ptPosition, quaternion qLook) {
		clear();
		SetViewMatrixPointQuaternion(ptPosition, qLook);
	}

	RESULT SetViewMatrixPitchYawRoll(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll) {
		//m_ptPosition = ptPosition;

		/*
		m_vLook = matrixRotation * vector(0.0f, 0.0f, 1.0f);
		m_vUp = matrixRotation * vector(0.0f, 1.0f, 0.0f);
		m_vRight = matrixRotation * vector(1.0f, 1.0f, 1.0f);
		*/

		return SetMatrix(RotationMatrix(pitch, yaw, roll) * TranslationMatrix(ptPosition));
		//return SetMatrix(TranslationMatrix(ptPosition));
	}

	RESULT SetViewMatrixPointQuaternion(point ptPosition, quaternion qLook) {
		return SetMatrix(RotationMatrix(qLook) * TranslationMatrix(ptPosition));
	}

	~ViewMatrix() {
		// empty stub
	}

	RotationMatrix GetRotationMatrix() {
		RotationMatrix matRotation;

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				matRotation.element(i, j) = this->element(i, j);

		return matRotation;
	}

	point GetPosition() {
		view_precision x = m_data[3];
		view_precision y = m_data[(4) + 3];
		view_precision z = m_data[(8) + 3];
		//view_precision w = this->element(3, 3);
		view_precision w = 1.0f;

		return point(x, y, z, 1.0f);
	}

	quaternion GetOrientation() {
		return GetRotationMatrix().GetQuaternion();
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("View Matrix");
		return matrix<view_precision, 4, 4>::PrintMatrix();
	}	

	// Explicitly specializing the assignment operator
	ViewMatrix& operator=(const matrix<view_precision, 4, 4> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(view_precision) * 4 * 4);

		return *this;
	}
};

#endif // ! VIEW_MATRIX_H_
