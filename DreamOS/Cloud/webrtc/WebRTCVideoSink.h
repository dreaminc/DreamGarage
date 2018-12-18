#ifndef WEBRTC_VIDEO_SINK_H_
#define WEBRTC_VIDEO_SINK_H_


#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCVideoSink.h
// The WebRTC Video Sink Interface

#include "media/base/videocommon.h"
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "api/mediastreaminterface.h"

class WebRTCVideoSink : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
public:
	class observer {
	public:
		virtual RESULT OnVideoFrame(std::string strVideoTrackName, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) = 0;
	};

public:
	WebRTCVideoSink(const std::string &strVideoTrackName, WebRTCVideoSink::observer *pParentObserver, webrtc::VideoTrackSourceInterface* m_pVideoTrackSource);
	~WebRTCVideoSink();

	// rtc::VideoSinkInterface<cricket::VideoFrame>
	virtual void OnFrame(const webrtc::VideoFrame& cricketVideoFrame) override;

	RESULT Initialize();

public:
	static WebRTCVideoSink* MakeWebRTCVideoSink(const std::string &strVideoTrackName, WebRTCVideoSink::observer *pParentObserver, webrtc::VideoTrackSourceInterface* pVideoTrackSource);

private:
	std::string m_strVideoTrackName;
	WebRTCVideoSink::observer *m_pParentObserver = nullptr;

	webrtc::VideoTrackSourceInterface* m_pVideoTrackSource = nullptr;

};

#endif // !WEBRTC_VIDEO_SINK_H_ 