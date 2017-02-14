#ifndef SCALING_MATRIX_H_
#define SCALING_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ScalingMatrix.h
// Scaling Matrix 
// A general purpose scaling matrix

#include "matrix.h"

#ifdef FLOAT_PRECISION
	typedef float scale_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double scale_precision;
#endif

class vector;

class ScalingMatrix : public matrix<scale_precision, 4, 4> {

	/*
	public:
	using matrix<scale_precision, 4, 4>::operator=;
	using matrix<scale_precision, 4, 4>::operator+;
	using matrix<scale_precision, 4, 4>::operator+=;
	*/

public:
	ScalingMatrix(vector vScale);
	ScalingMatrix(scale_precision x, scale_precision y, scale_precision z);
	~ScalingMatrix();

	RESULT PrintMatrix();
};

#endif // ! SCALING_MATRIX_H_
