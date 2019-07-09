#ifndef DREAM_TEST_APP_H_
#define DREAM_TEST_APP_H_

#include "RESULT/EHM.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS.h"
#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

class TestSuite;

class DreamTestApp :
	public DreamOS,
	public Subscriber<SenseKeyboardEvent>
{
public:
	DreamTestApp();
	~DreamTestApp();

	virtual RESULT ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT Update(void) override;
	virtual version GetDreamVersion() override;

	virtual RESULT MakePipeline(CameraNode* pCamera, OGLProgram* &pRenderNode, OGLProgram* &pEndNode, SandboxApp::PipelineType pipelineType) override;

	// Cloud
	virtual RESULT OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) override;
	virtual RESULT OnNewDreamPeer(DreamPeerApp *pDreamPeer) override;
	virtual RESULT OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	//virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;

	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;

	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnNewSocketConnection(int seatPosition) override;

	virtual RESULT SaveCameraSettings(point ptPosition, quaternion qOrientation) override;

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(CollisionObjectEvent *oEvent) override;

private:
	user *m_pPeerUser;

private:
	RESULT RegisterTestSuite(std::shared_ptr<TestSuite> pTestSuite);
	RESULT RegisterTestSuites();
	RESULT SelectTest(std::string strTestSuiteName, std::string strTestName);

	std::shared_ptr<TestSuite> m_pCurrentTestSuite = nullptr;
	std::map<std::string, std::shared_ptr<TestSuite>> m_registeredTestSuites;
};


#endif	// DREAM_TEST_APP_H_
