#ifndef TEST_OBJECT_H_
#define TEST_OBJECT_H_

// DREAM OS
// DreamOS/Test/TestObject.h
// TestObject is the base class for a given test to be run
// These can be used to benchmark components, or test the system within the
// DreamOS TEST unit testing framework

#include "RESULT/EHM.h"
#include <functional>
#include <memory>
#include <chrono>

// TODO: Add test name
// TODO: Add test description

class TestObject {
public:
	friend class TestSuite;

public:
	enum class state {
		NOT_INITIALIZED,
		INITIALIZED,
		RUNNING,
		STOPPED,
		DONE,
		COMPLETE
	};

	struct TestDescriptor {
		std::function<RESULT(void*)> fnInitialize = nullptr;
		std::function<RESULT(void*)> fnUpdate = nullptr;
		std::function<RESULT(void*)> fnTest = nullptr;
		std::function<RESULT(void*)> fnReset = nullptr;

		std::function<RESULT()> fnTestNoContext = nullptr;

		std::string strTestName = "default";
		std::string strTestDescription = "default";
		double sDuration = 10.0f;
		int nRepeats = 1;
	};

public:
	TestObject(std::function<RESULT()> fnTestFunction, void *pContext = nullptr);
	TestObject(std::function<RESULT(void*)> fnTest, void *pContext = nullptr);

	TestObject(std::function<RESULT(void*)> fnInitialize, 
			   std::function<RESULT(void*)> fnTest, 
			   void *pContext = nullptr);
	
	TestObject(std::function<RESULT(void*)> fnInitialize, 
			   std::function<RESULT(void*)> fnUpdate, 
			   std::function<RESULT(void*)> fnTest, 
			   void *pContext = nullptr);

	TestObject(std::function<RESULT(void*)> fnInitialize,
			   std::function<RESULT(void*)> fnUpdate,
			   std::function<RESULT(void*)> fnTest,
			   std::function<RESULT(void*)> fnReset,
			   void *pContext = nullptr);

	TestObject(const TestObject::TestDescriptor &testDescriptor, void *pContext = nullptr);

	~TestObject();

	RESULT InitializeTest(void* pContext = nullptr);
	RESULT UpdateTest(void* pContext = nullptr);
	RESULT RunTest(void* pContext = nullptr);
	RESULT ResetTest(void* pContext = nullptr);

	RESULT StartTest();
	RESULT StopTest();
	RESULT ContinueTest();
	RESULT CompleteTest();
	TestObject::state GetTestState();

	int CurrentRepetition();
	int Repetitions();

	bool DidTestPass();

protected:
	RESULT SetTestName(std::string strName);
	RESULT SetParentTestSuite(TestSuite *pParentTestSuite);

public:
	std::string GetTestName();

	RESULT SetTestDescription(std::string strDescription);
	RESULT SetTestDuration(double sDuration);
	RESULT SetTestRepeats(int nRepeats);

private:
	std::string m_strTestName;
	std::string m_strDescription;
	double m_sDuration;
	int m_nRepeats;
	int m_nCurrentRepetition = 0;
	TestObject::state m_testState = TestObject::state::NOT_INITIALIZED;

	// Start times
	std::chrono::high_resolution_clock::time_point m_timeStartInitialize;
	std::chrono::high_resolution_clock::time_point m_timeStartUpdate;
	std::chrono::high_resolution_clock::time_point m_timeStartRunTest;
	std::chrono::high_resolution_clock::time_point m_timeStartTest;

	// Duration
	std::chrono::high_resolution_clock::duration m_timeDurationInitialize;
	std::chrono::high_resolution_clock::duration m_timeDurationUpdate;
	std::chrono::high_resolution_clock::duration m_timeDurationRunTest;
	std::chrono::high_resolution_clock::duration m_timeDurationTotal;

	std::function<RESULT(void*)> m_fnInitialize = nullptr;
	RESULT m_initializeResult = R_NOT_HANDLED;

	std::function<RESULT(void*)> m_fnUpdate = nullptr;
	RESULT m_updateResult = R_NOT_HANDLED;

	std::function<RESULT(void*)> m_fnTest = nullptr;
	RESULT m_testResult = R_NOT_HANDLED;

	std::function<RESULT(void*)> m_fnReset = nullptr;
	RESULT m_resetResult = R_NOT_HANDLED;

	TestSuite *m_pParentTestSuite = nullptr;
	void* m_pContext = nullptr;
};

#endif // ! TEST_OBJECT_H_
