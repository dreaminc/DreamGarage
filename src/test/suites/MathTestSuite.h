#ifndef MATRIX_TEST_SUITE_H_
#define MATRIX_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Math Test Suite
// dos/src/test/suites/MathTestSuite.h

#include <functional>
#include <memory>

#include "test/TestSuite.h"

// TODO: Move to DreamTestSuite and port to DOS or find a way to run "headless"
class MathTestSuite : public TestSuite {
public:
	MathTestSuite();
	~MathTestSuite();

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
