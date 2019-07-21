#ifndef TRANSLATION_MATRIX_H_
#define TRANSLATION_MATRIX_H_

#include "core/ehm/EHM.h"

// DREAM OS Core
// dos/src/core/matrix/TranslationMatrix.h

// Translation Matrix 
// A general purpose translation matrix

#include "core/matrix/matrix.h"
#include "core/primitives/point.h"

#ifdef FLOAT_PRECISION
	typedef float translate_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double translate_precision;
#endif

class TranslationMatrix : public matrix<translate_precision, 4, 4> {

public:
	TranslationMatrix(translate_precision x, translate_precision y, translate_precision z);
	TranslationMatrix(point p);
	TranslationMatrix(point ptTranslate, point ptPivot);

	~TranslationMatrix();

	RESULT PrintMatrix();	
	
};

#endif // ! TRANSLATION_MATRIX_H_
