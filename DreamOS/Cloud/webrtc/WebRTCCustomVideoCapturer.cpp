#include "WebRTCCustomVideoCapturer.h"

#include <iostream>
//#include <pthread.h>
//#include <sys/time.h>

#include "common_video/libyuv/include/webrtc_libyuv.h"
//
#include "media/base/videocapturer.h"
#include "api/video/video_frame.h"

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

#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "third_party/libyuv/include/libyuv.h"  // NOLINT
#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"

// TODO: This is all kinds of fucked now
RESULT WebRTCCustomVideoCapturer::SubmitNewFrameBuffer(uint8_t *pVideoBufferFrame, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	size_t frameSize = sizeof(uint8_t) * pxHeight * pxWidth * channels;
	
	auto pWebRTCI420Buffer = webrtc::I420Buffer::Create(pxWidth, pxHeight);
	CN(pWebRTCI420Buffer);
	
	// Convert RGBA to YUV420
	libyuv::ConvertToI420(src_frame, sample_size, dst_buffer->MutableDataY(), ...);
	
	// Send to transport
	OnFrame(webrtc::VideoFrame(pWebRTCI420Buffer, 0, rtc::TimeMillis(), webrtc::kVideoRotation_0), pxWidth, pxHeight);

	
	//webrtc::VideoFrame videoFrame;

	//webrtc::VideoFrameBuffer

	//webrtc::I420ABufferInterface::

	/*
	cricket::CapturedFrame capturedVideoframe;

	CB(IsRunning());

	capturedVideoframe.width = pxWidth;
	capturedVideoframe.height = pxHeight;
	capturedVideoframe.fourcc = cricket::FOURCC_ARGB;
	//capturedVideoframe.fourcc = cricket::FOURCC_I420;
	capturedVideoframe.data_size = (uint32_t)frameSize;

	capturedVideoframe.time_stamp = rtc::TimeNanos();
	//capturedVideoframe.time_stamp = rtc::SystemTimeNanos();

	// Set the buffer
	// Not clear who deletes the buffer
	capturedVideoframe.data = pVideoBufferFrame;

	// Signal the frame
	SignalFrameCaptured(this, &capturedVideoframe);

	// forward the frame to the video capture start thread
	if (m_startThread->IsCurrent()) {
		SignalFrameCaptured(this, &capturedVideoframe);
	}

	//else {
	//	m_startThread->Invoke<void>(
	//		rtc::Bind(&WebRTCCustomVideoCapturer::SignalFrameCapturedOnStartThread, this, &capturedVideoFrame)
	//		);
	//}

	/*
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

	*/

	CR(r);

Error:
	return r;
}

/*
void WebRTCCustomVideoCapturer::SignalFrameCapturedOnStartThread(const cricket::CapturedFrame* frame) {
	SignalFrameCaptured(this, frame);
}
*/

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