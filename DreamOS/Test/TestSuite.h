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
	
	virtual RESULT AddTests() = 0;
	RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData);

	std::shared_ptr<TestObject> GetCurrentTest();

public:
	unsigned char* m_pDataBuffer;
	size_t m_pDataBuffer_n = 0;

private:
	std::vector<std::shared_ptr<TestObject>> m_tests;
	std::vector<std::shared_ptr<TestObject>>::iterator m_currentTest;
};

#endif // ! TEST_SUITE_H_
