#ifndef DREAM_VIDEO_STREAM_SUBSCRIBER_H_
#define DREAM_VIDEO_STREAM_SUBSCRIBER_H_

#include "core/ehm/EHM.h"

// TODO: Is this really supposed to be here?
// Dream OS
// os/DreamVideoStreamSubscriber.h

// This is a subscriber for the OnVideoFrame event in DreamOS

class PeerConnection;

class DreamVideoStreamSubscriber {
public:
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
};

#endif // ! DREAM_VIDEO_STREAM_SUBSCRIBER_H_