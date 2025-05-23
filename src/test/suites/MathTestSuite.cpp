#include "MathTestSuite.h"

#include "core/matrix/matrix.h"
#include "core/matrix/TranslationMatrix.h"

MathTestSuite::MathTestSuite() :
	TestSuite("matrix")
{
	// empty 
}

MathTestSuite::~MathTestSuite() {
	// empty
}

RESULT MathTestSuite::AddTests() {
	RESULT r = R_PASS;

	auto pNewTest = AddTest("creatematrix", (std::function<RESULT()>)std::bind(&MathTestSuite::TestMatrixCreate, this));
	pNewTest = AddTest("matrixmultiply", (std::function<RESULT()>)std::bind(&MathTestSuite::TestMatrixMultiply, this));
	pNewTest = AddTest("matrixdeterminant", (std::function<RESULT()>)std::bind(&MathTestSuite::TestMatrixDeterminant, this));
	pNewTest = AddTest("matrixminor", (std::function<RESULT()>)std::bind(&MathTestSuite::TestMatrixMinor, this));
	pNewTest = AddTest("matrixinverse", (std::function<RESULT()>)std::bind(&MathTestSuite::TestMatrixInverse, this));

	pNewTest = AddTest("translatematrix", (std::function<RESULT()>)std::bind(&MathTestSuite::TestTranslateMatrix, this));
	pNewTest = AddTest("rotatematrix", (std::function<RESULT()>)std::bind(&MathTestSuite::TestRotateMatrix, this));
	pNewTest = AddTest("viewmatrix", (std::function<RESULT()>)std::bind(&MathTestSuite::TestViewMatrix, this));
	pNewTest = AddTest("projectionmatrix", (std::function<RESULT()>)std::bind(&MathTestSuite::TestProjectionMatrix, this));
	pNewTest = AddTest("scalematrix", (std::function<RESULT()>)std::bind(&MathTestSuite::TestScaleMatrix, this));

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
RESULT MathTestSuite::TestMatrixCreate() {
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
RESULT MathTestSuite::TestMatrixMultiply() {
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

RESULT MathTestSuite::TestMatrixDeterminant() {
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

RESULT MathTestSuite::TestMatrixMinor() {
	RESULT r = R_PASS;

	CRM((TestMatrixMinors<float, 3, 3>()), "Test Matrix Minor failed for float 3x3");
	CRM((TestMatrixMinors<float, 4, 4>()), "Test Matrix Minor failed for float 4x4");

Error:
	return r;
}


template <typename typeMatrix, int N, int M>
RESULT TestMatrixInverses(int numIterations) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("TEST INVERSE MATRIX %d x %d", N, M);

	matrix<typeMatrix, N, M> matIdentity = matrix<typeMatrix, N, M>::MakeIdentity(1.0f);

	for (int i = 0; i < numIterations; i++) {
		matrix<typeMatrix, N, M> randomMat = matrix<typeMatrix, N, M>::MakeRandom();
		matrix<typeMatrix, N, M> randomMatInverse = inverse(randomMat);
		matrix<typeMatrix, N, M> multResult = randomMat * randomMatInverse;
		matrix<typeMatrix, N, M> multResultError = matIdentity - multResult;
		
		DEBUG_LINEOUT("Matrix %d x %d: ", N, M);
		randomMat.PrintMatrix();

		DEBUG_LINEOUT("Matrix Inverse %d x %d: ", N, M);
		randomMatInverse.PrintMatrix();

		DEBUG_LINEOUT(" M X M^-1 Result %d x %d: ", N, M);
		multResult.PrintMatrix();

		DEBUG_LINEOUT("RESULT Error Against Identity %d x %d: ", N, M);
		multResultError.PrintMatrix();

		DEBUG_LINEOUT("Max error: %f", multResultError.max());
		CBM((multResultError.max() < MAX_MATRIX_INVERSE_ERROR), "Max error %f exceeds set threshold %f", multResultError.max(), MAX_MATRIX_INVERSE_ERROR);
	}

Error:
	return r;
}

RESULT MathTestSuite::TestMatrixInverse() {
	RESULT r = R_PASS;

	int numIterations = 5;

	CRM((TestMatrixInverses<float, 2, 2>(numIterations)), "Test Matrix Inverse failed for float 3x3");
	CRM((TestMatrixInverses<float, 3, 3>(numIterations)), "Test Matrix Inverse failed for float 3x3");
	CRM((TestMatrixInverses<float, 4, 4>(numIterations)), "Test Matrix Inverse failed for float 4x4");

Error:
	return r;
}

RESULT MathTestSuite::TestTranslateMatrix() {
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

RESULT MathTestSuite::TestRotateMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT MathTestSuite::TestScaleMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT MathTestSuite::TestViewMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT MathTestSuite::TestProjectionMatrix() {
	// TODO:
	return R_NOT_IMPLEMENTED_WARNING;
}