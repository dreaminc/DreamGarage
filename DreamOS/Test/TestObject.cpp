#include "TestObject.h"

#include "TestSuite.h"

TestObject::TestObject(const TestObject::TestDescriptor &testDescriptor) :
	m_fnInitialize(testDescriptor.fnInitialize),
	m_fnUpdate(testDescriptor.fnUpdate),
	m_fnTest(testDescriptor.fnTest),
	m_fnReset(testDescriptor.fnReset),
	m_strTestName(testDescriptor.strTestName),
	m_strDescription(testDescriptor.strTestDescription),
	m_nRepeats(testDescriptor.nRepeats),
	m_sDuration(testDescriptor.sDuration),
	m_pContext(testDescriptor.pContext)
{
	if (testDescriptor.fnTestNoContext != nullptr) {
		// Wrap it up
		m_fnTest = [=](void *pContext) {
			return testDescriptor.fnTestNoContext();
		};
	}
}

TestObject::~TestObject() {
	/* stub */
}

RESULT TestObject::SetParentTestSuite(TestSuite *pParentTestSuite) {
	RESULT r = R_PASS;

	CNM(pParentTestSuite, "Cannot set nullptr parent test suite");
	CBM((m_pParentTestSuite == nullptr), "Parent TestSuite already set");

	m_pParentTestSuite = pParentTestSuite;

Error:
	return r;
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
		if (m_fnTest != nullptr) {
			if (m_pContext != nullptr)
				m_testResult = m_fnTest(m_pContext);
			else
				m_testResult = m_fnTest(pContext);
		}
		else {
			if (m_pContext != nullptr)
				m_testResult = m_pParentTestSuite->DefaultEvaluateProcess(m_pContext);
			else
				m_testResult = m_pParentTestSuite->DefaultEvaluateProcess(pContext);
		}
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
		if (m_fnInitialize != nullptr) {
			if (m_pContext != nullptr)
				m_initializeResult = m_fnInitialize(m_pContext);
			else
				m_initializeResult = m_fnInitialize(pContext);
		}
		else {
			if (m_pContext != nullptr)
				m_initializeResult = m_pParentTestSuite->DefaultInitializeProcess(m_pContext);
			else
				m_initializeResult = m_pParentTestSuite->DefaultInitializeProcess(pContext);
		}
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
	
	if (m_fnUpdate != nullptr) {
		if (m_pContext != nullptr)
			m_updateResult = m_fnUpdate(m_pContext);
		else
			m_updateResult = m_fnUpdate(pContext);
	}
	else {
		if (m_pContext != nullptr)
			m_updateResult = m_pParentTestSuite->DefaultUpdateProcess(m_pContext);
		else
			m_updateResult = m_pParentTestSuite->DefaultUpdateProcess(pContext);
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

	if (m_fnReset != nullptr) {
		if (pContext == nullptr)
			m_resetResult = m_fnReset(m_pContext);
		else
			m_resetResult = m_fnReset(pContext);
	}
	else {
		if (pContext == nullptr)
			m_resetResult = m_pParentTestSuite->DefaultResetProcess(m_pContext);
		else
			m_resetResult = m_pParentTestSuite->DefaultResetProcess(pContext);
	}
	
	CRM(m_resetResult, "Test Reset Failed");

Error:
	return R_PASS;
}

bool TestObject::DidTestPass() {
	return (m_testResult == R_PASS);
}

RESULT TestObject::SetTestName(std::string strName) {
	RESULT r = R_PASS;

	CBM((strName.find_first_of(" \t\n\v\f\r") == std::string::npos), "%s test name cannot have whitespace", strName.c_str());

	m_strTestName = strName;

	// Force lower case
	std::transform(m_strTestName.begin(), m_strTestName.end(), m_strTestName.begin(), ::tolower);

Error:
	return r;
}

std::string TestObject::GetTestName() {
	return m_strTestName;
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