#include "WebRTCCustomVideoCapturer.h"

#include <iostream>
//#include <pthread.h>
//#include <sys/time.h>

#include "webrtc/common_video/libyuv/include/webrtc_libyuv.h"
#include "webrtc/media/base/videocapturer.h"
#include "webrtc/media/base/videoframe.h"

#include <memory>

WebRTCCustomVideoCapturer::WebRTCCustomVideoCapturer(int deviceId) {
	// empty
}

WebRTCCustomVideoCapturer::~WebRTCCustomVideoCapturer() {
	// empty
}

cricket::CaptureState WebRTCCustomVideoCapturer::Start(const cricket::VideoFormat& videoCaptureFormat) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("WebRTCCustomVideoCapturer Start");

	CBM((capture_state() != cricket::CS_RUNNING), "Start called when it's already started.");

	// get a reference to the current thread so we can send the frames to webrtc
	// on the same thread on which the capture was started
	m_startThread = rtc::Thread::Current();

	// start frame grabbing thread
	// pthread_create(&g_pthread, NULL, grabCapture, (void*)this);

	SetCaptureFormat(&videoCaptureFormat);

	return cricket::CS_RUNNING;

Error:
	return capture_state();
}

void WebRTCCustomVideoCapturer::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCCustomVideoCapturer Stop");

	CBM((capture_state() != cricket::CS_STOPPED), "WebRTCCustomVideoCapturer already stopped");
	
	// TODO: 
	m_startThread = nullptr;

	SetCaptureFormat(nullptr);
	SetCaptureState(cricket::CS_STOPPED);

Error:
	return;
}

void* WebRTCCustomVideoCapturer::grabCapture(void* pContext) {
	RESULT r = R_PASS;


	WebRTCCustomVideoCapturer *pWebRTCCustomVideoCapturer = (WebRTCCustomVideoCapturer*)pContext;
	CN(pWebRTCCustomVideoCapturer);



	while (pWebRTCCustomVideoCapturer->IsRunning()) {
		
		// Create and populate a frame

		webrtc::VideoFrame webRTCVideoFrame;

		if (0 != webRTCVideoFrame.CreateEmptyFrame(bgra.cols, bgra.rows, bgra.cols, (bgra.cols + 1) / 2, (bgra.cols + 1) / 2)) {
			std::cout << "Failed to create empty frame" << std::endl;
		}

		//convert the frame to I420, which is the supported format for webrtc transport
		if (0 != webrtc::ConvertToI420(webrtc::kBGRA, bgra.ptr(), 0, 0, bgra.cols, bgra.rows, 0, webrtc::kVideoRotation_0, &webRTCVideoFrame)) {
			std::cout << "Failed to convert frame to i420" << std::endl;
		}

		std::vector<uint8_t> captureBufferVector;
		size_t length = webrtc::CalcBufferSize(webrtc::kI420, webRTCVideoFrame.width(), webRTCVideoFrame.height());
		captureBufferVector.resize(length);
		webrtc::ExtractBuffer(webRTCVideoFrame, length, &captureBufferVector[0]);
		
		std::shared_ptr<cricket::CapturedFrame> capturedVideoFrame(new cricket::CapturedFrame(webRTCVideoFrame, &captureBufferVector[0], length));

		// forward the frame to the video capture start thread
		if (pWebRTCCustomVideoCapturer->m_startThread->IsCurrent()) {
			pWebRTCCustomVideoCapturer->SignalFrameCaptured(pWebRTCCustomVideoCapturer, capturedVideoFrame.get());
		}
		else {
			pWebRTCCustomVideoCapturer->m_startThread->Invoke<void>(
				rtc::Bind(&WebRTCCustomVideoCapturer::SignalFrameCapturedOnStartThread, 
						  pWebRTCCustomVideoCapturer, 
						  capturedVideoFrame.get()
				)
			);
		}
	}

	return nullptr;

Error:
	return nullptr;
}

void WebRTCCustomVideoCapturer::SignalFrameCapturedOnStartThread(const cricket::CapturedFrame* frame) {
	SignalFrameCaptured(this, frame);
}

bool WebRTCCustomVideoCapturer::IsRunning() {
	return capture_state() == cricket::CS_RUNNING;
}

bool WebRTCCustomVideoCapturer::GetPreferredFourccs(std::vector<uint32_t>* pFourccs) {
	RESULT r = R_PASS;

	CN(pFourccs);

	pFourccs->push_back(cricket::FOURCC_I420);

	return true;

Error:
	return false;
}

bool WebRTCCustomVideoCapturer::GetBestCaptureFormat(const cricket::VideoFormat& desiredVideoFormat, cricket::VideoFormat* pBestVideoFormat) {
	RESULT r = R_PASS;

	CN(pBestVideoFormat);

	// Use the desired format as the best format.

	pBestVideoFormat->width = desiredVideoFormat.width;
	pBestVideoFormat->height = desiredVideoFormat.height;
	pBestVideoFormat->fourcc = cricket::FOURCC_I420;
	pBestVideoFormat->interval = desiredVideoFormat.interval;

	return true;

Error:
	return false;
}

bool WebRTCCustomVideoCapturer::IsScreencast() const {
	return false;
}