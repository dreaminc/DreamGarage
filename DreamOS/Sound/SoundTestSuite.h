#ifndef SOUND_TEST_SUITE_H_
#define SOUND_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class SoundTestSuite : public TestSuite
{
public:
	SoundTestSuite(DreamOS *pDreamOS);
	~SoundTestSuite();

	virtual RESULT AddTests() override;

public:

	// Enumerate
	RESULT AddTestEnumerateDevices();

private:
	RESULT SetupPipeline(std::string strRenderProgramName = "environment");

private:
	DreamOS *m_pDreamOS = nullptr;
};

#endif // ! SOUND_TEST_SUITE_H_