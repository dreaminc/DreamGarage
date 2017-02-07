#include "TestSuite.h"

TestSuite::TestSuite() {
	ClearTests();
}

TestSuite::~TestSuite() {
	// Empty
}

// TODO: this should be added when this is a stand alone module
RESULT TestSuite::EndCurrentTest() {
	return R_NOT_IMPLEMENTED;
}

RESULT TestSuite::ResetTests() {
	RESULT r = R_PASS;

	CR(EndCurrentTest());

	m_currentTest = m_tests.begin();

Error:
	return r;
}

// This will run tests per the update loop, for the given duration 
// zero duration indicates no duration

RESULT TestSuite::UpdateAndRunTests() {
	RESULT r = R_PASS;

	CB((m_currentTest != m_tests.end()));

Error:
	return r;
}

// This will run all tests consecutively 
RESULT TestSuite::RunTests() {
	RESULT r = R_PASS;

	for (m_currentTest = m_tests.begin(); m_currentTest < m_tests.end(); m_currentTest++) {
		CR((*m_currentTest)->RunTest());
	}

Error:
	return r;
}

RESULT TestSuite::RunTest() {
	RESULT r = R_PASS;

	CB((m_currentTest != m_tests.end()));
	CR((*m_currentTest)->RunTest());

Error:
	return r;
}

RESULT TestSuite::NextTest() {
	RESULT r = R_PASS;

	CB((m_currentTest != m_tests.end()));
	m_currentTest++;

Error:
	return r;
}

std::shared_ptr<TestObject> TestSuite::GetCurrenTest() {
	if (m_currentTest != m_tests.end())
		return (*m_currentTest);
	else
		return nullptr;
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT(void*)> fnInitialize, std::function<RESULT(void*)> fnUpdate, std::function<RESULT(void*)> fnTest, void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<TestObject> pNewTest = std::make_shared<TestObject>(fnInitialize, fnUpdate, fnTest, pContext);
	CNM(pNewTest, "Failed to allocate new test");
	m_tests.push_back(pNewTest);
	
	return pNewTest;

Error:
	return nullptr;
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT(void*)> fnTest, void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<TestObject> pNewTest = std::make_shared<TestObject>(fnTest, pContext);
	CNM(pNewTest, "Failed to allocate new test");
	m_tests.push_back(pNewTest);

	return pNewTest;

Error:
	return nullptr;
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT()> fnTestFunction) {
	RESULT r = R_PASS;

	std::shared_ptr<TestObject> pNewTest = std::make_shared<TestObject>(fnTestFunction);
	CNM(pNewTest, "Failed to allocate new test");

	m_tests.push_back(pNewTest);

	return pNewTest;

Error:
	return nullptr;
}

RESULT TestSuite::ClearTests() {
	m_tests.clear();
	m_currentTest = m_tests.begin();

	return R_PASS;
}

RESULT TestSuite::Initialize() {
	RESULT r = R_PASS;

	CR(ClearTests());
	CRM(AddTests(), "Failed to add tests");

	m_currentTest = m_tests.begin();

Error:
	return r;
}