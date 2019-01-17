#include "TestSuite.h"

TestSuite::TestSuite(std::string strName) :
	m_strName(strName)
{
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

RESULT TestSuite::UpdateAndRunTests(void *pContext) {
	RESULT r = R_PASS;

	CBR((m_currentTest != m_tests.end()), R_COMPLETE);

	{
		auto pTest = (*m_currentTest);

		switch (pTest->GetTestState()) {
			case TestObject::state::NOT_INITIALIZED: {
				// Initialize the test
				CR(pTest->InitializeTest(pContext));
			} break;

			case TestObject::state::INITIALIZED: {
				// Kick off the test
				CR(pTest->StartTest());
			} break;

			case TestObject::state::RUNNING: {
				// Update the test
				CR(pTest->UpdateTest(pContext));
			} break;

			case TestObject::state::STOPPED: {
				// Do nothing for now
				// TODO: Support this
			} break;

			case TestObject::state::DONE: {
				// Run the final test
				CR(pTest->RunTest(pContext));
			} break;

			case TestObject::state::COMPLETE: {
				// Reset and load next test
				CR(pTest->ResetTest());

				// Repeat the number of repeats as needed - otherwise increment test
				if (pTest->CurrentRepetition() < pTest->Repetitions()) {
					CR(pTest->InitializeTest(pContext));
				}
				else {
					m_currentTest++;
				}
			} break;
		}
	}

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
	{
		auto pTest = (*m_currentTest);
		CR(pTest->ResetTest());
		m_currentTest++;
	}

Error:
	return r;
}

std::shared_ptr<TestObject> TestSuite::GetCurrentTest() {
	if (m_currentTest != m_tests.end())
		return (*m_currentTest);
	else
		return nullptr;
}

std::shared_ptr<TestObject> TestSuite::AddTest(const TestObject::Functions &fnStruct, void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<TestObject> pNewTest = std::make_shared<TestObject>(fnStruct, pContext);

	CNM(pNewTest, "Failed to allocate new test");

	m_tests.push_back(pNewTest);

	return pNewTest;

Error:
	return nullptr;
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT(void*)> fnInitialize,
	std::function<RESULT(void*)> fnUpdate,
	std::function<RESULT(void*)> fnTest,
	std::function<RESULT(void*)> fnReset,
	void *pContext)
{
	TestObject::Functions fnStruct = { nullptr };

	fnStruct.fnInitialize = fnInitialize;
	fnStruct.fnUpdate = fnUpdate;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = fnReset;
	fnStruct.fnTestNoContext = nullptr;

	return AddTest(fnStruct, pContext);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT(void*)> fnInitialize,
	std::function<RESULT(void*)> fnUpdate,
	std::function<RESULT(void*)> fnTest,
	void *pContext)
{
	TestObject::Functions fnStruct = { nullptr };

	fnStruct.fnInitialize = fnInitialize;
	fnStruct.fnUpdate = fnUpdate;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = nullptr;

	return AddTest(fnStruct, pContext);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT(void*)> fnInitialize, std::function<RESULT(void*)> fnTest, void *pContext) {
	TestObject::Functions fnStruct = { nullptr };

	fnStruct.fnInitialize = fnInitialize;
	fnStruct.fnUpdate = nullptr;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = nullptr;

	return AddTest(fnStruct, pContext);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT(void*)> fnTest, void *pContext) {
	TestObject::Functions fnStruct = { nullptr };

	fnStruct.fnInitialize = nullptr;
	fnStruct.fnUpdate = nullptr;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = nullptr;

	return AddTest(fnStruct, pContext);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::function<RESULT()> fnTestFunction, void *pContext) {
	TestObject::Functions fnStruct = { nullptr };

	fnStruct.fnInitialize = nullptr;
	fnStruct.fnUpdate = nullptr;
	fnStruct.fnTest = nullptr;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = fnTestFunction;

	return AddTest(fnStruct, pContext);
}

RESULT TestSuite::ClearTests() {
	m_tests.clear();
	m_currentTest = m_tests.begin();

	return R_PASS;
}

RESULT TestSuite::Initialize() {
	RESULT r = R_PASS;

	CR(ClearTests());

	CRM(SetupTestSuite(), "Failed to set up test suite %s", m_strName.c_str());

	CRM(AddTests(), "Failed to add tests");

	m_currentTest = m_tests.begin();

Error:
	return r;
}
