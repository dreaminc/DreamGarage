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

// TODO: Add test name
// TODO: Add test description

class TestObject {
	enum class state {
		NOT_INITIALIZED,
		INITIALIZED,
		RUNNING,
		STOPPED,
		DONE
	};

public:
	TestObject(std::function<RESULT()> fnTestFunction);
	TestObject(std::function<RESULT(void*)> fnTest, void *pContext = nullptr);
	
	TestObject(std::function<RESULT(void*)> fnInitialize, 
			   std::function<RESULT(void*)> fnUpdate, 
			   std::function<RESULT(void*)> fnTest, 
			   void *pContext = nullptr);

	~TestObject();

	RESULT InitializeTest(void* pContext);
	RESULT UpdateTest(void* pContext);
	RESULT RunTest(void* pContext = nullptr);

	RESULT ResetTest();
	bool DidTestPass();

public:
	RESULT SetTestName(std::string strName);
	RESULT SetTestDescription(std::string strDescription);
	RESULT SetTestDuration(double sDuration);
	RESULT SetTestRepeats(int nRepeats);

private:
	std::string m_strTestName;
	std::string m_strDescription;
	double m_sDuration;
	int m_nRepeats;
	TestObject::state m_testState = TestObject::state::NOT_INITIALIZED;

	std::function<RESULT(void*)> m_fnTest;
	RESULT m_testResult;

	std::function<RESULT(void*)> m_fnInitialize;
	RESULT m_initializeResult;

	std::function<RESULT(void*)> m_fnUpdate;
	RESULT m_updateResult;

	void* m_pContext;
};

#endif // ! TEST_OBJECT_H_
