#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

// DREAM OS
// DreamOS/Test/TestSuite.h
// A test suite is, like it sounds, a suite of tests designed to exercise a given module of the engine.

#include "RESULT/EHM.h"
#include "TestObject.h"

#include <vector>

class TestSuite {
private:
	std::vector<std::shared_ptr<TestObject>> m_tests;

public:
	TestSuite();
	~TestSuite();

	RESULT ClearTests();
	RESULT RunTests();
	RESULT AddTest(std::function<RESULT()> fnTestFunction);

};

#endif // ! TEST_SUITE_H_
