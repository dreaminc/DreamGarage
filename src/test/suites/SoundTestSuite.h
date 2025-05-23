#ifndef SOUND_TEST_SUITE_H_
#define SOUND_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Sound Test Suite
// DreamOS/Sound/SoundTestSuite.h

#include <functional>
#include <memory>

#include "DreamTestSuite.h"

class DreamOS;

class SoundTestSuite : public DreamTestSuite
{
public:
	SoundTestSuite(DreamOS *pDreamOS);
	~SoundTestSuite() = default;

	virtual RESULT AddTests() override;

	virtual RESULT SetupTestSuite() override;

public:

	// Sound System Module Test
	// 12.18.18 <depr>: This has been moved to DreamOS
	// RESULT AddTestSoundSystemModule();

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
};

#endif // ! SOUND_TEST_SUITE_H_