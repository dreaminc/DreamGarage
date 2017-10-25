#ifndef WEBRTC_TEST_SUITE_H_
#define WEBRTC_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;
class CloudController;
class EnvironmentAsset;

class DreamContentView;

class WebRTCTestSuite : public TestSuite
{
public:
	WebRTCTestSuite(DreamOS *pDreamOS);
	~WebRTCTestSuite();

	virtual RESULT AddTests() override;

public:

	// Tests
	RESULT AddTestWebRTCVideoStream();

	// Apps
	RESULT AddTestChromeMultiBrowser();

private:
	CloudController *GetCloudController();
	RESULT SetupSkyboxPipeline(std::string strRenderShaderName);

private:
	DreamOS *m_pDreamOS;
	//CloudController *m_pCloudController = nullptr;
};

#endif // ! WEBRTC_TEST_SUITE_H_
