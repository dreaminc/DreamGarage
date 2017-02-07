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
public:
	TestObject(std::function<RESULT()> fnTestFunction);
	TestObject(std::function<RESULT(void*)> fnTestFunction, void *pContext = nullptr);
	~TestObject();

	RESULT RunTest(void* pContext = nullptr);
	RESULT ResetTest();
	bool DidTestPass();

private:
	std::function<RESULT(void*)> m_fnTestFunction;
	RESULT m_returnResult;

	void* m_pContext;
};

#endif // ! TEST_OBJECT_H_
