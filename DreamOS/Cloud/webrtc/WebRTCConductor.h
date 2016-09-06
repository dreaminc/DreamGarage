#ifndef WEBRTC_CONDUCTOR_H_
#define WEBRTC_CONDUCTOR_H_

#include "RESULT/EHM.h"
#include "Cloud/CloudImp.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCConductor.h
// The WebRTC Conductor which conducts the peer connection and generally handles the WebRTC 
// connections

#include <deque>
#include <map>
#include <set>
#include <string>

#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"

#define DTLS_ON  true
#define DTLS_OFF false

class WebRTCImp;
class WebRTCPeerConnection;

class WebRTCConductor : public WebRTCPeerConnection::WebRTCPeerConnectionObserver {

public:
	friend class WebRTCImp;

public:
	WebRTCConductor(WebRTCImp *pParentWebRTCImp);
	~WebRTCConductor();
	
	RESULT Initialize();

public:
	// WebRTCPeerConnectionObserver Interface
	virtual RESULT OnWebRTCConnectionStable() override;
	virtual RESULT OnWebRTCConnectionClosed() override;
	virtual RESULT OnSDPOfferSuccess() override;		// TODO: Consolidate with below
	virtual RESULT OnSDPAnswerSuccess() override;	// TODO: Consolidate with below
	virtual RESULT OnSDPSuccess(bool fOffer) override;
	virtual RESULT OnSDPFailure(bool fOffer) override;
	virtual RESULT OnICECandidatesGatheringDone() override;
	virtual RESULT OnDataChannelStringMessage(const std::string& strDataChannelMessage) override;
	virtual RESULT OnDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) override;

public:
	RESULT ClearPeerConnections();
	RESULT AddNewPeerConnection(long peerConnectionID);
	std::shared_ptr<WebRTCPeerConnection> GetPeerConnection(long peerConnectionID);
	bool FindPeerConnectionByID(long peerConnectionID);
	bool FindPeerConnectionByUserID(long peerUserID);

	RESULT SendDataChannelStringMessage(long peerConnectionID, std::string& strMessage);
	RESULT SendDataChannelMessage(long peerConnectionID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n);

private:
	WebRTCImp *m_pParentWebRTCImp;

	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pWebRTCPeerConnectionFactory;
	std::vector<std::shared_ptr<WebRTCPeerConnection>> m_webRTCPeerConnections;
};

#endif	// ! WEBRTC_CONDUCTOR_H_