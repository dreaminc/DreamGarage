#ifndef PEER_CONNECTION_OBSERVER_H_
#define PEER_CONNECTION_OBSERVER_H_

class PeerConnectionObserver {
public:
	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) = 0;
	virtual RESULT OnNewSocketConnection(int seatPosition) = 0;
	virtual RESULT OnPeerConnectionClosed(PeerConnection* pPeerConnection) = 0;
	virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message* pDataMessage) = 0;
	virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) = 0;
	virtual RESULT OnAudioData(const std::string& strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	virtual RESULT OnVideoFrame(const std::string& strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t* pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
	virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) = 0;
	virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) = 0;
};

#endif // PEER_CONNECTION_OBSERVER_H_