#include "TestObject.h"

TestObject::TestObject(std::function<RESULT()> fnTestFunction) :
	m_pContext(nullptr)
{
	// Wrap it up
	m_fnTest = [=](void *pContext) {
		return fnTestFunction();
	};
}

TestObject::TestObject(std::function<RESULT(void*)> fnTest, void *pContext) :
	m_fnTest(fnTest),
	m_pContext(pContext)
{
	// empty
}

TestObject::TestObject(std::function<RESULT(void*)> fnInitialize,
					   std::function<RESULT(void*)> fnUpdate,
					   std::function<RESULT(void*)> fnTest,
					   void *pContext) :
	m_fnInitialize(fnInitialize),
	m_fnUpdate(fnUpdate),
	m_fnTest(fnTest),
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
		m_testResult = m_fnTest(m_pContext);
	else 
		m_testResult = m_fnTest(pContext);

	CRM(m_testResult, "Test Failed");

Error:
	return r;
}

RESULT TestObject::InitializeTest(void* pContext) {
	RESULT r = R_PASS;

	// allow for override 
	if (pContext == nullptr)
		m_initializeResult = m_fnInitialize(m_pContext);
	else
		m_initializeResult = m_fnInitialize(pContext);

	CRM(m_initializeResult, "Test Initialization Failed");

Error:
	return r;
}

RESULT TestObject::UpdateTest(void* pContext) {
	RESULT r = R_PASS;

	// allow for override 
	if (pContext == nullptr)
		m_updateResult = m_fnUpdate(m_pContext);
	else
		m_updateResult = m_fnUpdate(pContext);

	CRM(m_updateResult, "Test Update Failed");

Error:
	return r;
}

RESULT TestObject::ResetTest() {
	m_testResult = R_RESET;
	m_testState = TestObject::state::NOT_INITIALIZED;

	return R_PASS;
}

bool TestObject::DidTestPass() {
	return (m_testResult == R_PASS);
}

RESULT TestObject::SetTestName(std::string strName) {
	m_strTestName = strName;
	return R_PASS;
}

RESULT TestObject::SetTestDescription(std::string strDescription) {
	m_strDescription = strDescription;
	return R_PASS;
}

RESULT TestObject::SetTestDuration(double sDuration) {
	m_sDuration = sDuration;
	return R_PASS;
}

RESULT TestObject::SetTestRepeats(int nRepeats) {
	m_nRepeats = nRepeats;
	return R_PASS;
}