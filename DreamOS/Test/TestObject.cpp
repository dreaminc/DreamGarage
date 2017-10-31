#include "TestObject.h"

TestObject::TestObject(std::function<RESULT()> fnTestFunction, void *pContext) :
	m_fnInitialize(nullptr),
	m_fnUpdate(nullptr),
	m_fnTest(nullptr),
	m_fnReset(nullptr),
	m_pContext(pContext)
{
	// Wrap it up
	m_fnTest = [=](void *pContext) {
		return fnTestFunction();
	};
}

TestObject::TestObject(std::function<RESULT(void*)> fnTest, void *pContext) :
	m_fnInitialize(nullptr),
	m_fnUpdate(nullptr),
	m_fnTest(fnTest),
	m_fnReset(nullptr),
	m_pContext(pContext)
{
	// empty
}

TestObject::TestObject(std::function<RESULT(void*)> fnInitialize, 
					   std::function<RESULT(void*)> fnTest, 
					   void *pContext) :
	m_fnInitialize(fnInitialize),
	m_fnUpdate(nullptr),
	m_fnTest(fnTest),
	m_fnReset(nullptr),
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
	m_fnReset(nullptr),
	m_pContext(pContext)
{
	// empty
}

TestObject::TestObject(std::function<RESULT(void*)> fnInitialize,
					   std::function<RESULT(void*)> fnUpdate,
					   std::function<RESULT(void*)> fnTest,
					   std::function<RESULT(void*)> fnReset,
					   void *pContext) :
	m_fnInitialize(fnInitialize),
	m_fnUpdate(fnUpdate),
	m_fnTest(fnTest),
	m_fnReset(fnReset),
	m_pContext(pContext)
{
	// empty
}

TestObject::~TestObject() {
	/* stub */
}

RESULT TestObject::RunTest(void* pContext) {
	RESULT r = R_PASS;

	if (m_fnTest == nullptr) {
		m_testState = TestObject::state::COMPLETE;
		return r;
	}

	// Allow for override 
	m_timeStartRunTest = std::chrono::high_resolution_clock::now();
	{
		if (m_pContext != nullptr)
			m_testResult = m_fnTest(m_pContext);
		else
			m_testResult = m_fnTest(pContext);
	}
	m_timeDurationRunTest = std::chrono::high_resolution_clock::now() - m_timeStartRunTest;

	CRM(m_testResult, "Test Failed");

	m_testState = TestObject::state::COMPLETE;
	m_nCurrentRepetition++;

Error:
	return r;
}

RESULT TestObject::InitializeTest(void* pContext) {
	RESULT r = R_PASS;

	if (m_fnInitialize == nullptr) {
		m_testState = TestObject::state::INITIALIZED;
		return r;
	}

	// Allow for override 
	m_timeStartInitialize = std::chrono::high_resolution_clock::now();
	{
		if (m_pContext != nullptr)
			m_initializeResult = m_fnInitialize(m_pContext);
		else
			m_initializeResult = m_fnInitialize(pContext);
	}
	m_timeDurationInitialize = std::chrono::high_resolution_clock::now() - m_timeStartInitialize;

	CRM(m_initializeResult, "Test Initialization Failed");

	m_testState = TestObject::state::INITIALIZED;

Error:
	return r;
}

RESULT TestObject::UpdateTest(void* pContext) {
	RESULT r = R_PASS;

	if (m_sDuration <= 0.0f) {
		CR(CompleteTest());
		return r;
	}

	m_timeStartUpdate = std::chrono::high_resolution_clock::now();
	
	if(m_fnUpdate != nullptr) {
		// Allow for override 
		if (m_pContext != nullptr)
			m_updateResult = m_fnUpdate(m_pContext);
		else
			m_updateResult = m_fnUpdate(pContext);
	}
	else {
		m_updateResult = R_SKIPPED;
	}

	m_timeDurationUpdate = std::chrono::high_resolution_clock::now() - m_timeStartUpdate;
	m_timeDurationTotal = std::chrono::high_resolution_clock::now() - m_timeStartTest;

	// This will set the test to done
	if (std::chrono::duration_cast<std::chrono::seconds>(m_timeDurationTotal).count() > m_sDuration) {
		CR(CompleteTest());
	}

	CRM(m_updateResult, "Test Update Failed");

Error:
	return r;
}

RESULT TestObject::StartTest() {
	m_testState = TestObject::state::RUNNING;
	m_timeStartTest = std::chrono::high_resolution_clock::now();
	return R_PASS;
}

RESULT TestObject::ContinueTest() {
	m_testState = TestObject::state::RUNNING;
	return R_PASS;
}

RESULT TestObject::CompleteTest() {
	m_testState = TestObject::state::DONE;
	return R_PASS;
}

RESULT TestObject::StopTest() {
	m_testState = TestObject::state::STOPPED;
	return R_PASS;
}

TestObject::state TestObject::GetTestState() {
	return m_testState;
}

int TestObject::CurrentRepetition() {
	return m_nCurrentRepetition;
}

int TestObject::Repetitions() {
	return m_nRepeats;
}

RESULT TestObject::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	CNR(m_fnReset, R_SKIPPED);

	// Allow for override 
	if (pContext == nullptr)
		m_resetResult = m_fnReset(m_pContext);
	else
		m_resetResult = m_fnReset(pContext);
	
	CRM(m_resetResult, "Test Reset Failed");

Error:
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