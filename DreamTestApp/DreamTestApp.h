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
	DreamTestApp() :
		m_pTestSuite(nullptr)
	{
		// empty
	}

	~DreamTestApp() {
		// empty
	}

	virtual RESULT DreamTestApp::ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT Update(void) override;

	// Cloud
	virtual RESULT OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) override;
	virtual RESULT OnNewDreamPeer(DreamPeerApp *pDreamPeer) override;
	virtual RESULT OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	//virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;

	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnNewSocketConnection(int seatPosition) override;

	virtual RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) override;

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(CollisionObjectEvent *oEvent) override;

private:
	user *m_pPeerUser;

private:
	std::shared_ptr<TestSuite> m_pTestSuite;
};


#endif	// DREAM_TEST_APP_H_