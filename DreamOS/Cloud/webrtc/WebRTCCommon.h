#ifndef WEBRTCCOMMON_H_
#define WEBRTCCOMMON_H_	

// The above had to be changed due to an already existing WEBRTC_COMMON_H_ in the WebRTC code namespace

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCCommon.h
// The WebRTC Common defines

// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";
const char kSDPName[] = "sdp";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const char kDataLabel[] = "data_label";

const uint16_t kDefaultServerPort = 8888;

#endif	// ! WEBRTCCOMMON_H_