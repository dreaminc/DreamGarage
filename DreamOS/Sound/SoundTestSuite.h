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

	// Sound System Module Test
	RESULT AddTestSoundSystemModule();

	// Enumerate
	RESULT AddTestEnumerateDevices();

	RESULT AddTestSoundClient();

	// Capture Sound
	RESULT AddTestCaptureSound();

	RESULT AddTestBrowserSoundRouting();

	// Play Sound
	RESULT AddTestPlaySound();
	RESULT AddTestSpatialSound();
	RESULT AddTestPlaySoundHRTF();

	// Record Sound
	//TODO: RESULT AddTestRecordSound();

private:
	RESULT SetupPipeline(std::string strRenderProgramName = "environment");

private:
	DreamOS *m_pDreamOS = nullptr;
};

#endif // ! SOUND_TEST_SUITE_H_