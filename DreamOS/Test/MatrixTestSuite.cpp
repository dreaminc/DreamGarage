#include "MatrixTestSuite.h"

#include "Primitives/matrix/matrix.h"

MatrixTestSuite::MatrixTestSuite() {
	// empty 
}

MatrixTestSuite::~MatrixTestSuite() {
	// empty
}

RESULT MatrixTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR((AddTest(std::bind(&MatrixTestSuite::TestMatrixCreate, this))));

Error:
	return r;
}

// Tests

template <typename typeMatrix, int N, int M>
RESULT TestMatrix(int numIterations) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Range Matrix %d x %d", N, M);

	matrix<typeMatrix, N, M> rangeMat = matrix<typeMatrix, N, M>::MakeRange();
	CBM((rangeMat.rows() == N), "Rows %d mismatch N value %d", rangeMat.rows(), N);
	CBM((rangeMat.cols() == M), "Columns %d mismatch M value %d", rangeMat.cols(), M);
	rangeMat.PrintMatrix();

	DEBUG_LINEOUT("Random Matrix %d x %d", N, M);

	for (int i = 0; i < numIterations; i++) {
		matrix<typeMatrix, N, M> testMat = matrix<typeMatrix, N, M>::MakeRandom();

		CBM((testMat.rows() == N), "Rows %d mismatch N value %d", testMat.rows(), N);
		CBM((testMat.cols() == M), "Columns %d mismatch M value %d", testMat.cols(), M);

		//testMat.PrintMatrix();
	}

// Success:
	DEBUG_LINEOUT("Matrix Test Success: %d x %d", N, M);
	return r;

Error:
	DEBUG_LINEOUT("Matrix Test Failed: %d x %d", N, M);
	return r;
}

// This will simply create a bunch of matrices of various
// sizes and confirm parameters 
RESULT MatrixTestSuite::TestMatrixCreate() {
	RESULT r = R_PASS;

	int numIterations = 5;

	CRM((TestMatrix<float, 1, 1>(numIterations)), "Test Matrix Create failed for float 1x1");
	CRM((TestMatrix<float, 1, 2>(numIterations)), "Test Matrix Create failed for float 1x2");
	CRM((TestMatrix<float, 1, 3>(numIterations)), "Test Matrix Create failed for float 1x3");
	CRM((TestMatrix<float, 1, 4>(numIterations)), "Test Matrix Create failed for float 1x4");

	CRM((TestMatrix<float, 2, 1>(numIterations)), "Test Matrix Create failed for float 2x1");
	CRM((TestMatrix<float, 2, 2>(numIterations)), "Test Matrix Create failed for float 2x2");
	CRM((TestMatrix<float, 2, 3>(numIterations)), "Test Matrix Create failed for float 2x3");
	CRM((TestMatrix<float, 2, 4>(numIterations)), "Test Matrix Create failed for float 2x4");

	CRM((TestMatrix<float, 3, 1>(numIterations)), "Test Matrix Create failed for float 3x1");
	CRM((TestMatrix<float, 3, 2>(numIterations)), "Test Matrix Create failed for float 3x2");
	CRM((TestMatrix<float, 3, 3>(numIterations)), "Test Matrix Create failed for float 3x3");
	CRM((TestMatrix<float, 3, 4>(numIterations)), "Test Matrix Create failed for float 3x4");

	CRM((TestMatrix<float, 4, 1>(numIterations)), "Test Matrix Create failed for float 4x1");
	CRM((TestMatrix<float, 4, 2>(numIterations)), "Test Matrix Create failed for float 4x2");
	CRM((TestMatrix<float, 4, 3>(numIterations)), "Test Matrix Create failed for float 4x3");
	CRM((TestMatrix<float, 4, 4>(numIterations)), "Test Matrix Create failed for float 4x4");

Error:
	return r;
}

