#ifndef TEST_SUITE_FACTORY_H_
#define TEST_SUITE_FACTORY_H_

// DREAM OS
// DreamOS/Test/TestSuiteFactory.h
// Factory for creating test suites

#include "TestSuite.h"

class TestSuiteFactory {
public:
	enum class TEST_SUITE_TYPE : std::uint32_t {
		PHYSICS,
		INTERACTION,
		UI,
		INVALID
	} ;

public:
	static std::shared_ptr<TestSuite> Make(TEST_SUITE_TYPE type, void *pContext = nullptr);
};

#endif // ! TEST_SUITE_FACTORY_H_