#include "TestSuite.h"

TestSuite::TestSuite(std::string strName) :
	m_strName(strName)
{
	ClearTests();
}

// TODO: this should be added when this is a stand alone module
RESULT TestSuite::EndCurrentTest() {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT TestSuite::ResetTests() {
	RESULT r = R_PASS;

	CR(EndCurrentTest());

	m_currentTest = m_tests.begin();

Error:
	return r;
}


RESULT TestSuite::SelectTest(std::string strTestName) {
	RESULT r = R_PASS;

	for (auto &pTest : m_tests) {
		if (pTest->GetTestName() == strTestName) {
			m_pSingleTestToRun = pTest;
			break;
		}
	}

	CNM(m_pSingleTestToRun, "Test %s not found", strTestName.c_str());

Error:
	return r;
}

// This will run tests per the update loop, for the given duration
// zero duration indicates no duration

RESULT TestSuite::UpdateAndRunTests(void *pContext) {
	RESULT r = R_PASS;

	// This will run set up the first time
	// TODO: Would be better for set up / tear down to be run
	//       for each test
	if (m_fTestSuiteSetup == false) {
		CRM(SetupTestSuite(), "Failed to set up test suite %s", m_strName.c_str());
		m_fTestSuiteSetup = true;
	}

	if (m_pSingleTestToRun == nullptr) {
		CBR((m_currentTest != m_tests.end()), R_COMPLETE);
	}

	{
		std::shared_ptr<TestObject> pTest = nullptr;
		
		if (m_pSingleTestToRun != nullptr)
			pTest = m_pSingleTestToRun;
		else 
			pTest = (*m_currentTest);

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
					if (m_pSingleTestToRun == nullptr) {
						m_currentTest++;
					}
					else {
						// We're done!
						r = R_COMPLETE;
					}
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

// Shim to let tests be created with the descriptor struct
std::shared_ptr<TestObject> TestSuite::AddTest(std::string strTestName, TestObject::TestDescriptor testDescriptor) {
	testDescriptor.strTestName = strTestName;

	return AddTest(testDescriptor);
}

std::shared_ptr<TestObject> TestSuite::AddTest(const TestObject::TestDescriptor &testDescriptor) {
	RESULT r = R_PASS;

	std::shared_ptr<TestObject> pNewTest = nullptr;

	// Ensure no tests of same name exists
	for (auto &pTest : m_tests) {
		CBM((pTest->GetTestName() != testDescriptor.strTestName), 
			"%s test already exists, test names must be unique to suites", testDescriptor.strTestName.c_str());
	}

	pNewTest = std::make_shared<TestObject>(testDescriptor);
	CNM(pNewTest, "Failed to allocate new test");

	CRM(pNewTest->SetParentTestSuite(this), "Failed to set parent test suite for test %s", testDescriptor.strTestName.c_str());

	m_tests.push_back(pNewTest);

	return pNewTest;

Error:
	if (pNewTest != nullptr) {
		pNewTest = nullptr;
	}

	return nullptr;
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::string strTestName, std::function<RESULT(void*)> fnInitialize,
	std::function<RESULT(void*)> fnUpdate,
	std::function<RESULT(void*)> fnTest,
	std::function<RESULT(void*)> fnReset,
	void *pContext)
{
	TestObject::TestDescriptor fnStruct = { nullptr };

	fnStruct.fnInitialize = fnInitialize;
	fnStruct.fnUpdate = fnUpdate;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = fnReset;
	fnStruct.fnTestNoContext = nullptr;
	fnStruct.pContext = pContext;

	return AddTest(strTestName, fnStruct);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::string strTestName, std::function<RESULT(void*)> fnInitialize,
	std::function<RESULT(void*)> fnUpdate,
	std::function<RESULT(void*)> fnTest,
	void *pContext)
{
	TestObject::TestDescriptor fnStruct = { nullptr };

	fnStruct.fnInitialize = fnInitialize;
	fnStruct.fnUpdate = fnUpdate;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = nullptr;
	fnStruct.pContext = pContext;

	return AddTest(strTestName, fnStruct);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::string strTestName, std::function<RESULT(void*)> fnInitialize, std::function<RESULT(void*)> fnTest, void *pContext) {
	TestObject::TestDescriptor fnStruct = { nullptr };

	fnStruct.fnInitialize = fnInitialize;
	fnStruct.fnUpdate = nullptr;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = nullptr;
	fnStruct.pContext = pContext;

	return AddTest(strTestName, fnStruct);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::string strTestName, std::function<RESULT(void*)> fnTest, void *pContext) {
	TestObject::TestDescriptor fnStruct = { nullptr };

	fnStruct.fnInitialize = nullptr;
	fnStruct.fnUpdate = nullptr;
	fnStruct.fnTest = fnTest;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = nullptr;
	fnStruct.pContext = pContext;

	return AddTest(strTestName, fnStruct);
}

std::shared_ptr<TestObject> TestSuite::AddTest(std::string strTestName, std::function<RESULT()> fnTestFunction, void *pContext) {
	TestObject::TestDescriptor fnStruct = { nullptr };

	fnStruct.fnInitialize = nullptr;
	fnStruct.fnUpdate = nullptr;
	fnStruct.fnTest = nullptr;
	fnStruct.fnReset = nullptr;
	fnStruct.fnTestNoContext = fnTestFunction;
	fnStruct.pContext = pContext;

	return AddTest(strTestName, fnStruct);
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
