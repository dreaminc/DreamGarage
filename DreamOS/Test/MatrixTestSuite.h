#ifndef MATRIX_TEST_SUITE_H_
#define MATRIX_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitices/matrix/MatrixTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class MatrixTestSuite : public TestSuite {
public:
	MatrixTestSuite();
	~MatrixTestSuite();

	// TODO: Needed tests
	// element() vs operator () and operator []

	// Tests
	RESULT TestMatrixCreate();
	RESULT TestMatrixMultiply();
	RESULT TestMatrixDeterminant();
	RESULT TestMatrixMinor();
	RESULT TestMatrixInverse();

	RESULT TestTranslateMatrix();
	RESULT TestRotateMatrix();
	RESULT TestScaleMatrix();
	RESULT TestViewMatrix();
	RESULT TestProjectionMatrix();

	virtual RESULT AddTests() override;
};

#endif // ! TEST_OBJECT_H_
