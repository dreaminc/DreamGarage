#include "TestObject.h"

TestObject::TestObject(std::function<RESULT()> fnTestFunction) :
	m_pContext(nullptr)
{
	// Wrap it up
	m_fnTestFunction = [=](void *pContext) {
		return fnTestFunction();
	};
}

TestObject::TestObject(std::function<RESULT(void*)> fnTestFunction, void *pContext) :
	m_fnTestFunction(fnTestFunction),
	m_pContext(pContext)
{
	// empty
}

TestObject::~TestObject() {
	/* stub */
}

RESULT TestObject::RunTest(void* pContext) {
	RESULT r = R_PASS;

	// allow for override 
	if(pContext == nullptr)
		m_returnResult = m_fnTestFunction(m_pContext);
	else 
		m_returnResult = m_fnTestFunction(pContext);

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