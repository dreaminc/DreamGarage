#ifndef TEST_SUITE_H_
#define TEST_SUITE_H_

// DREAM OS
// DreamOS/Test/TestSuite.h
// A test suite is, like it sounds, a suite of tests designed to exercise a given module of the engine.

#include "RESULT/EHM.h"
#include "TestObject.h"

#include <vector>

class TestSuite {


public:
	TestSuite();
	~TestSuite();

	RESULT Initialize();

	RESULT ClearTests();
	RESULT ResetTests();
	RESULT EndCurrentTest();
	RESULT RunTests();
	RESULT RunTest();
	RESULT NextTest();
	RESULT AddTest(std::function<RESULT()> fnTestFunction);
	RESULT AddTest(std::function<RESULT(void*)> fnTestFunction, void *pContext = nullptr);
	
	virtual RESULT AddTests() = 0;

	std::shared_ptr<TestObject> GetCurrenTest();

private:
	std::vector<std::shared_ptr<TestObject>> m_tests;
	std::vector<std::shared_ptr<TestObject>>::iterator m_currentTest;
};

#endif // ! TEST_SUITE_H_
