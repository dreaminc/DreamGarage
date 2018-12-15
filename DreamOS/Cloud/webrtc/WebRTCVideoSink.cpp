#include "WebRTCVideoSink.h"

#include "api/video/video_frame.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"

WebRTCVideoSink::WebRTCVideoSink(const std::string &strVideoTrackName, WebRTCVideoSink::observer *pParentObserver) :
	m_pParentObserver(pParentObserver),
	m_strVideoTrackName(strVideoTrackName)
{
	// empty
}

WebRTCVideoSink::~WebRTCVideoSink() {
	// empty
}


// rtc::VideoSinkInterface<cricket::VideoFrame>
void WebRTCVideoSink::OnFrame(const webrtc::VideoFrame& cricketVideoFrame) {
	RESULT r = R_PASS;

	int videoFrameWidth = cricketVideoFrame.width();
	int videoFrameHeight = cricketVideoFrame.height();

	uint8_t *pVideoFrameDataBuffer = (uint8_t*)malloc(sizeof(uint8_t) * videoFrameWidth * videoFrameHeight * 4);
	//size_t res = frame.ConvertToRgbBuffer(webrtc::VideoType::kARGB, dst_frame, frame.height()*frame.width() * 4, frame.width() * 4);

	const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& webRTCVideoFrameBuffer = cricketVideoFrame.video_frame_buffer();
	webrtc::VideoFrame webRTCVideoFrame(webRTCVideoFrameBuffer, 0, 0, webrtc::VideoRotation::kVideoRotation_0);

	/*
	VideoFrame(const rtc::scoped_refptr<webrtc::VideoFrameBuffer>& buffer,
	uint32_t timestamp,
	int64_t render_time_ms,
	VideoRotation rotation);
	*/

	int convertFromI420Result = webrtc::ConvertFromI420(webRTCVideoFrame, webrtc::VideoType::kARGB, 0, pVideoFrameDataBuffer);
	CBM((convertFromI420Result == 0), "YUV I420 conversion failed");

	/*
	{
	std::lock_guard<std::mutex> lock(g_UpdateTextureMutex);
	memcpy_s(m_pRecieveBuffer, m_ScreenWidth * m_ScreenHeight * 4, dst_frame, frame.height()*frame.width() * 4);
	g_updateTexture = true;
	}
	*/

	if (m_pParentObserver != nullptr) {
		m_pParentObserver->OnVideoFrame(m_strVideoTrackName, pVideoFrameDataBuffer, videoFrameWidth, videoFrameHeight);
	}

	return;

Error:
	// In a non-error state, this is left to the app to do
	if (pVideoFrameDataBuffer != nullptr) {
		delete pVideoFrameDataBuffer;
		pVideoFrameDataBuffer = nullptr;
	}

	return;
}