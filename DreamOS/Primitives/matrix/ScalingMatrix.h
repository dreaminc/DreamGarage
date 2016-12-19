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

class ScalingMatrix : public matrix<scale_precision, 4, 4> {

	/*
	public:
	using matrix<scale_precision, 4, 4>::operator=;
	using matrix<scale_precision, 4, 4>::operator+;
	using matrix<scale_precision, 4, 4>::operator+=;
	*/

public:
	ScalingMatrix(scale_precision x, scale_precision y, scale_precision z) {
		this->clear();

		this->element(0, 0) = x;
		this->element(1, 1) = y;
		this->element(2, 2) = z;
		this->element(3, 3) = 1.0f;
	}

	~ScalingMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Scaling Matrix");
		return matrix<scale_precision, 4, 4>::PrintMatrix();
	}


	/*
	TranslationMatrix& TranslationMatrix::operator=(const matrix<translate_precision, 4, 4> &arg) {
	matrix<translate_precision, 4, 4>::operator=(arg);
	return *this;
	}

	explicit operator matrix<translate_precision, 4, 4>() { return *this; }

	matrix<TMatrix, N, M>& operator+=(const matrix<TMatrix, N, M>& rhs) {
	addData((TMatrix*)rhs.m_data);
	return *this;
	}

	// TODO: Is this the right way?
	friend matrix<TMatrix, N, M> operator+(const matrix<TMatrix, N, M>& lhs, const matrix<TMatrix, N, M>& rhs) {
	//lhs += rhs; // reuse compound assignment
	matrix newMatrix = lhs;
	newMatrix.addData(rhs);
	return newMatrix;
	}

	matrix<TMatrix, N, M>& operator+(const matrix<TMatrix, N, M>&arg) {
	return matrix<TMatrix, N, M>(*this).operator+=(arg);
	}
	*/
};

#endif // ! SCALING_MATRIX_H_
