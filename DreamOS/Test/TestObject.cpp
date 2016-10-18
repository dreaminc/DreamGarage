#include "TestObject.h"

TestObject::TestObject(std::function<RESULT()> fnTestFunction) :
	m_fnTestFunction(fnTestFunction)
{
	// empty
}

TestObject::~TestObject() {
	/* stub */
}

RESULT TestObject::RunTest(void) {
	RESULT r = R_PASS;

	m_returnResult = m_fnTestFunction();
	CRM(m_returnResult, "Test Failed");

Error:
	return r;
}

RESULT TestObject::ResetTest() {
	m_returnResult = R_RESET;
	return R_PASS;
}

bool TestObject::DidTestPass() {
	return (m_returnResult == R_PASS);
}