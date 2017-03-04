#ifndef CLOUD_TEST_SUITE_H_
#define CLOUD_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class CloudTestSuite : public TestSuite {
public:
	CloudTestSuite(DreamOS *pDreamOS);
	~CloudTestSuite();

	virtual RESULT AddTests() override;

public:
	RESULT AddTestConnectLogin();

private:
	DreamOS *m_pDreamOS;
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
