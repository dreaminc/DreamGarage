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
	
	// This will start with i, j, k vectors and effectively rotate them about the appropriate axes 
	// pitch is about the x axis, yaw is about the y axis and roll is about the z axis
	ViewMatrix(point ptPosition, view_precision pitch, view_precision yaw, view_precision roll) {
		clear();
		SetViewMatrixPitchYawRoll(ptPosition, pitch, yaw, roll);
	}

	/*
	ViewMatrix(point ptPosition, vector vLook, vector vUp) {

	}
	*/

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

	~ViewMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("View Matrix");
		return matrix<view_precision, 4, 4>::PrintMatrix();
	}	
};

#endif // ! VIEW_MATRIX_H_
