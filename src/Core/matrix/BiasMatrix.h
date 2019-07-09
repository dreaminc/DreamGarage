#ifndef BIAS_MATRIX_H_
#define BIAS_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BiasMatrix.h
// Bias Matrix 
// A general purpose bias matrix

#include "matrix.h"
#include "Primitives/point.h"

#ifdef FLOAT_PRECISION
	typedef float bias_precision;
#elif defined(DOUBLE_PRECISION)
	typedef double bias_precision;
#endif

class BiasMatrix : public matrix<bias_precision, 4, 4> {
public:
	BiasMatrix(bias_precision val) {
		this->clear();

		this->element(0, 0) = val;
		
		this->element(1, 1) = val;
		
		this->element(2, 2) = val;

		this->element(3, 0) = val;
		this->element(3, 1) = val;
		this->element(3, 2) = val;

		this->element(3, 3) = 1.0f;
	}

	~BiasMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Bias Matrix");
		return matrix<bias_precision, 4, 4>::PrintMatrix();
	}
};

#endif // ! BIAS_MATRIX_H_
