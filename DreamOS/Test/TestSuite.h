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
	TestSuite(std::string strName);
	~TestSuite() = default;

	RESULT Initialize();

	RESULT ClearTests();
	RESULT ResetTests();
	RESULT EndCurrentTest();
	RESULT RunTests();
	RESULT RunTest();
	RESULT NextTest();

	RESULT UpdateAndRunTests(void *pContext);

	std::shared_ptr<TestObject> AddTest(std::function<RESULT()> fnTestFunction, void *pContext = nullptr);
	std::shared_ptr<TestObject> AddTest(std::function<RESULT(void*)> fnTest, void *pContext = nullptr);

	std::shared_ptr<TestObject> AddTest(std::function<RESULT(void*)> fnInitialize,
									    std::function<RESULT(void*)> fnTest,
										void *pContext = nullptr);

	std::shared_ptr<TestObject> AddTest(std::function<RESULT(void*)> fnInitialize,
										std::function<RESULT(void*)> fnUpdate,
										std::function<RESULT(void*)> fnTest,
										void *pContext = nullptr);

	std::shared_ptr<TestObject> AddTest(std::function<RESULT(void*)> fnInitialize,
										std::function<RESULT(void*)> fnUpdate,
										std::function<RESULT(void*)> fnTest,
										std::function<RESULT(void*)> fnReset,
										void *pContext = nullptr);

	std::shared_ptr<TestObject> AddTest(const TestObject::Functions &fnStruct, void *pContext = nullptr);

	virtual RESULT SetupTestSuite() { return R_NOT_IMPLEMENTED; }
	virtual RESULT AddTests() = 0;
	
	std::shared_ptr<TestObject> GetCurrentTest();

	std::string GetName() {
		return m_strName;
	}

private:
	std::vector<std::shared_ptr<TestObject>> m_tests;
	std::vector<std::shared_ptr<TestObject>>::iterator m_currentTest;

	std::string m_strName;

	bool m_fTestSuiteSetup = false;

};

#endif // ! TEST_SUITE_H_
