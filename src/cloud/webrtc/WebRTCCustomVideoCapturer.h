#ifndef WEB_RTC_CUSTOM_VIDEO_CAPTURER_H_
#define WEB_RTC_CUSTOM_VIDEO_CAPTURER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCCustomVideoCapturer.h
// Custom Video Capturer Class for Web RTC based video streaming

#include "media/base/videocommon.h"

#include "media/base/videocapturer.h"

#include "media/engine/webrtcvideocapturerfactory.h"
#include "modules/video_capture/video_capture_factory.h"

#include "rtc_base/thread.h"

class WebRTCCustomVideoCapturer : public cricket::VideoCapturer {
public:
	WebRTCCustomVideoCapturer(int deviceId);
	virtual ~WebRTCCustomVideoCapturer();

	// cricket::VideoCapturer implementation.
	virtual cricket::CaptureState Start(const cricket::VideoFormat& videoCaptureFormat) override;
	virtual void Stop() override;
	virtual bool IsRunning() override;
	virtual bool GetPreferredFourccs(std::vector<uint32_t>* pFourccs) override;
	virtual bool GetBestCaptureFormat(const cricket::VideoFormat& desiredVideoFormat, cricket::VideoFormat* pBestVideoFormat) override;
	virtual bool IsScreencast() const override;

	RESULT SubmitNewFrameBuffer(uint8_t *pVideoBufferFrame, int pxWidth, int pxHeight, int channels = 4);

private:
	// To call the SignalFrameCaptured call on the main thread
	//void SignalFrameCapturedOnStartThread(const cricket::CapturedFrame* frame);

	// RTC Video capture thread
	rtc::Thread* m_startThread; 
};


class WebRTCCustomVideoCapturerFactory : public cricket::VideoDeviceCapturerFactory {
public:
	WebRTCCustomVideoCapturerFactory() {}
	virtual ~WebRTCCustomVideoCapturerFactory() {}

	// WebRTC uses device name to instantiate the capture, which is always 0.
	//virtual cricket::VideoCapturer* Create(const cricket::Device& device) {
	std::unique_ptr<cricket::VideoCapturer> Create(const cricket::Device& device) {
		return std::unique_ptr<cricket::VideoCapturer>(new WebRTCCustomVideoCapturer(atoi(device.id.c_str())));
	}
};

#endif	// ! WEB_RTC_CUSTOM_VIDEO_CAPTURER_H_