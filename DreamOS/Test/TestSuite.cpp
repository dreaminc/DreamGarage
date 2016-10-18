#include "TestSuite.h"

TestSuite::TestSuite() {
	ClearTests();
}

TestSuite::~TestSuite() {
	// Empty
}

RESULT TestSuite::RunTests() {
	RESULT r = R_PASS;

	for (auto &pTest : m_tests) {
		CR(pTest->RunTest());
	}

Error:
	return r;
}

RESULT TestSuite::AddTest(std::function<RESULT()> fnTestFunction) {
	RESULT r = R_PASS;

	std::shared_ptr<TestObject> pNewTest = std::make_shared<TestObject>(fnTestFunction);
	CNM(pNewTest, "Failed to allocate new test");
	m_tests.push_back(pNewTest);

Error:
	return r;
}

RESULT TestSuite::ClearTests() {
	m_tests.clear();
	return R_PASS;
}