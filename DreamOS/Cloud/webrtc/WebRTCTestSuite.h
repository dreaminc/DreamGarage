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

<<<<<<< 77790ebae92d0bcc7ce30bff84c7e5b826f0bff0
	// WebRTC Testing
	RESULT AddTestWebRTCMultiPeer();
	
	// Tests
	
	// Capabilities

	RESULT AddTestWebRTCVideoStream();
	RESULT AddTestWebRTCAudio();

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
