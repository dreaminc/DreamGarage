#include "MatrixTestSuite.h"

#include "Primitives/matrix/matrix.h"
#include "Primitives/matrix/TranslationMatrix.h"

MatrixTestSuite::MatrixTestSuite() {
	// empty 
}

MatrixTestSuite::~MatrixTestSuite() {
	// empty
}

RESULT MatrixTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR((AddTest(std::bind(&MatrixTestSuite::TestMatrixCreate, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestMatrixMultiply, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestMatrixDeterminant, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestMatrixMinor, this))));

	CR((AddTest(std::bind(&MatrixTestSuite::TestTranslateMatrix, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestRotateMatrix, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestViewMatrix, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestProjectionMatrix, this))));
	CR((AddTest(std::bind(&MatrixTestSuite::TestScaleMatrix, this))));

Error:
	return r;
}

// Tests

template <typename typeMatrix, int N, int M>
RESULT TestMatrix(int numIterations) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Range Matrix By Value %d x %d", N, M);

	matrix<typeMatrix, N, M> rangeMat = matrix<typeMatrix, N, M>::MakeRange();
	matrix<typeMatrix, N, M> rangeByElementMat = matrix<typeMatrix, N, M>::MakeRangeByElement();
	
	CBM((rangeMat.rows() == N), "Rows %d mismatch N value %d", rangeMat.rows(), N);
	CBM((rangeMat.cols() == M), "Columns %d mismatch M value %d", rangeMat.cols(), M);
	
	DEBUG_LINEOUT("Range Matrix By Element %d x %d", N, M);

	rangeMat.PrintMatrix();
	rangeByElementMat.PrintMatrix();

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

template <typename typeMatrix, int N, int M>
RESULT TestMatrixMultiplication() {
	RESULT r = R_PASS;
	
	DEBUG_LINEOUT("TEST MATRIX MULTIPLICATION %d x %d", N, M);

	matrix<typeMatrix, N, M> rangeMat = matrix<typeMatrix, N, M>::MakeRange();
	matrix<typeMatrix, N, M> rangeByElementMat = matrix<typeMatrix, N, M>::MakeRangeByElement();

	matrix<typeMatrix, N, M> resultMatrixRange = rangeMat * rangeMat;
	matrix<typeMatrix, N, M> resultMatrixRangeByElement = rangeByElementMat * rangeByElementMat;
	matrix<typeMatrix, N, M> resultMatrixRangeByElementFirst = rangeByElementMat * rangeMat;
	matrix<typeMatrix, N, M> resultMatrixRangeByElementSecond = rangeMat * rangeByElementMat;

	if (resultMatrixRange != resultMatrixRangeByElement) {
		DEBUG_LINEOUT("Failed Range and Range Element Mismatch");
		
		DEBUG_LINEOUT("Range:");
		resultMatrixRange.PrintMatrix();

		DEBUG_LINEOUT("Range by Element:");
		resultMatrixRangeByElement.PrintMatrix();

		CB((false));
	}
	else if (resultMatrixRange != resultMatrixRangeByElementFirst) {
		DEBUG_LINEOUT("Failed Range and Range Element First Mismatch");

		DEBUG_LINEOUT("Range:");
		resultMatrixRange.PrintMatrix();

		DEBUG_LINEOUT("Range by Element First:");
		resultMatrixRangeByElementFirst.PrintMatrix();

		CB((false));
	}
	else if (resultMatrixRange != resultMatrixRangeByElementSecond) {
		DEBUG_LINEOUT("Failed Range and Range Element Second Mismatch");

		DEBUG_LINEOUT("Range:");
		resultMatrixRange.PrintMatrix();

		DEBUG_LINEOUT("Range by Element Second:");
		resultMatrixRangeByElementSecond.PrintMatrix();

		CB((false));
	}
	else if (resultMatrixRangeByElementFirst != resultMatrixRangeByElementSecond) {
		DEBUG_LINEOUT("Failed Range and Range Element Order Mismatch");

		DEBUG_LINEOUT("Range by Element First:");
		resultMatrixRangeByElementFirst.PrintMatrix();

		DEBUG_LINEOUT("Range by Element Second:");
		resultMatrixRangeByElementSecond.PrintMatrix();

		CB((false));
	}

	resultMatrixRange.PrintMatrix();

	// Success:
	DEBUG_LINEOUT("Matrix Multiply Test Success: %d x %d", N, M);
	return r;

Error:
	DEBUG_LINEOUT("Matrix Multiply Test Failed: %d x %d", N, M);
	return r;
}

// TODO: Add the N x M variants
RESULT MatrixTestSuite::TestMatrixMultiply() {
	RESULT r = R_PASS;

	int numIterations = 5;

	//CRM((TestMatrixMultiplication<float, 1, 1>()), "Test Matrix Multiply failed for float 1x1");
	CRM((TestMatrixMultiplication<float, 2, 2>()), "Test Matrix Multiply failed for float 2x2");
	CRM((TestMatrixMultiplication<float, 3, 3>()), "Test Matrix Multiply failed for float 3x3");
	CRM((TestMatrixMultiplication<float, 4, 4>()), "Test Matrix Multiply failed for float 4x4");

Error:
	return r;
}

template <typename typeMatrix, int N, int M>
RESULT TestMatrixDeterminants(int numIterations) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("TEST DETERMINANT MATRIX %d x %d", N, M);

	matrix<typeMatrix, N, M> rangeMat = matrix<typeMatrix, N, M>::MakeRange();
	matrix<typeMatrix, N, M> randomMat = matrix<typeMatrix, N, M>::MakeRandom();
	typeMatrix detValue = determinant(rangeMat);

	if (N > 2) {
		CBM((detValue == 0), "Co-dependent matrix should have a determinant of zero but %f found", detValue);
	}

	detValue = determinant(randomMat);

	DEBUG_LINEOUT("Matrix Determinant Found: %f", detValue);
	randomMat.PrintMatrix();

	// TODO: Test using inverse 

Error:
	return r;
}

RESULT MatrixTestSuite::TestMatrixDeterminant() {
	RESULT r = R_PASS;

	int numIterations = 5;

	CRM((TestMatrixDeterminants<float, 4, 4>(numIterations)), "Test Matrix Determinant failed for float 4x4");
	CRM((TestMatrixDeterminants<float, 3, 3>(numIterations)), "Test Matrix Determinant failed for float 3x3");
	CRM((TestMatrixDeterminants<float, 2, 2>(numIterations)), "Test Matrix Determinant failed for float 2x2");

Error:
	return r;
}

template <typename typeMatrix, int N, int M>
RESULT TestMatrixMinors() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("TEST DETERMINANT MATRIX %d x %d", N, M);

	matrix<typeMatrix, N, M> rangeMat = matrix<typeMatrix, N, M>::MakeRange();
	rangeMat.PrintMatrix();

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			DEBUG_LINEOUT("Minor Matrix %d %d", i, j);

			auto minorMat = rangeMat.minormatrix(i, j);
			minorMat.PrintMatrix();

			DEBUG_LINEOUT("Matrix minor value: %f with cofactor %f", rangeMat.minor(i, j), rangeMat.cofactor(i, j));
		}
	}

//Error:
	return r;
}

RESULT MatrixTestSuite::TestMatrixMinor() {
	RESULT r = R_PASS;

	CRM((TestMatrixMinors<float, 3, 3>()), "Test Matrix Minor failed for float 3x3");
	CRM((TestMatrixMinors<float, 4, 4>()), "Test Matrix Minor failed for float 4x4");

Error:
	return r;
}

RESULT MatrixTestSuite::TestTranslateMatrix() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("TEST TRANSLATION MATRIX");

	matrix<translate_precision, 4, 4> matIdentity = matrix<translate_precision, 4, 4>::MakeIdentity(1.0f);

	TranslationMatrix matTranslationStart = TranslationMatrix(0.0f, 0.0f, 0.0f);
	TranslationMatrix matTranslationIncrement = TranslationMatrix(1.0f, 2.0f, 3.0f);

	auto matTranslationResult = matTranslationStart * matTranslationIncrement;

	matTranslationResult.PrintMatrix();

	// TODO: Test outcome

//Error:
	return r;
}

RESULT MatrixTestSuite::TestRotateMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED;
}

RESULT MatrixTestSuite::TestScaleMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED;
}

RESULT MatrixTestSuite::TestViewMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED;
}

RESULT MatrixTestSuite::TestProjectionMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED;
}