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
	~TestObject();

	RESULT RunTest();
	RESULT ResetTest();
	bool DidTestPass();

private:
	std::function<RESULT()> m_fnTestFunction;
	RESULT m_returnResult;
};

#endif // ! TEST_OBJECT_H_
