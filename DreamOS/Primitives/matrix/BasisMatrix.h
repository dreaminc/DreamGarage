#ifndef BASIS_MATRIX_H_
#define BASIS_MATRIX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BasisMatrix.h
// Basis Matrix 
// A general purpose change of basis matrix
// TODO: Not currently checking for orthogonality 

#include "matrix.h"
#include "Primitives/point.h"
#include "Primitives/vector.h"

#ifdef FLOAT_PRECISION
typedef float basis_precision;
#elif defined(DOUBLE_PRECISION)
typedef double basis_precision;
#endif

class BasisMatrix : public matrix<basis_precision, 4, 4> {
public:
	BasisMatrix(vector vXAxis, vector vYAxis, vector vZAxis) {
		identity(1.0f);

		this->element(0, 0) = vXAxis.x();
		this->element(1, 0) = vXAxis.y();
		this->element(2, 0) = vXAxis.z();

		this->element(0, 1) = vYAxis.x();
		this->element(1, 1) = vYAxis.y();
		this->element(2, 1) = vYAxis.z();

		this->element(0, 2) = vZAxis.x();
		this->element(1, 2) = vZAxis.y();
		this->element(2, 2) = vZAxis.z();
	}

	~BasisMatrix() {
		// empty stub
	}

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Basis Matrix");
		return matrix<basis_precision, 4, 4>::PrintMatrix();
	}
};

#endif // ! BASIS_MATRIX_H_
