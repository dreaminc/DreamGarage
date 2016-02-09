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

	ViewMatrix(point ptPosition, view_precision pitch, view_precision yaw) {
		
	}

	ViewMatrix(point ptPosition, vector vLook, vector vUp) {

	}

	~ViewMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("View Matrix");
		return matrix<view_precision, 4, 4>::PrintMatrix();
	}

private:
	
};

#endif // ! VIEW_MATRIX_H_
