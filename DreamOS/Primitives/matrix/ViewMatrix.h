#ifndef VIEW_MATRIX_H_
#define VIEW_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ViewMatrix.h
// View Matrix 
// This is a 4x4 matrix for purpose of converting model space into world space
// by way of a view matrix

#include "matrix.h"
#include "Primitives/vector.h"
#include "Primitives/point.h"
#include "Primitives/quaternion.h"

#include "RotationMatrix.h"
#include "TranslationMatrix.h"
#include "BasisMatrix.h"

#ifdef FLOAT_PRECISION
	typedef float view_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double view_precision;
#endif

class ViewMatrix : public matrix<view_precision, 4, 4> {
public:
	ViewMatrix();
	ViewMatrix(point ptPosition);

	// This will start with i, j, k vectors and effectively rotate them about the appropriate axes 
	// pitch is about the x axis, yaw is about the y axis and roll is about the z axis
	ViewMatrix(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll);
	ViewMatrix(point ptPosition, quaternion qLook);
	ViewMatrix(point ptPosition, vector vLook);
	ViewMatrix(point ptPosition, BasisMatrix matBasis);

	~ViewMatrix();

	RESULT SetViewMatrixPitchYawRoll(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll);
	RESULT SetViewMatrixPointQuaternion(point ptPosition, quaternion qLook);
	RESULT SetViewMatrixPointVector(point ptPosition, vector vLook);
	RESULT SetViewMatrixPointBasis(point ptPosition, BasisMatrix matBasis);

	RotationMatrix GetRotationMatrix();

	point GetPosition();

	quaternion GetOrientation();

	RESULT PrintMatrix();

	// Explicitly specializing the assignment operator
	ViewMatrix& operator=(const matrix<view_precision, 4, 4> &arg) {
		if (this == &arg)      // Same object?
			return *this;        // Yes, so skip assignment, and just return *this.

		memcpy(this->m_data, arg.m_data, sizeof(view_precision) * 4 * 4);

		return *this;
	}
};

#endif // ! VIEW_MATRIX_H_
