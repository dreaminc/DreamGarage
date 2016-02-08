#ifndef TRANSLATION_MATRIX_H_
#define TRANSLATION_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/TranslationMatrix.h
// Translation Matrix 
// A general purpose translation matrix

#include "matrix.h"

#ifdef FLOAT_PRECISION
	typedef float translate_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double translate_precision;
#endif

class TranslationMatrix : public matrix<translate_precision, 4, 4> {
public:
	TranslationMatrix(translate_precision x, translate_precision y, translate_precision z) {
		this->identity();
		
		this->element(3, 0) = x;
		this->element(3, 1) = y;
		this->element(3, 2) = z;
	}

	~TranslationMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Translation Matrix");
		return matrix<translate_precision, 4, 4>::PrintMatrix();
	}
};

#endif // ! TRANSLATION_MATRIX_H_
