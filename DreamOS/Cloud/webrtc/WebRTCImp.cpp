#include "WebRTCImp.h"

WebRTCImp::WebRTCImp() :
	CloudImp()
{
	// empty
}

WebRTCImp::~WebRTCImp() {
	// empty
}

// CloudImp Interface
RESULT WebRTCImp::Initialize() {
	RESULT r = R_PASS;

	rtc::EnsureWinsockInit();
	rtc::ThreadManager::Instance()->SetCurrentThread(&m_Win32thread);
	rtc::InitializeSSL();

	//rtc::scoped_refptr<WebRTCConductor> conductor(new rtc::RefCountedObject<WebRTCConductor>(&m_WebRTCClient, this));

Error:
	return r;
}

RESULT WebRTCImp::CreateNewURLRequest(std::wstring& strURL) {
	return R_NOT_IMPLEMENTED;
}

// Will simply update the message loop as needed
RESULT WebRTCImp::Update() {
	return R_NOT_IMPLEMENTED;
}

// WebRTC Specific

RESULT WebRTCImp::QueueUIThreadCallback(int msg_id, void* data) {
	RESULT r = R_PASS;
	
	CB(::PostThreadMessage(m_UIThreadID, UI_THREAD_CALLBACK, static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(data)));

Error:
	return r;
}