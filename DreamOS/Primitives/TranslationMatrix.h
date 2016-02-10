#ifndef TRANSLATION_MATRIX_H_
#define TRANSLATION_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/TranslationMatrix.h
// Translation Matrix 
// A general purpose translation matrix

#include "matrix.h"
#include "point.h"

#ifdef FLOAT_PRECISION
	typedef float translate_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double translate_precision;
#endif

class TranslationMatrix : public matrix<translate_precision, 4, 4> {

	/*
public:
	using matrix<translate_precision, 4, 4>::operator=;
	using matrix<translate_precision, 4, 4>::operator+;
	using matrix<translate_precision, 4, 4>::operator+=;
	*/

public:
	TranslationMatrix(translate_precision x, translate_precision y, translate_precision z) {
		this->identity();
		
		this->element(0, 3) = x;
		this->element(1, 3) = y;
		this->element(2, 3) = z;
	}

	TranslationMatrix(point p) {
		this->identity();

		this->element(0, 3) = p.x();
		this->element(1, 3) = p.y();
		this->element(2, 3) = p.z();
	}

	~TranslationMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Translation Matrix");
		return matrix<translate_precision, 4, 4>::PrintMatrix();
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

#endif // ! TRANSLATION_MATRIX_H_
