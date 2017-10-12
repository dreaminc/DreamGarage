#ifndef WEB_RTC_CUSTOM_VIDEO_CAPTURER_H_
#define WEB_RTC_CUSTOM_VIDEO_CAPTURER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/webrtc/WebRTCCustomVideoCapturer.h
// Custom Video Capturer Class for Web RTC based video streaming

#include "webrtc/media/base/videocommon.h"
#include "webrtc/media/base/videocapturer.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"

#include "webrtc/base/thread.h"

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

private:
	//DISALLOW_COPY_AND_ASSIGN(CustomVideoCapturer);

	static void* grabCapture(void* pContext);

	// To call the SignalFrameCaptured call on the main thread
	void SignalFrameCapturedOnStartThread(const cricket::CapturedFrame* frame);

	// video capture thread
	rtc::Thread* m_startThread; 
};


class VideoCapturerFactoryCustom : public cricket::VideoDeviceCapturerFactory
{
public:
	VideoCapturerFactoryCustom() {}
	virtual ~VideoCapturerFactoryCustom() {}

	virtual cricket::VideoCapturer* Create(const cricket::Device& device) {

		// XXX: WebRTC uses device name to instantiate the capture, which is always 0.
		return new WebRTCCustomVideoCapturer(atoi(device.id.c_str()));
	}
};

#endif	// ! WEB_RTC_CUSTOM_VIDEO_CAPTURER_H_