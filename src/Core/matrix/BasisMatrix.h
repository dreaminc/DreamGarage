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
	enum type {
		POS_X,
		NEG_X,
		POS_Y,
		NEG_Y,
		POS_Z,
		NEG_Z,
		INVALID
	};

public:
	BasisMatrix(vector vXAxis, vector vYAxis, vector vZAxis);
	BasisMatrix(BasisMatrix::type matType);

	~BasisMatrix();

private:
	RESULT SetBasisMatrixType(BasisMatrix::type matType);
	RESULT SetBasisMatrixVectors(vector vA, vector vB, vector vC);

	RESULT PrintMatrix() {
		DEBUG_LINEOUT("Basis Matrix");
		return matrix<basis_precision, 4, 4>::PrintMatrix();
	}
};

#endif // ! BASIS_MATRIX_H_
