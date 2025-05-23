#ifndef WEBRTC_TEST_SUITE_H_
#define WEBRTC_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream WebRTC Test Suite
// dos/src/test/suites/WebRTCTestSuite.h

#include <functional>
#include <memory>

#include "DreamTestSuite.h"

#include "modules/InteractionEngine/InteractionEngine.h"

class DreamOS;
class CloudController;
class EnvironmentAsset;

class DreamContentViewApp;

class sphere;
class quad;
class composite;

class WebRTCTestSuite : public DreamTestSuite
{
public:
	WebRTCTestSuite(DreamOS *pDreamOS);
	~WebRTCTestSuite();

	virtual RESULT AddTests() override;

	virtual RESULT SetupPipeline(std::string strRenderShaderName) override;
	virtual RESULT SetupTestSuite() override;

public:

	// WebRTC Testing
	RESULT AddTestWebRTCMultiPeer();
	
	// Tests
	
	// Capabilities

	RESULT AddTestWebRTCVideoStream();
	RESULT AddTestWebRTCAudio();
	RESULT AddTestWebRTCVCamAudioRelay();

	// Apps
	RESULT AddTestChromeMultiBrowser();
	
	RESULT HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent);

private:
	CloudController *GetCloudController();

private:
	
	//CloudController *m_pCloudController = nullptr;
	//*
	std::shared_ptr<sphere> m_pTestSphereRelative = nullptr;
	sphere *m_pTestSphereAbsolute = nullptr;
	std::shared_ptr<quad> m_pTestQuad = nullptr;
	std::shared_ptr<composite> m_pPointerCursor = nullptr;
	//*/
};

#endif // ! WEBRTC_TEST_SUITE_H_
