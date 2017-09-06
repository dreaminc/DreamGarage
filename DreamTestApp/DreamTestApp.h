#ifndef DREAM_TEST_APP_H_
#define DREAM_TEST_APP_H_

#include "RESULT/EHM.h"
#include "DreamConsole/DreamConsole.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS.h"
#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

class TestSuite;

class DreamTestApp : public DreamOS, public Subscriber<SenseKeyboardEvent>, public Subscriber<CmdPromptEvent> {
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
	virtual RESULT OnNewDreamPeer(DreamPeer *pDreamPeer) override;
	virtual RESULT OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeer> pDreamPeer) override;
	virtual RESULT OnAudioData(PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(CollisionObjectEvent *oEvent) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:
	user *m_pPeerUser;

private:
	std::shared_ptr<TestSuite> m_pTestSuite;
};


#endif	// DREAM_TEST_APP_H_