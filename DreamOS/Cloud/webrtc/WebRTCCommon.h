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

// User Stream
const char kUserStreamLabel[] = "user_stream_label";
const char kUserAudioLabel[] = "user_audio_label";
const char kUserDataLabel[] = "user_data_label";

// User Video Camera
const char kVCamStreamLabel[] = "vcam_stream_label";
const char kVCamVideoLabel[] = "vcam_video_label";
const char kVCamAudiolabel[] = "vcam_audio_label";

// Chrome Stream
const char kChromeStreamLabel[] = "chrome_stream_label";
const char kChromeVideoLabel[] = "chrome_video_label";
const char kChromeAudioLabel[] = "chrome_audio_label";

// Capture Devices
const char kChromeCaptureDevice[] = "chrome_capture";
const char kVCamCaptureDevice[] = "vcam_capture";


const uint16_t kDefaultServerPort = 8888;

#endif	// ! WEBRTCCOMMON_H_