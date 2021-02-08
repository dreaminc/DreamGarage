#ifndef DREAM_TEST_ANDROID_CLIENT_H_
#define DREAM_TEST_ANDROID_CLIENT_H_

#include "core/ehm/EHM.h"

#include "core/types/UID.h"
#include "core/types/valid.h"
#include "core/types/version.h"

#include "os/DreamOS.h"

// Notes: This is an initial bootstrap of android functionality 
// it's clear that over time sandbox, dreamos, and the client have become inseparabely linked 
// and so lets just get something to work a bit before cleaning up all of the arch since 
// there's a lot of work to be done there

class DreamTestAndroidClient :
	public DreamOS
{

public:
	DreamTestAndroidClient();
	virtual ~DreamTestAndroidClient();


public:
	virtual RESULT ConfigureSandbox() override;
	virtual RESULT LoadScene() override;
	virtual RESULT Update() override;
	virtual version GetDreamVersion() override;

public:

	// Cloud Controller Hooks
	virtual RESULT OnNewDreamPeer(DreamPeerApp* pDreamPeer) override;
	virtual RESULT OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) override;
	virtual RESULT OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage* pDreamMessage) override;

	virtual RESULT MakePipeline(CameraNode* pCamera, OGLProgram*& pRenderNode, OGLProgram*& pEndNode, Sandbox::PipelineType pipelineType) override;

	virtual RESULT OnNewSocketConnection(int seatPosition) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) override;
	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override;

private:

};

#endif // ! DREAM_TEST_ANDROID_CLIENT_H_